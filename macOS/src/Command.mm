#include "Command.h"
#include "Globals.h"
#include "Clipboard.h"
#include "SoundControl.h"
#include "SystemControl.h"
#include "ShellOps.h"
#include "QrCodeWindow.h"
#include "ConfigFile.h"
#include "ConfigWatcher.h"
#include "ConfigTrie.h"
#include "Macro.h"
#include "Tray.h"
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

#include <unordered_map>

// ============================================================================
// Command dispatch
// ============================================================================
// Same table and semantics as the Windows build; only the handlers' bodies
// are platform specific.

void writeText(std::string text) {
	pasteText(text);
}

namespace {

// ---- helpers ----
std::string TruncateForToast(const std::string& s, size_t maxLen = 40) {
	if (s.size() <= maxLen) return s;
	return s.substr(0, maxLen - 3) + "...";
}

std::string UrlEncode(const std::string& s) {
	NSString* str = [NSString stringWithUTF8String:s.c_str()] ?: @"";
	NSMutableCharacterSet* allowed = NSCharacterSet.URLQueryAllowedCharacterSet.mutableCopy;
	[allowed removeCharactersInString:@"&=+?#"];
	NSString* enc = [str stringByAddingPercentEncodingWithAllowedCharacters:allowed];
	return enc ? std::string(enc.UTF8String) : "";
}

// ---- individual command handlers ----

void cmd_quit() {
	[NSApp activateIgnoringOtherApps:YES];
	NSAlert* alert = [[NSAlert alloc] init];
	alert.messageText = @"Quit";
	alert.informativeText = @"Do you want to quit HOGN?";
	[alert addButtonWithTitle:@"OK"];
	[alert addButtonWithTitle:@"Cancel"];
	if ([alert runModal] == NSAlertFirstButtonReturn) {
		clearMem(o);
		exit(0);
	}
}

void cmd_close_all_directories() { closeAllDirectories(); }

void cmd_reload() {
	clearMem(o);
	readPara();
	refreshTrayIcon();
	ShowToastMessage("Reloaded config");
}

void cmd_changeconf() {
	std::string customPath = GetCustomSavePath();
	if (customPath.empty()) return;
	config_path = customPath;
	cmd_reload();
	WriteStringToFile(para_path, config_path);
}

void cmd_config() {
	openConfigInEditor();
	ShowToastMessage("Config opened");
	StartConfigWatcher();
}

void cmd_mute_toggle()          { toggleMute(); }
void cmd_shutdown()              { shutdown(); }
void cmd_reboot()                { reboot(); }
void cmd_cancel_shutdown()       { cancelShutdown(); }
void cmd_qrcode()                { printClipboard(); }
void cmd_open_cmd_in_curr_path() { openTerminalInCurrentPath(); }

void cmd_google_search() {
	openTar(std::string("https://google.com/search?q=") + UrlEncode(getClipboardText()));
}

void cmd_close_this_window() {
	postKey(kVK_ANSI_W, true, kCGEventFlagMaskCommand);
	postKey(kVK_ANSI_W, false, kCGEventFlagMaskCommand);
}

void cmd_copy_pdf_text() {
	EditPDFText(getClipboardText());
	ShowToastMessage("Clipboard text reformatted");
}

void cmd_record() {
	if (action_doing.load()) {
		ShowToastMessage("Cannot record while playing macro");
		return;
	}
	action_listening = !action_listening;
	if (action_listening) {
		{
			std::lock_guard<std::mutex> lk(actions_mutex);
			actions.clear();
		}
		ShowToastMessage("Macro recording started");
	} else {
		// Strip the hotkey that stopped recording (one down + one up event
		// per letter) and any mouse moves interleaved with it.
		std::lock_guard<std::mutex> lk(actions_mutex);
		int keysToDrop = g_lastTriggerLength * 2;
		while (!actions.empty() && keysToDrop > 0) {
			if (actions.back().action == 1) keysToDrop--;
			actions.pop_back();
		}
		ShowToastMessage("Macro recording stopped (" + std::to_string(actions.size()) + " events)");
	}
}

void cmd_action() {
	StartMacroPlayback(getClipboardNumber());
}

void cmd_action_stop() {
	if (action_doing.load()) {
		action_doing = false;
		action_paused = false;
	} else {
		ShowToastMessage("No macro playing");
	}
}

void cmd_action_pause() {
	if (action_doing.load() && !action_paused.load()) {
		action_paused = true;
		ShowToastMessage("Macro playback paused");
	}
}

void cmd_action_resume() {
	if (action_doing.load() && action_paused.load()) {
		action_paused = false;
		ShowToastMessage("Macro playback resumed");
	}
}

void cmd_action_pause_toggle() {
	if (!action_doing.load()) return;
	if (action_paused.load()) {
		action_paused = false;
		ShowToastMessage("Macro playback resumed");
	} else {
		action_paused = true;
		ShowToastMessage("Macro playback paused");
	}
}

// ---- dispatch table ----
using CommandFn = void(*)();
const std::unordered_map<std::string, CommandFn>& Dispatch() {
	static const std::unordered_map<std::string, CommandFn> kTable = {
		{ "QUIT",                  cmd_quit                  },
		{ "CLOSEALLDIRECTORIES",   cmd_close_all_directories },
		{ "RELOAD",                cmd_reload                },
		{ "CHANGECONF",            cmd_changeconf            },
		{ "CONFIG",                cmd_config                },
		{ "MUTETOGGLE",            cmd_mute_toggle           },
		{ "MUTEREVERSE",           cmd_mute_toggle           },
		{ "SHUTDOWN",              cmd_shutdown              },
		{ "REBOOT",                cmd_reboot                },
		{ "CANCELSHUTDOWNORREBOOT", cmd_cancel_shutdown      },
		{ "QRCODE",                cmd_qrcode                },
		{ "OPENCMDINCURRENTPATH",  cmd_open_cmd_in_curr_path },
		{ "GOOGLESEARCH",          cmd_google_search         },
		{ "CLOSETHISWINDOW",       cmd_close_this_window     },
		{ "COPYPDFTEXT",           cmd_copy_pdf_text         },
		{ "RECORD",                cmd_record                },
		{ "ACTION",                cmd_action                },
		{ "ACTIONSTOP",            cmd_action_stop           },
		{ "ACTIONPAUSE",           cmd_action_pause          },
		{ "ACTIONRESUME",          cmd_action_resume         },
		{ "ACTIONPAUSETOGGLE",     cmd_action_pause_toggle   },
	};
	return kTable;
}

} // anonymous namespace

