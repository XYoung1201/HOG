#include "Tray.h"
#include "Globals.h"
#include "Command.h"
#import <Cocoa/Cocoa.h>

// Menu bar status item. The menu mirrors the Windows tray popup menu; the
// icon switches between hands.png and hand_gray.png like IDI_ICON /
// IDI_ICON_GRAY.

@interface HOGTrayTarget : NSObject <NSMenuDelegate>
@end

namespace {

NSStatusItem*  g_statusItem = nil;
HOGTrayTarget* g_target = nil;

// Runs a built-in command with the listening gate temporarily opened, the
// same trick WndProc's WM_COMMAND handler uses.
void runForced(const char* cmd) {
	bool prev = listening;
	listening = true;
	runCommand(cmd);
	listening = prev;
}

NSImage* trayImage() {
	NSString* name = listening ? @"hands" : @"hand_gray";
	NSImage* img = [NSImage imageNamed:name];
	img.size = NSMakeSize(18, 18);
	return img;
}

} // anonymous namespace

@implementation HOGTrayTarget
- (void)toggleListening:(id)s { toggleTrayIcon(); }
- (void)editConfig:(id)s      { runForced("CONFIG"); }
- (void)reloadConfig:(id)s    { runForced("RELOAD"); }
- (void)changeConfig:(id)s    { runCommand("CHANGECONF"); }
- (void)showQr:(id)s          { runForced("QRCODE"); }
- (void)closeDirs:(id)s       { runForced("CLOSEALLDIRECTORIES"); }
- (void)quit:(id)s            { runForced("QUIT"); }

- (void)menuNeedsUpdate:(NSMenu*)menu {
	[menu removeAllItems];
	auto add = ^(NSString* title, SEL action) {
		NSMenuItem* item = [menu addItemWithTitle:title action:action keyEquivalent:@""];
		item.target = self;
	};
	add(listening ? @"Stop Listening" : @"Start Listening", @selector(toggleListening:));
	add(@"Edit Config",            @selector(editConfig:));
	add(@"Reload Config",          @selector(reloadConfig:));
	add(@"Change Config",          @selector(changeConfig:));
	add(@"Show Clipboard QRcode",  @selector(showQr:));
	add(@"Close all Directories",  @selector(closeDirs:));
	[menu addItem:NSMenuItem.separatorItem];
	add(@"Exit",                   @selector(quit:));
}
@end

void refreshTrayIcon() {
	if (g_statusItem)
		g_statusItem.button.image = trayImage();
}

void trayCreate() {
	g_target = [[HOGTrayTarget alloc] init];
	g_statusItem = [NSStatusBar.systemStatusBar statusItemWithLength:NSSquareStatusItemLength];
	g_statusItem.button.toolTip = @"HOG";
	g_statusItem.behavior = NSStatusItemBehaviorRemovalAllowed;
	NSMenu* menu = [[NSMenu alloc] init];
	menu.delegate = g_target;
	g_statusItem.menu = menu;
	refreshTrayIcon();
	trayIconVisible = true;
}

void toggleTrayIconVisibility() {
	trayIconVisible = !trayIconVisible;
	if (g_statusItem)
		g_statusItem.visible = trayIconVisible;
}

void toggleTrayIcon() {
	listening = !listening;
	refreshTrayIcon();
}
