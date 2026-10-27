#pragma once
#include "HOGN.h"

// ============================================================================
// Config trie: builds and walks the prefix-tree of hotkey definitions
// loaded from the user's config file.
//
// Key mapping inside a trie node's `letters[]` array:
//   index 0..25 -> A..Z          (vk 'A'..'Z' minus 'A')
//   index 26    -> left Ctrl     (vk 0xA2)
//   index 27    -> right Ctrl    (vk 0xA3)
// All other vk codes fall back to the sentinel `o`.
// ============================================================================

bool hasPrefix(const std::string& str, const std::string& prefix);
std::string extractContent(const std::string& str, const std::string& prefix);

// Parse a "SET:KEY:ON/OFF" instruction and apply it at load time.
void loadSet(std::string set);
// Parse a runtime "SET:KEY" toggle (e.g. invoked from a hotkey).
void handleSet(std::string set);

// Insert a single config line (e.g. "ABC CMD:RELOAD") into the trie.
void commandLoad(std::string word);

// Build the trie from the config file at `config_path`.
void readPara();
// Release the trie. Must be called before re-loading or on shutdown.
void clearMem(struct letter* node);

// Track Ctrl / letter key state so the hook can know what's currently held.
void keyStateChange(int code, bool state);
bool getKeyState(int code);

// Walk one step in the trie for vk `code` from `lastCode`. Returns the
// sentinel `o` when there's no match.
struct letter* getNextLetter(struct letter* lastCode, int code);
