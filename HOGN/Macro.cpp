#include "Macro.h"
#include "Globals.h"

// ============================================================================
// Recording
// ============================================================================
// All recorders are called from the global hooks. We hold actions_mutex
// briefly to keep playback (running on its own thread) safe from concurrent
// vector growth.

void recordClick(int button, int state) {
	actionRecord record;
	record.action = 0;
	record.value1 = state;   // 0 down, 1 up
	record.value2 = button;  // 0 left, 1 right
	std::lock_guard<std::mutex> lk(actions_mutex);
	actions.push_back(record);
}

void recordKeyboard(int keyChar, int state) {
	actionRecord record;
	record.action = 1;
	record.value1 = state;
	record.value2 = keyChar;
	std::lock_guard<std::mutex> lk(actions_mutex);
	actions.push_back(record);
}

// Mouse move events fire many times per second. Two efficiency tweaks:
//   1. We trust the hook-supplied (x, y) instead of calling GetCursorPos
//      again (the original code did the latter, which is racy and wasteful).
//   2. If the most recent recorded event is also a move, we *coalesce*: just
//      update its x/y instead of appending. The macro file shrinks by orders
//      of magnitude and playback no longer has to Sleep(10) between every
//      intermediate cursor sample.
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
// Low-level replay primitives
// ============================================================================

void replayMouseClick(int button, int state) {
	INPUT input[1] = {};
	input[0].type = INPUT_MOUSE;
	if (button == 0)
		input[0].mi.dwFlags = (state == 0) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
	else if (button == 1)
		input[0].mi.dwFlags = (state == 0) ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
	SendInput(1, input, sizeof(INPUT));
}

void replayKeyboardAction(int vkCode, int state) {
	INPUT input[1] = {};
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = (WORD)vkCode;
	input[0].ki.dwFlags = (state == 0) ? 0 : KEYEVENTF_KEYUP;
	SendInput(1, input, sizeof(INPUT));
}

void replayMouseMove(int x, int y) {
	SetCursorPos(x, y);
}

// ============================================================================
// Playback thread
// ============================================================================
// Spawned by StartMacroPlayback. The thread holds a private copy of `actions`
// taken under the mutex so the user can RECORD again safely as soon as
// playback's snapshot is taken (though we still gate RECORD on !action_doing).

namespace {

void PlaybackThreadProc(int repeat_num) {
	std::vector<actionRecord> snapshot;
	{
		std::lock_guard<std::mutex> lk(actions_mutex);
		snapshot = actions;
	}

	int last_action_num = 2;
	for (int r = 0; r < repeat_num && action_doing.load(); r++) {
		for (const auto& record : snapshot) {
			while (action_paused.load() && action_doing.load())
				Sleep(50);
			if (!action_doing.load()) break;

			switch (record.action) {
			case 0:
				Sleep(last_action_num != 0 ? 300 : 100);
				if (!action_doing.load()) break;
				replayMouseClick(record.value2, record.value1);
				last_action_num = 0;
				break;
			case 1:
				Sleep(last_action_num != 1 ? 300 : 100);
				if (!action_doing.load()) break;
				replayKeyboardAction(record.value2, record.value1);
				last_action_num = 1;
				break;
			case 2:
				Sleep(10);
				if (!action_doing.load()) break;
				replayMouseMove(record.value1, record.value2);
				last_action_num = 2;
				break;
			}
		}
	}
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
