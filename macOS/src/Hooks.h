#pragma once
#include "HOGN.h"

// Global keyboard + mouse event tap (the macOS counterpart of the Windows
// WH_KEYBOARD_LL / WH_MOUSE_LL hooks). Returns false if the tap couldn't be
// created, which means the Accessibility / Input Monitoring permission is
// missing.
bool installEventTap();