void runCommand(std::string command) {
	if (listening) {
		const auto& tbl = Dispatch();
		auto it = tbl.find(command);
		if (it != tbl.end()) {
			it->second();
			return;
		}
	}

	// ONOFFHOGTOGGLE intentionally bypasses the listening gate so the user
	// can re-enable HOG from its paused state.
	if (command == "ONOFFHOGTOGGLE") {
		toggleTrayIcon();
		ShowToastMessage(listening ? "HOG listening" : "HOG paused");
		return;
	}

	if (listening)
		ShowToastMessage("Unknown command: " + command);
}

void excuteTar(std::string* tar) {
	std::string text   = extractContent(*tar, "TEXT:");
	std::string cmd    = extractContent(*tar, "CMD:");
	std::string set    = extractContent(*tar, "SET:");
	std::string path   = extractContent(*tar, "PATH:");
	std::string path_A = extractContent(*tar, "PATH_A:");
	try {
		if (!text.empty() && listening) {
			ShowToastMessage("Triggered: text input");
			writeText(text);
		}
		else if (!cmd.empty()) {
			runCommand(cmd);
		}
		else if (!set.empty() && listening) {
			ShowToastMessage("Triggered: " + TruncateForToast(set));
			handleSet(set);
		}
		else if (!path.empty() && listening) {
			ShowToastMessage("Opening: " + TruncateForToast(path));
			openTar(path);
		}
		else if (!path_A.empty() && listening) {
			ShowToastMessage("Opening (admin): " + TruncateForToast(path_A));
			openTar(path_A, true);
		}
	}
	catch (...) {
		ShowToastMessage("Configure Wrong!");
	}
}
