#include "SoundControl.h"
#include "ShellOps.h"

void mute()   { runAppleScript("set volume output muted true"); }
void unmute() { runAppleScript("set volume output muted false"); }

void toggleMute() {
	std::string muted;
	runAppleScript(
		"set m to not (output muted of (get volume settings))\n"
		"set volume output muted m\n"
		"return m as text", &muted);
	ShowToastMessage(muted == "true" ? "Muted" : "Unmuted");
}
