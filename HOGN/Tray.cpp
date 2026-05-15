#include "Tray.h"
#include "Globals.h"
#include "Command.h"
#include "ShellOps.h"

void toggleTrayIconVisibility() {
	if (trayIconVisible)
		Shell_NotifyIcon(NIM_DELETE, &nid);
	else
		Shell_NotifyIcon(NIM_ADD, &nid);
	trayIconVisible = !trayIconVisible;
}

void toggleTrayIcon() {
	listening = !listening;
	nid.hIcon = LoadIcon(GetModuleHandle(NULL),
		MAKEINTRESOURCE(listening ? IDI_ICON : IDI_ICON_GRAY));
	Shell_NotifyIcon(NIM_MODIFY, &nid);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	const bool listeningState = listening;
	switch (msg) {
	case WM_CREATE: {
		g_mainHwnd = hwnd;
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = 14735;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = WM_TRAYICON;
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
		strcpy_s(nid.szTip, _countof(nid.szTip), "HOG");
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD, &nid);
		break;
	}
	case WM_USER_RELOAD_CONFIG: {
		// Marshalled here from the config-watcher thread so the trie reload
		// happens on the same thread as the hook callbacks.
		bool prev = listening;
		listening = true;
		runCommand("RELOAD");
		listening = prev;
		return 0;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TRAY_EXIT:
			listening = true;
			runCommand("QUIT");
			listening = listeningState;
			break;
		case ID_TRAY_RELOAD:
			listening = true;
			runCommand("RELOAD");
			listening = listeningState;
			break;
		case ID_TRAY_CONFIG:
			listening = true;
			runCommand("CONFIG");
			listening = listeningState;
			break;
		case ID_TRAY_PAUSE:
			if (listening) toggleTrayIcon();
			break;
		case ID_TRAY_START:
			if (!listening) toggleTrayIcon();
			break;
		case ID_TRAY_QRCODE:
			listening = true;
			runCommand("QRCODE");
			listening = listeningState;
			break;
		case ID_TRAY_CLOSEDIRECTORIES:
			listening = true;
			runCommand("CLOSEALLDIRECTORIES");
			listening = listeningState;
			break;
		case ID_TRAY_CHANGECONF:
			runCommand("CHANGECONF");
			break;
		}
		break;
	case WM_TRAYICON:
		switch (lParam) {
		case WM_LBUTTONUP:
			openTar("C:\\ProgramData\\HOG\\HOG.conf");
			break;
		case WM_RBUTTONUP: {
			HMENU hMenu = CreatePopupMenu();
			if (listening)
				AppendMenu(hMenu, MF_STRING, ID_TRAY_PAUSE, TEXT("Stop Listening"));
			else
				AppendMenu(hMenu, MF_STRING, ID_TRAY_START, TEXT("Start Listening"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_CONFIG,            TEXT("Edit Config"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_RELOAD,            TEXT("Reload Config"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_CHANGECONF,        TEXT("Change Config"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_QRCODE,            TEXT("Show Clipboard QRcode"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_CLOSEDIRECTORIES,  TEXT("Close all Directories"));
			AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT,              TEXT("Exit"));

			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hwnd);
			UINT selectedMenu = TrackPopupMenu(hMenu,
				TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_RETURNCMD,
				pt.x, pt.y, 0, hwnd, NULL);
			if (selectedMenu != 0)
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(selectedMenu, 0), 0);
			SendMessage(hwnd, WM_NULL, 0, 0);
			DestroyMenu(hMenu);
			break;
		}
		}
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void windowCreate() {
	// NB: the class name lives in static storage so the WNDCLASSEX pointer
	// stays valid across RegisterClassEx + CreateWindowEx. (The original
	// passed a CString rvalue here and got lucky with stale memory.)
	static const TCHAR kClassName[] = _T("hiddenWindowClass");

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = kClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		return;

	CreateWindowEx(0, kClassName, NULL, 0, 0, 0, 0, 0,
		HWND_MESSAGE, NULL, wc.hInstance, NULL);
	// g_mainHwnd is captured inside WM_CREATE.
}
