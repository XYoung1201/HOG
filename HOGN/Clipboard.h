#pragma once
#include "HOGN.h"

// ============================================================================
// Clipboard + keystroke simulation utilities.
//
// The keystroke helpers (simulatePush / simulateWrite / TouchBackspace) live
// here because their main consumer is the clipboard-paste/text-typing flow.
// ============================================================================

std::map<UINT, HANDLE> saveClipboardData();
void restoreClipboardData(const std::map<UINT, HANDLE>& savedData);

void setClipboardText(const std::string& text);
std::string getClipboardText();

// Parses the clipboard text as a positive integer (used to choose how many
// times to repeat macro playback). Returns 1 if the clipboard isn't a number.
int getClipboardNumber();

// Reformats PDF-copied text (joins broken lines, collapses spaces) and puts
// the result back on the clipboard.
void EditPDFText(std::string str);

// Keystroke simulation.
void simulatePaste();
void simulatePush(char c);
void simulateWrite(std::string str);
void TouchBackspace(int letterNumber);
