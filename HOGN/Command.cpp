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

#include <unordered_map>
#include <functional>

// ============================================================================
// Command dispatch
// ============================================================================
// Built-in commands are looked up in a hash table — O(1) on average vs. the
// long if/else chain we used to have. Each handler is a no-arg function that
// reads/writes globals as needed.

void writeText(std::string text) {
	simulateWrite(text);
}

namespace {

// ---- helpers ----
std::string TruncateForToast(const std::string& s, size_t maxLen = 40) {
	if (s.size() <= maxLen) return s;
	return s.substr(0, maxLen - 3) + "...";
}

// ---- individual command handlers ----

void cmd_quit() {
	int result = MessageBox(GetForegroundWindow(),
		CString("Do you want to quit HOGN?"), CString("Quit"), MB_OKCANCEL);
	if (result == IDOK) {
		UnhookWindowsHookEx(myhook);
		UnhookWindowsHookEx(mouseHook);
		clearMem(o);
		exit(0);
	}
}

void cmd_close_all_directories() { closeAllDirectories(); }

void cmd_reload() {
	clearMem(o);
	readPara();
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
	openTar(config_path);
	ShowToastMessage("Config opened");
	StartConfigWatcher();
}

void cmd_mute_toggle()          { toggleMute(); }
void cmd_shutdown()              { shutdown(); }
void cmd_reboot()                { reboot(); }
void cmd_cancel_shutdown()       { cancelShutdown(); }
void cmd_qrcode()                { printClipboard(); }
void cmd_open_cmd_in_curr_path() { ShowToastMessage("OPENCMDINCURRENTPATH"); }

void cmd_google_search() {
	openTar(std::string("https://google.com/search?q=") + getClipboardText());
}

void cmd_close_this_window() {
	HWND hwnd = GetForegroundWindow();
	if (hwnd)
		PostMessage(hwnd, WM_CLOSE, 0, 0);
}

void cmd_copy_pdf_text() { EditPDFText(getClipboardText()); }

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
		// Strip the hotkey events that triggered start/stop from the buffer.
		std::lock_guard<std::mutex> lk(actions_mutex);
		if (mark && (int)actions.size() >= mark->num * 3)
			actions.erase(actions.end() - mark->num * 3 + 1, actions.end());
		if (!actions.empty())
			actions.erase(actions.begin(), actions.begin() + 1);
		ShowToastMessage("Macro recording stopped");
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
//
// Built once at program start; lookup is O(1) average for the entire set.
using CommandFn = void(*)();
const std::unordered_map<std::string, CommandFn>& Dispatch() {
	static const std::unordered_map<std::string, CommandFn> kTable = {
		{ "QUIT",                  cmd_quit                  },
		{ "CLOSEALLDIRECTORIES",   cmd_close_all_directories },
		{ "RELOAD",                cmd_reload                },
		{ "CHANGECONF",            cmd_changeconf            },
		{ "CONFIG",                cmd_config                },
		{ "MUTETOGGLE",            cmd_mute_toggle           },
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
		listening = !listening;
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
