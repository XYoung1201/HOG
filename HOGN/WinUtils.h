#pragma once
#include "HOGN.h"

// Misc window / desktop / DPI helpers shared across modules.

double getzoom();
HMODULE GetCurrentModule();

// Helpers used by the QR-code window's custom text rendering.
void DrawTextWithLineSpacing(HDC hdc, const TCHAR* text, int count, RECT* rect, UINT format, int lineSpacing);

// True if the foreground window is a File-Explorer window. Used to suppress
// trie-backspace behaviour in those windows.
bool isActiveWindowExplorer();
