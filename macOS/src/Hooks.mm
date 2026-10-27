#include "Hooks.h"
#include "Globals.h"
#include "Clipboard.h"    // TouchBackspace
#include "ConfigTrie.h"   // mark/o/getNextLetter/...
#include "Macro.h"        // recordKeyboard/recordClick/recordMove
#include "Command.h"      // excuteTar
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

// ============================================================================
// Global event tap — replaces KeyboardProc / MouseProc from the Windows build.
//
// The trie walk is identical to the Windows hook; macOS key codes are first
// translated to the Windows virtual-key numbering the trie expects. Actions
// are dispatched asynchronously so the tap callback returns immediately
// (macOS disables taps whose callbacks are slow).
// ============================================================================

namespace {

CFMachPortRef g_tap = nullptr;

// macOS ANSI key code -> Windows virtual key ('A'..'Z', Command, Backspace).
int MacKeyToVk(int keyCode) {
	switch (keyCode) {
	case kVK_ANSI_A: return 'A'; case kVK_ANSI_B: return 'B'; case kVK_ANSI_C: return 'C';
	case kVK_ANSI_D: return 'D'; case kVK_ANSI_E: return 'E'; case kVK_ANSI_F: return 'F';
	case kVK_ANSI_G: return 'G'; case kVK_ANSI_H: return 'H'; case kVK_ANSI_I: return 'I';
	case kVK_ANSI_J: return 'J'; case kVK_ANSI_K: return 'K'; case kVK_ANSI_L: return 'L';
	case kVK_ANSI_M: return 'M'; case kVK_ANSI_N: return 'N'; case kVK_ANSI_O: return 'O';
	case kVK_ANSI_P: return 'P'; case kVK_ANSI_Q: return 'Q'; case kVK_ANSI_R: return 'R';
	case kVK_ANSI_S: return 'S'; case kVK_ANSI_T: return 'T'; case kVK_ANSI_U: return 'U';
	case kVK_ANSI_V: return 'V'; case kVK_ANSI_W: return 'W'; case kVK_ANSI_X: return 'X';
	case kVK_ANSI_Y: return 'Y'; case kVK_ANSI_Z: return 'Z';
	case kVK_Command:      return VK_LWIN;
	case kVK_RightCommand: return VK_RWIN;
	case kVK_Delete:       return VK_BACK;
	default:               return 0;
	}
}

// Finder is the macOS counterpart of Explorer: there the typed hotkey
// letters don't land in a text field, so no backspaces are sent.
bool isActiveWindowFinder() {
	NSRunningApplication* app = NSWorkspace.sharedWorkspace.frontmostApplication;
	return [app.bundleIdentifier isEqualToString:@"com.apple.finder"];
}

void handleKey(int vk, bool down, bool injected) {
	auto thisTime = high_resolution_clock::now();
	int interval_ms = (int)std::chrono::duration_cast<milliseconds>(thisTime - lastTime).count();

	if (!down) {
		lastTime = thisTime;
		keyStateChange(vk, false);
		if (mark && mark->a != NULL) {
			std::string action = *mark->a;
			int backspaces = (!isActiveWindowFinder() && listening) ? mark->num : 0;
			bool cmdHeld = not_up[26] || not_up[27];
			bool run = !cmdHeld && !action_doing.load();
			int num = mark->num;
			// Backspaces go out from a background queue so the tap keeps
			// running, and the action waits until the target app (and an
			// input method such as Pinyin) has consumed them — otherwise an
			// action that takes focus, like QRCODE, swallows the rest.
			dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INTERACTIVE, 0), ^{
				TouchBackspace(backspaces);
				if (backspaces > 0)
					usleep(120 * 1000);
				if (!run) return;
				dispatch_async(dispatch_get_main_queue(), ^{
					std::string a = action;
					g_lastTriggerLength = num;
					excuteTar(&a);
				});
			});
			mark = o;
		}
	} else {
		// Playback control hotkeys, ignored for synthesized input so a macro
		// can't stop or pause itself.
		if (!injected && action_doing.load()) {
			if (vk == 'Q') {
				action_doing = false;
			} else if (vk == 'P') {
				action_paused = !action_paused.load();
				ShowToastMessage(action_paused.load() ? "Macro playback paused" : "Macro playback resumed");
			}
		}
		if (!getKeyState(vk))
			lastTime = thisTime;
		keyStateChange(vk, true);
		mark = getNextLetter(mark, vk);
	}

	if (interval_ms > 1000)
		mark = getNextLetter(o, vk);
}

