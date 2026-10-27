#pragma once
#include "HOGN.h"

// ============================================================================
// Global state. Declared extern here so every module sees the same instances.
// Definitions (and any default initialization) live in Globals.mm.
// ============================================================================

// --- Config paths ---
extern std::string para_path;     // path of the meta file recording where the config lives
extern std::string config_path;   // active config file path
extern std::string para_dir;      // directory holding both of the above
extern const std::string config_text;  // default config contents written on first install

// --- Trie & hotkey state ---
extern struct letter* o;          // sentinel "no child" pointer
extern struct letter* mark;       // current trie position while walking key events
extern bool not_up[28];           // which letter/modifier slots are currently pressed
extern int char_down;
extern high_resolution_clock::time_point lastTime;
extern int g_lastTriggerLength;   // letter count of the hotkey that triggered the running action

// --- Tray state ---
extern bool trayIconVisible;

// --- Listening / mode flags ---
extern bool listening;            // master enable for hotkey commands
extern bool action_listening;     // recording state
extern std::atomic<bool> action_doing;     // macro playback active
extern std::atomic<bool> action_paused;    // macro playback paused

// --- Recorded macro events ---
extern std::vector<struct actionRecord> actions;
extern std::mutex actions_mutex;  // protects 'actions' between tap recording and playback thread

// --- Config watcher ---
extern std::atomic<bool> g_watching_config;
