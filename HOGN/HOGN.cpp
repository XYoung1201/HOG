// ============================================================================
// HOGN — main entry point.
//
// Everything else lives in its own translation unit:
//   Toast.cpp         : ShowToastMessage and the layered toast window
//   ConfigFile.cpp    : config file I/O and first-run setup
//   ConfigWatcher.cpp : auto-reload after the editor saves
//   ConfigTrie.cpp    : parse + walk the hotkey trie
//   Macro.cpp         : record + threaded playback
//   Command.cpp       : runCommand + excuteTar dispatch
//   Hooks.cpp         : KeyboardProc + MouseProc
//   Tray.cpp          : tray icon, popup menu, hidden main window
//   Clipboard.cpp     : clipboard ops + keystroke simulation
//   SoundControl.cpp  : volume / mute
//   SystemControl.cpp : shutdown / reboot / privileges
//   ShellOps.cpp      : openTar, env vars, explorer windows
//   QrCodeWindow.cpp  : clipboard QR popup
//   WinUtils.cpp      : DPI, foreground class check, drawing helpers
//   Globals.cpp       : global-state definitions
// ============================================================================

#include "HOGN.h"
#include "Globals.h"
#include "ConfigFile.h"
#include "ConfigTrie.h"
#include "Hooks.h"
#include "Tray.h"
#include "WinUtils.h"
#include "resource.h"

int main() {
	HANDLE hMutex = CreateMutex(NULL, TRUE, "Global\\HOGNMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(GetForegroundWindow(),
			_T("An instance of this application is already running!"),
			_T("Warning"), MB_OK);
		return -1;
	}

	// GDI+ powers the toast renderer; init before anything that might toast.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

	trayIconVisible = true;
	listening = true;
	action_listening = false;
	action_doing = false;
	action_paused = false;
	lastTime = high_resolution_clock::now();

	para_path   = "C:\\ProgramData\\HOG\\config";
	config_path = "C:\\ProgramData\\HOG\\HOG.conf";
	para_dir    = "C:\\ProgramData\\HOG";

	firstInitialJudge();
	windowCreate();
	readPara();

	myhook    = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetCurrentModule(), 0);
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL,    MouseProc,    GetCurrentModule(), 0);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(g_gdiplusToken);
	return 0;
}
