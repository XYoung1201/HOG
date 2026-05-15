#pragma once

// ============================================================================
// Project-wide header.
//
// Pulls in every system / std header the modules need, declares the
// project-wide types and message constants, and forward-declares the
// notification helper used everywhere. Module-specific declarations live
// in their own headers; global state lives in Globals.h.
// ============================================================================

// --- std ---
#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <time.h>
#include <chrono>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <regex>
#include <cstdlib>
#include <tchar.h>
#include <wchar.h>
#include <io.h>
#include <direct.h>

// --- windows ---
#include <windows.h>
#include <commdlg.h>
#include <ShlObj.h>
#include <ShObjIdl.h>
#include <UIAutomation.h>
#include <atlbase.h>
#include <atlstr.h>
#include <atlcom.h>
#include <MMDeviceAPI.h>
#include <EndpointVolume.h>
#include <gdiplus.h>

// --- third party ---
#include "qrcodegen.hpp"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")

using namespace std;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

// ----------------------------------------------------------------------------
// Message ids
// ----------------------------------------------------------------------------
#define WM_TRAYICON           (WM_USER + 1)
#define WM_CONFIGMESSAGE      (WM_USER + 2)
#define WM_USER_RELOAD_CONFIG (WM_USER + 3)

#define IDI_ICON                101
#define IDI_ICON_GRAY           108
#define ID_TRAY_EXIT            9001
#define ID_TRAY_RELOAD          9002
#define ID_TRAY_CONFIG          9003
#define ID_TRAY_PAUSE           9004
#define ID_TRAY_START           9005
#define ID_TRAY_QRCODE          9006
#define ID_TRAY_CLOSEDIRECTORIES 9007
#define ID_TRAY_CHANGECONF      9008

// ----------------------------------------------------------------------------
// Project types
// ----------------------------------------------------------------------------

// Sometimes-used: used in shell helpers.
typedef struct windowWithProcessAndHWND {
	DWORD processID;
	HWND m_hwnd;
} wPH;

// Compact trie node for hotkey lookup. Each node has 28 children: 0..25 for
// A..Z, 26 for left-Ctrl (vk 0xA2), 27 for right-Ctrl (vk 0xA3). Leaf-payload
// `a` stores the configured action string (TEXT:.. / CMD:.. / PATH:.. / ...).
typedef struct letter {
	string* a;
	int num;
	struct letter* letters[28];
	letter(struct letter* o) {
		if (o == NULL)
			o = this;
		for (struct letter*& i : letters)
			i = o;
		a = NULL;
		num = 0;
	}
} ltr;

// One recorded input event during macro capture.
//   action == 0 -> mouse click,  value1 = state (0 down / 1 up), value2 = button (0 left / 1 right)
//   action == 1 -> keyboard,     value1 = state,                value2 = vk code
//   action == 2 -> mouse move,   value1 = x,                    value2 = y
struct actionRecord {
	int action;
	int value1;
	int value2;
};

// Public façade — implemented in Toast.cpp. Declared here because almost
// every module calls it.
void ShowToastMessage(const std::string& message);
