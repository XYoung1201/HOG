#pragma once
#include "HOGN.h"

// ============================================================================
// Command dispatch.
//
// runCommand: handler for built-in CMD:* values (RELOAD, CONFIG, ACTION, ...)
// excuteTar:  routes a configured action string (TEXT:/CMD:/SET:/PATH:/PATH_A:)
//             to the right implementation and emits an appropriate toast.
// ============================================================================

void writeText(std::string text);
void runCommand(std::string command);
void excuteTar(std::string* tar);
