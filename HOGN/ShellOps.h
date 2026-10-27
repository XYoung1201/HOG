#pragma once
#include "HOGN.h"

// Shell / ShellExecute / explorer-control helpers.

std::string replaceEnvironmentVariables(const std::string& input);

// Open a file, URL or shell target. Set asAdmin=true to run with the "runas"
// verb. The function tries to bring the launched window to the foreground.
void openTar(std::string target, bool asAdmin = false);

// EnumWindowsProc callbacks used internally.
BOOL CALLBACK getRightHWND(HWND hwnd, LPARAM lParam);
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

void closeAllDirectories();
