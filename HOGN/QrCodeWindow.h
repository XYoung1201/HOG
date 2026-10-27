#pragma once
#include "HOGN.h"

// QR-code popup window for visualizing clipboard text as a QR.

void ShowCustomWindow(const TCHAR* displayText);

// Top-level entry: build a QR for the current clipboard text and show it.
void printClipboard();
