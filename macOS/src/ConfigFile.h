#pragma once
#include "HOGN.h"

// ============================================================================
// Config file I/O and first-run setup.
//
// The trie parser lives in ConfigTrie.h. This module covers reading/writing
// the file from disk and the one-time install prompt.
// ============================================================================

bool AskToUseDefaultPath();
std::string GetCustomSavePath();
void configFileSet();

// Open the active config in the default plain-text editor (.conf files
// have no default app on macOS).
void openConfigInEditor();

std::string ReadFileToString(const std::string& filePath);
bool WriteStringToFile(const std::string& filePath, const std::string& content);

// Ensure the config dir exists and the config file is present (writing the
// default `config_text` if missing). Persists the user-chosen path to para_path.
void firstInitialJudge();
