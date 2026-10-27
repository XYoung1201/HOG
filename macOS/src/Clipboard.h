#pragma once
#include "HOGN.h"

// ============================================================================
// Clipboard + keystroke simulation utilities (NSPasteboard + CGEvent).
// ============================================================================

void setClipboardText(const std::string& text);
std::string getClipboardText();

// Parses the clipboard text as a positive integer (used to choose how many
// times to repeat macro playback). Returns 1 if the clipboard isn't a number.
int getClipboardNumber();

// Reformats PDF-copied text (joins broken lines, collapses spaces) and puts
// the result back on the clipboard.
void EditPDFText(std::string str);

// Keystroke simulation. Every posted event carries HOGN_EVENT_MARK.
void postKey(int keyCode, bool down, uint64_t flags = 0);
void simulatePaste();
// Pastes text through the clipboard, then restores the previous clipboard.
// Unlike typing, this isn't intercepted by input methods such as Pinyin.
void pasteText(const std::string& text);
void simulateWrite(std::string str);
void TouchBackspace(int letterNumber);