CGEventRef TapCallback(CGEventTapProxy, CGEventType type, CGEventRef event, void*) {
	if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
		if (g_tap) CGEventTapEnable(g_tap, true);
		return event;
	}

	bool injected = CGEventGetIntegerValueField(event, kCGEventSourceUserData) == HOGN_EVENT_MARK;
	bool canRecord = action_listening && !action_doing.load() && !injected;

	switch (type) {
	case kCGEventKeyDown:
	case kCGEventKeyUp: {
		int keyCode = (int)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
		bool down = type == kCGEventKeyDown;
		if (down && CGEventGetIntegerValueField(event, kCGKeyboardEventAutorepeat))
			break;
		// Our own backspaces / typed text must not walk the trie, otherwise
		// a TEXT: payload could re-trigger a hotkey.
		if (!injected)
			handleKey(MacKeyToVk(keyCode), down, injected);
		if (canRecord)
			recordKeyboard(keyCode, down ? 1 : 0);
		break;
	}
	case kCGEventFlagsChanged: {
		int keyCode = (int)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
		uint64_t flags = CGEventGetFlags(event);
		if (!injected && (keyCode == kVK_Command || keyCode == kVK_RightCommand)) {
			// Command is down when its flag is set and this side wasn't
			// already down; the keyCode tells left from right.
			bool down = (flags & kCGEventFlagMaskCommand) != 0 && !getKeyState(MacKeyToVk(keyCode));
			handleKey(MacKeyToVk(keyCode), down, injected);
		}
		if (canRecord)
			recordModifiers(keyCode, flags);
		break;
	}
	case kCGEventLeftMouseDown:  if (canRecord) recordClick(0, 0); break;
	case kCGEventLeftMouseUp:    if (canRecord) recordClick(0, 1); break;
	case kCGEventRightMouseDown: if (canRecord) recordClick(1, 0); break;
	case kCGEventRightMouseUp:   if (canRecord) recordClick(1, 1); break;
	case kCGEventMouseMoved:
	case kCGEventLeftMouseDragged:
	case kCGEventRightMouseDragged:
		if (canRecord) {
			CGPoint p = CGEventGetLocation(event);
			recordMove((int)p.x, (int)p.y);
		}
		break;
	default:
		break;
	}
	return event;
}

} // anonymous namespace

bool installEventTap() {
	CGEventMask mask = CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventKeyUp)
		| CGEventMaskBit(kCGEventFlagsChanged)
		| CGEventMaskBit(kCGEventLeftMouseDown) | CGEventMaskBit(kCGEventLeftMouseUp)
		| CGEventMaskBit(kCGEventRightMouseDown) | CGEventMaskBit(kCGEventRightMouseUp)
		| CGEventMaskBit(kCGEventMouseMoved)
		| CGEventMaskBit(kCGEventLeftMouseDragged) | CGEventMaskBit(kCGEventRightMouseDragged);

	g_tap = CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap,
		kCGEventTapOptionListenOnly, mask, TapCallback, nullptr);
	if (!g_tap)
		return false;

	CFRunLoopSourceRef src = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, g_tap, 0);
	CFRunLoopAddSource(CFRunLoopGetMain(), src, kCFRunLoopCommonModes);
	CFRelease(src);
	CGEventTapEnable(g_tap, true);
	return true;
}
