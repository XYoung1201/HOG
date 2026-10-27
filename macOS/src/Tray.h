#pragma once
#include "HOGN.h"

// Menu bar (status item) icon and its menu — the macOS stand-in for the
// Windows tray icon.

void trayCreate();
void toggleTrayIcon();
void toggleTrayIconVisibility();
void refreshTrayIcon();
