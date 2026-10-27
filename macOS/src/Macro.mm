#include "Macro.h"
#include "Globals.h"
#include "Clipboard.h"
#import <ApplicationServices/ApplicationServices.h>

// ============================================================================
// Recording
// ============================================================================
// All recorders are called from the event tap. We hold actions_mutex briefly
// to keep playback (running on its own thread) safe from concurrent growth.

void recordClick(int button, int state) {
	actionRecord record;
	record.action = 0;
	record.value1 = state;   // 0 down, 1 up
	record.value2 = button;  // 0 left, 1 right
	std::lock_guard<std::mutex> lk(actions_mutex);
	actions.push_back(record);
}

void recordKeyboard(int keyCode, int state) {
	actionRecord record;
	record.action = 1;
	record.value1 = state;
	record.value2 = keyCode;
	std::lock_guard<std::mutex> lk(actions_mutex);
	actions.push_back(record);
}

// Modifier keys (Shift, Ctrl, Option, Cmd) arrive as flags-changed events on
// macOS; we store the resulting flag mask so playback can reproduce it.
void recordModifiers(int keyCode, uint64_t flags) {
	actionRecord record;
	record.action = 3;
	record.value1 = (int)(uint32_t)flags;
	record.value2 = keyCode;
	std::lock_guard<std::mutex> lk(actions_mutex);
	actions.push_back(record);
}

// Consecutive mouse moves are coalesced into one record (see the Windows
// Macro.cpp for the rationale).
void recordMove(int x, int y) {
	std::lock_guard<std::mutex> lk(actions_mutex);
	if (!actions.empty() && actions.back().action == 2) {
		actions.back().value1 = x;
		actions.back().value2 = y;
		return;
	}
	actionRecord record;
	record.action = 2;
	record.value1 = x;
	record.value2 = y;
	actions.push_back(record);
}

// ============================================================================
// Replay primitives
// ============================================================================

namespace {

constexpr uint64_t kModifierMask = kCGEventFlagMaskShift | kCGEventFlagMaskControl
	| kCGEventFlagMaskAlternate | kCGEventFlagMaskCommand | kCGEventFlagMaskSecondaryFn;

struct ReplayState {
	uint64_t flags = 0;          // modifiers currently "held" by the macro
	bool buttonDown[2] = { false, false };
};

void postMouse(CGEventType type, CGPoint p, CGMouseButton button, uint64_t flags) {
	CGEventRef e = CGEventCreateMouseEvent(NULL, type, p, button);
	if (!e) return;
	CGEventSetFlags(e, (CGEventFlags)flags);
	CGEventSetIntegerValueField(e, kCGEventSourceUserData, HOGN_EVENT_MARK);
	CGEventPost(kCGHIDEventTap, e);
	CFRelease(e);
}

CGPoint currentCursor() {
	CGEventRef e = CGEventCreate(NULL);
	CGPoint p = CGEventGetLocation(e);
	CFRelease(e);
	return p;
}

void replayMouseClick(ReplayState& st, int button, int state) {
	bool left = button == 0;
	CGEventType type = left ? (state == 0 ? kCGEventLeftMouseDown : kCGEventLeftMouseUp)
	                        : (state == 0 ? kCGEventRightMouseDown : kCGEventRightMouseUp);
	st.buttonDown[left ? 0 : 1] = (state == 0);
	postMouse(type, currentCursor(), left ? kCGMouseButtonLeft : kCGMouseButtonRight, st.flags);
}

void replayMouseMove(ReplayState& st, int x, int y) {
	CGPoint p = CGPointMake(x, y);
	if (st.buttonDown[0])
		postMouse(kCGEventLeftMouseDragged, p, kCGMouseButtonLeft, st.flags);
	else if (st.buttonDown[1])
		postMouse(kCGEventRightMouseDragged, p, kCGMouseButtonRight, st.flags);
	else
		postMouse(kCGEventMouseMoved, p, kCGMouseButtonLeft, st.flags);
}

void replayModifiers(ReplayState& st, int keyCode, uint32_t flags) {
	st.flags = flags & kModifierMask;
	CGEventRef e = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)keyCode, true);
	if (!e) return;
	CGEventSetType(e, kCGEventFlagsChanged);
	CGEventSetFlags(e, (CGEventFlags)st.flags);
	CGEventSetIntegerValueField(e, kCGEventSourceUserData, HOGN_EVENT_MARK);
	CGEventPost(kCGHIDEventTap, e);
	CFRelease(e);
}

// ============================================================================
// Playback thread
// ============================================================================

void PlaybackThreadProc(int repeat_num) {
	std::vector<actionRecord> snapshot;
	{
		std::lock_guard<std::mutex> lk(actions_mutex);
		snapshot = actions;
	}

	ReplayState st;
	int last_action_num = 2;
	for (int r = 0; r < repeat_num && action_doing.load(); r++) {
		for (const auto& record : snapshot) {
			while (action_paused.load() && action_doing.load())
				usleep(50 * 1000);
			if (!action_doing.load()) break;

			switch (record.action) {
			case 0:
				usleep((last_action_num != 0 ? 300 : 100) * 1000);
				if (!action_doing.load()) break;
				replayMouseClick(st, record.value2, record.value1);
				last_action_num = 0;
				break;
			case 1:
			case 3:
				usleep((last_action_num != 1 ? 300 : 100) * 1000);
				if (!action_doing.load()) break;
				if (record.action == 1)
					postKey(record.value2, record.value1 == 1, st.flags);
				else
					replayModifiers(st, record.value2, (uint32_t)record.value1);
				last_action_num = 1;
				break;
			case 2:
				usleep(10 * 1000);
				if (!action_doing.load()) break;
				replayMouseMove(st, record.value1, record.value2);
				last_action_num = 2;
				break;
			}
		}
	}

	// Never leave a modifier or mouse button stuck down.
	if (st.flags)
		replayModifiers(st, 0x38 /* kVK_Shift */, 0);
	for (int b = 0; b < 2; b++)
		if (st.buttonDown[b])
			replayMouseClick(st, b, 1);

	bool stopped_by_user = !action_doing.load();
	action_doing = false;
	action_paused = false;
	ShowToastMessage(stopped_by_user ? "Macro playback stopped" : "Macro playback finished");
}

} // anonymous namespace

bool StartMacroPlayback(int repeatCount) {
	if (action_listening) {
		ShowToastMessage("Cannot play macro while recording");
		return false;
	}
	if (action_doing.load()) {
		ShowToastMessage("Macro already playing");
		return false;
	}
	{
		std::lock_guard<std::mutex> lk(actions_mutex);
		if (actions.empty()) {
			ShowToastMessage("No macro recorded");
			return false;
		}
	}
	if (repeatCount < 1) repeatCount = 1;

	action_doing = true;
	action_paused = false;
	ShowToastMessage("Macro playback started");
	std::thread(PlaybackThreadProc, repeatCount).detach();
	return true;
}
