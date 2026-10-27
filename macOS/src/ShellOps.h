#pragma once
#include "HOGN.h"

// Shell / NSWorkspace / Finder-control helpers.

// Expands %VAR% (Windows style), $VAR / ${VAR} and a leading ~.
std::string replaceEnvironmentVariables(const std::string& input);

// Open a file, folder, app, URL or script. With asAdmin=true the target is
// run through `do shell script ... with administrator privileges`.
void openTar(std::string target, bool asAdmin = false);

// Run an AppleScript source string; returns false on error.
bool runAppleScript(const std::string& source, std::string* result = nullptr);

void closeAllDirectories();
void openTerminalInCurrentPath();
