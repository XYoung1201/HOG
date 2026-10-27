#pragma once

// ============================================================================
// Project-wide header (macOS port).
//
// Mirrors HOGN/HOGN.h from the Windows build: pulls in the std headers the
// modules need, declares the project-wide types, and forward-declares the
// notification helper used everywhere. Global state lives in Globals.h.
//
// Key codes: the trie keeps the Windows virtual-key numbering ('A'..'Z' =
// 65..90, Backspace = 8) so ConfigTrie stays close to the Windows version.
// The `*` / `+` slots are bound to left / right Command (VK_LWIN / VK_RWIN,
// the Windows keys sitting where ⌘ is) instead of Windows' Ctrl. Hooks.mm
// translates macOS key codes into these values before walking the trie.
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

// --- third party ---
#include "../../HOGN/qrcodegen.hpp"

using namespace std;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

// Windows-style virtual key codes used by the trie.
#define VK_BACK      8
#define VK_LWIN      0x5B  // left Command  (`*` in the config)
#define VK_RWIN      0x5C  // right Command (`+` in the config)

// Marker written into kCGEventSourceUserData of every event HOGN posts, so
// the event tap can recognise (and ignore) its own synthesized input — the
// macOS counterpart of LLKHF_INJECTED.
#define HOGN_EVENT_MARK 0x484F474E  // 'HOGN'

// ----------------------------------------------------------------------------
// Project types
// ----------------------------------------------------------------------------

// Compact trie node for hotkey lookup. Each node has 28 children: 0..25 for
// A..Z, 26 for left-Command, 27 for right-Command. Leaf-payload `a` stores the
// configured action string (TEXT:.. / CMD:.. / PATH:.. / ...).
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
//   action == 1 -> keyboard,     value1 = state,                value2 = macOS key code
//   action == 2 -> mouse move,   value1 = x,                    value2 = y
//   action == 3 -> modifiers,    value1 = CGEventFlags (low 32 bits), value2 = macOS key code
struct actionRecord {
	int action;
	int value1;
	int value2;
};

// Public façade — implemented in Toast.mm. Safe to call from any thread.
void ShowToastMessage(const std::string& message);
