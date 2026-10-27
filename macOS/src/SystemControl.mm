#include "SystemControl.h"
#include "ShellOps.h"
#import <Foundation/Foundation.h>

// macOS has no InitiateSystemShutdownEx with a timeout, so HOGN keeps the
// 20-second countdown itself and asks System Events to shut down / restart
// when it expires. CANCELSHUTDOWNORREBOOT simply invalidates the countdown.

namespace {

constexpr int kShutdownDelaySeconds = 20;
NSUInteger g_shutdownGeneration = 0;
bool g_shutdownPending = false;

void scheduleSystemEvent(const char* verb, const char* label) {
	NSUInteger gen = ++g_shutdownGeneration;
	g_shutdownPending = true;
	std::string verbStr(verb);
	ShowToastMessage(std::string(label) + " in " + std::to_string(kShutdownDelaySeconds) + "s");
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)kShutdownDelaySeconds * NSEC_PER_SEC),
		dispatch_get_main_queue(), ^{
			if (gen != g_shutdownGeneration || !g_shutdownPending) return;
			g_shutdownPending = false;
			runAppleScript("tell application \"System Events\" to " + verbStr);
		});
}

} // anonymous namespace

void cancelShutdown() {
	if (!g_shutdownPending) {
		ShowToastMessage("No shutdown or reboot pending");
		return;
	}
	g_shutdownPending = false;
	++g_shutdownGeneration;
	ShowToastMessage("Shutdown / reboot cancelled");
}

void shutdown() { scheduleSystemEvent("shut down", "Shutting down"); }
void reboot()   { scheduleSystemEvent("restart",   "Restarting"); }
