#pragma once
#include "HOGN.h"

// Low-level keyboard and mouse hook callbacks. Installed in main().

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
