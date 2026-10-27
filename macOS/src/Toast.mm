#include "HOGN.h"
#import <Cocoa/Cocoa.h>

// ============================================================================
// Toast notification
// ============================================================================
// Non-activating, rounded-corner panel near the bottom of the screen with the
// mouse. Calling ShowToastMessage from any thread updates the displayed text
// and restarts the fade-in / hold / fade-out animation — same look and
// timings as the Windows GDI+ toast.

namespace {

constexpr CGFloat TOAST_WIDTH         = 360;
constexpr CGFloat TOAST_HEIGHT        = 60;
constexpr CGFloat TOAST_CORNER        = 16;
constexpr CGFloat TOAST_BOTTOM_MARGIN = 140;
constexpr double  TOAST_FADE_IN_S     = 0.18;
constexpr double  TOAST_HOLD_S        = 1.8;
constexpr double  TOAST_FADE_OUT_S    = 0.42;
constexpr CGFloat TOAST_MAX_ALPHA     = 235.0 / 255.0;

NSPanel*     g_toastPanel = nil;
NSTextField* g_toastLabel = nil;
NSUInteger   g_toastGeneration = 0;

void EnsureToastPanel() {
	if (g_toastPanel) return;

	g_toastPanel = [[NSPanel alloc]
		initWithContentRect:NSMakeRect(0, 0, TOAST_WIDTH, TOAST_HEIGHT)
		          styleMask:NSWindowStyleMaskBorderless | NSWindowStyleMaskNonactivatingPanel
		            backing:NSBackingStoreBuffered
		              defer:NO];
	g_toastPanel.opaque = NO;
	g_toastPanel.backgroundColor = NSColor.clearColor;
	g_toastPanel.level = NSStatusWindowLevel;
	g_toastPanel.ignoresMouseEvents = YES;
	g_toastPanel.hasShadow = YES;
	g_toastPanel.collectionBehavior = NSWindowCollectionBehaviorCanJoinAllSpaces
		| NSWindowCollectionBehaviorFullScreenAuxiliary
		| NSWindowCollectionBehaviorStationary;

	NSView* bg = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, TOAST_WIDTH, TOAST_HEIGHT)];
	bg.wantsLayer = YES;
	bg.layer.backgroundColor = [NSColor colorWithSRGBRed:28/255.0 green:28/255.0 blue:32/255.0 alpha:1].CGColor;
	bg.layer.cornerRadius = TOAST_CORNER;
	bg.layer.borderWidth = 1;
	bg.layer.borderColor = [NSColor colorWithWhite:1 alpha:0.4].CGColor;
	g_toastPanel.contentView = bg;

	g_toastLabel = [NSTextField labelWithString:@""];
	g_toastLabel.font = [NSFont systemFontOfSize:15];
	g_toastLabel.textColor = [NSColor colorWithWhite:245/255.0 alpha:1];
	g_toastLabel.alignment = NSTextAlignmentCenter;
	g_toastLabel.lineBreakMode = NSLineBreakByTruncatingTail;
	g_toastLabel.frame = NSMakeRect(10, (TOAST_HEIGHT - 22) / 2, TOAST_WIDTH - 20, 22);
	[bg addSubview:g_toastLabel];
}

NSScreen* ScreenWithMouse() {
	NSPoint p = NSEvent.mouseLocation;
	for (NSScreen* s in NSScreen.screens)
		if (NSPointInRect(p, s.frame))
			return s;
	return NSScreen.mainScreen;
}

void ShowToastOnMain(NSString* text) {
	EnsureToastPanel();
	g_toastLabel.stringValue = text;

	NSRect vf = ScreenWithMouse().visibleFrame;
	[g_toastPanel setFrameOrigin:NSMakePoint(NSMidX(vf) - TOAST_WIDTH / 2,
	                                         NSMinY(vf) + TOAST_BOTTOM_MARGIN)];

	NSUInteger gen = ++g_toastGeneration;
	if (!g_toastPanel.visible)
		g_toastPanel.alphaValue = 0;
	[g_toastPanel orderFrontRegardless];
	[NSAnimationContext runAnimationGroup:^(NSAnimationContext* ctx) {
		ctx.duration = TOAST_FADE_IN_S;
		g_toastPanel.animator.alphaValue = TOAST_MAX_ALPHA;
	} completionHandler:nil];

	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)((TOAST_FADE_IN_S + TOAST_HOLD_S) * NSEC_PER_SEC)),
		dispatch_get_main_queue(), ^{
			if (gen != g_toastGeneration) return;
			[NSAnimationContext runAnimationGroup:^(NSAnimationContext* ctx) {
				ctx.duration = TOAST_FADE_OUT_S;
				g_toastPanel.animator.alphaValue = 0;
			} completionHandler:^{
				if (gen == g_toastGeneration)
					[g_toastPanel orderOut:nil];
			}];
		});
}

} // anonymous namespace

void ShowToastMessage(const std::string& message) {
	NSString* text = [NSString stringWithUTF8String:message.c_str()] ?: @"";
	if (NSThread.isMainThread)
		ShowToastOnMain(text);
	else
		dispatch_async(dispatch_get_main_queue(), ^{ ShowToastOnMain(text); });
}
