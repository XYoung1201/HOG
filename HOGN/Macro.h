#pragma once
#include "HOGN.h"

// ============================================================================
// Macro recording + playback.
//
// Recording is driven by the global hooks (Hooks.cpp). Playback runs on a
// detached thread spawned by StartMacroPlayback() so the keyboard hook stays
// responsive to pause/stop hotkeys across multi-round repeats.
//
// State lives in Globals.h:
//   - actions             : recorded event buffer (guarded by actions_mutex)
//   - action_listening    : recording is active
//   - action_doing        : playback is active
//   - action_paused       : playback is paused
// ============================================================================

// Recording sinks – called from the LL hooks.
void recordClick(int button, int state);
void recordKeyboard(int keyChar, int state);
void recordMove(int x, int y);

// Low-level replay primitives – also useful as building blocks.
void replayMouseClick(int button, int state);
void replayKeyboardAction(int vkCode, int state);
void replayMouseMove(int x, int y);

// Launch the playback thread. Returns false if playback couldn't be started
// (already running, nothing to play, currently recording).
bool StartMacroPlayback(int repeatCount);
