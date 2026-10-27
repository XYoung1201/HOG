#pragma once
#include "HOGN.h"

// Tray icon and main hidden window. The window is HWND_MESSAGE-only — it
// exists solely to host the tray callback, the menu, and the
// WM_USER_RELOAD_CONFIG marshalling target.

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void windowCreate();
void toggleTrayIcon();
void toggleTrayIconVisibility();
