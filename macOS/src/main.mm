// ============================================================================
// HOGN — macOS entry point.
//
// Module layout mirrors the Windows build (HOGN/HOGN.cpp):
//   Toast.mm         : ShowToastMessage and the floating toast panel
//   ConfigFile.mm    : config file I/O and first-run setup
//   ConfigWatcher.mm : auto-reload after the editor saves
//   ConfigTrie.cpp   : parse + walk the hotkey trie (shared logic)
//   Macro.mm         : record + threaded playback
//   Command.mm       : runCommand + excuteTar dispatch
//   Hooks.mm         : global CGEvent tap
//   Tray.mm          : menu bar icon and menu
//   Clipboard.mm     : NSPasteboard ops + keystroke simulation
//   SoundControl.mm  : mute
//   SystemControl.mm : shutdown / reboot
//   ShellOps.mm      : openTar, env vars, Finder windows
//   QrCodeWindow.mm  : clipboard QR popup
//   Globals.mm       : global-state definitions
// ============================================================================

#include "HOGN.h"
#include "Globals.h"
#include "ConfigFile.h"
#include "ConfigTrie.h"
#include "Hooks.h"
#include "Tray.h"
#import <Cocoa/Cocoa.h>
#import <ApplicationServices/ApplicationServices.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace {

// Single-instance lock: the macOS counterpart of the Global\HOGNMutex.
bool acquireInstanceLock(const std::string& dir) {
	std::string lockPath = dir + "/.hogn.lock";
	int fd = open(lockPath.c_str(), O_CREAT | O_RDWR, 0644);
	return fd >= 0 && flock(fd, LOCK_EX | LOCK_NB) == 0;  // fd stays open for the process lifetime
}

void showAlert(NSString* title, NSString* text) {
	[NSApp activateIgnoringOtherApps:YES];
	NSAlert* alert = [[NSAlert alloc] init];
	alert.messageText = title;
	alert.informativeText = text;
	[alert runModal];
}

// The event tap needs Accessibility (to post keystrokes) and Input
// Monitoring (to observe them). Prompt, then keep retrying until granted.
void startTapWhenPermitted() {
	NSDictionary* opts = @{ (__bridge NSString*)kAXTrustedCheckOptionPrompt: @YES };
	AXIsProcessTrustedWithOptions((__bridge CFDictionaryRef)opts);
	CGRequestListenEventAccess();

	if (AXIsProcessTrusted() && installEventTap()) {
		NSLog(@"HOGN: event tap running");
		ShowToastMessage("HOG is running");
		return;
	}
	NSLog(@"HOGN: waiting for Accessibility / Input Monitoring access");
	ShowToastMessage("Grant HOGN Accessibility + Input Monitoring access");
	__block NSTimer* timer = [NSTimer scheduledTimerWithTimeInterval:2.0 repeats:YES block:^(NSTimer* t) {
		if (AXIsProcessTrusted() && installEventTap()) {
			[t invalidate];
			NSLog(@"HOGN: event tap running");
			ShowToastMessage("HOG is running");
		}
	}];
	(void)timer;
}

} // anonymous namespace

int main(int argc, const char* argv[]) {
	@autoreleasepool {
		NSApplication* app = NSApplication.sharedApplication;
		app.activationPolicy = NSApplicationActivationPolicyAccessory;  // no Dock icon

		NSString* support = [NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,
			NSUserDomainMask, YES).firstObject stringByAppendingPathComponent:@"HOG"];
		para_dir    = support.UTF8String;
		para_path   = para_dir + "/config";
		config_path = para_dir + "/HOG.conf";

		mkdir(para_dir.c_str(), 0755);
		if (!acquireInstanceLock(para_dir)) {
			showAlert(@"Warning", @"An instance of this application is already running!");
			return -1;
		}

		trayIconVisible = true;
		listening = true;
		action_listening = false;
		action_doing = false;
		action_paused = false;
		lastTime = high_resolution_clock::now();

		trayCreate();
		firstInitialJudge();
		readPara();
		startTapWhenPermitted();

		[app run];
	}
	return 0;
}
