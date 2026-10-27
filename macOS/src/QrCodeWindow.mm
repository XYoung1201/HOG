#include "QrCodeWindow.h"
#include "Clipboard.h"
#import <Cocoa/Cocoa.h>

// Draws the QR modules as crisp black squares on white, with the source text
// underneath. Esc or Cmd+W closes the window.

@interface HOGQrView : NSView
@property (nonatomic) std::vector<bool> modules;
@property (nonatomic) int size;
@end

@implementation HOGQrView
- (BOOL)isFlipped { return YES; }
- (void)drawRect:(NSRect)dirty {
	[NSColor.whiteColor setFill];
	NSRectFill(self.bounds);
	const int border = 4;
	int total = self.size + border * 2;
	CGFloat side = MIN(self.bounds.size.width, self.bounds.size.height);
	CGFloat cell = floor(side / total);
	CGFloat ox = floor((self.bounds.size.width - cell * total) / 2);
	CGFloat oy = floor((self.bounds.size.height - cell * total) / 2);
	[NSColor.blackColor setFill];
	for (int y = 0; y < self.size; y++)
		for (int x = 0; x < self.size; x++)
			if (self.modules[y * self.size + x])
				NSRectFill(NSMakeRect(ox + (x + border) * cell, oy + (y + border) * cell, cell, cell));
}
@end

@interface HOGQrWindow : NSWindow
@end

@implementation HOGQrWindow
- (void)cancelOperation:(id)sender { [self close]; }
- (void)keyDown:(NSEvent*)e {
	if (e.keyCode == 53) [self close];   // Esc
	else [super keyDown:e];
}
@end

namespace {
NSMutableArray<NSWindow*>* g_qrWindows = nil;  // keeps open windows alive
}

void printClipboard() {
	std::string text = getClipboardText();
	if (text.empty()) {
		ShowToastMessage("Clipboard has no text");
		return;
	}
	try {
		const QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::LOW);
		int size = qr.getSize();
		std::vector<bool> modules(size * size);
		for (int y = 0; y < size; y++)
			for (int x = 0; x < size; x++)
				modules[y * size + x] = qr.getModule(x, y);

		const CGFloat qrSide = 320, pad = 12, labelH = 44;
		HOGQrWindow* win = [[HOGQrWindow alloc]
			initWithContentRect:NSMakeRect(0, 0, qrSide, qrSide + labelH)
			          styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable
			            backing:NSBackingStoreBuffered
			              defer:NO];
		win.title = @"QRCode of Clipboard Text";
		win.releasedWhenClosed = NO;
		win.level = NSFloatingWindowLevel;
		win.backgroundColor = NSColor.whiteColor;

		HOGQrView* view = [[HOGQrView alloc] initWithFrame:NSMakeRect(0, labelH, qrSide, qrSide)];
		view.modules = modules;
		view.size = size;
		[win.contentView addSubview:view];

		NSTextField* label = [NSTextField wrappingLabelWithString:
			[NSString stringWithUTF8String:text.c_str()] ?: @""];
		label.frame = NSMakeRect(pad, 4, qrSide - pad * 2, labelH - 8);
		label.alignment = NSTextAlignmentCenter;
		label.textColor = NSColor.darkGrayColor;
		label.font = [NSFont systemFontOfSize:11];
		label.maximumNumberOfLines = 2;
		label.lineBreakMode = NSLineBreakByTruncatingTail;
		[win.contentView addSubview:label];

		if (!g_qrWindows) g_qrWindows = [NSMutableArray array];
		[g_qrWindows addObject:win];
		__block id token = [NSNotificationCenter.defaultCenter
			addObserverForName:NSWindowWillCloseNotification object:win queue:nil
			        usingBlock:^(NSNotification*) {
				[g_qrWindows removeObject:win];
				[NSNotificationCenter.defaultCenter removeObserver:token];
			}];

		[win center];
		[NSApp activateIgnoringOtherApps:YES];
		[win makeKeyAndOrderFront:nil];
	}
	catch (const qrcodegen::data_too_long&) {
		ShowToastMessage("Text too long for QR code");
	}
	catch (...) {
	}
}
