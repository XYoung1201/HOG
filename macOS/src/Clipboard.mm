#include "Clipboard.h"
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>

void setClipboardText(const std::string& text) {
	NSPasteboard* pb = NSPasteboard.generalPasteboard;
	[pb clearContents];
	[pb setString:[NSString stringWithUTF8String:text.c_str()] ?: @"" forType:NSPasteboardTypeString];
}

std::string getClipboardText() {
	NSString* s = [NSPasteboard.generalPasteboard stringForType:NSPasteboardTypeString];
	return s ? std::string(s.UTF8String) : std::string();
}

int getClipboardNumber() {
	std::string clipboardText = getClipboardText();
	auto it = clipboardText.begin();
	while (it != clipboardText.end() && std::isdigit(static_cast<unsigned char>(*it))) ++it;
	if (!clipboardText.empty() && it == clipboardText.end()) {
		try { return std::stoi(clipboardText); } catch (...) {}
	}
	return 1;
}

void EditPDFText(std::string str) {
	// Joins lines broken by the PDF layout while keeping the line break after
	// a sentence end ("." / "。"), then collapses doubled spaces.
	auto replaceAll = [&str](const std::string& from, const std::string& to) {
		std::size_t pos = 0;
		while ((pos = str.find(from, pos)) != std::string::npos) {
			str.replace(pos, from.length(), to);
			pos += to.length();
		}
	};
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
	replaceAll("\xE3\x80\x82\n", "\xE3\x80\x82\x01");  // 。
	replaceAll(".\n", ".\x01");
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	while (str.find("  ") != std::string::npos)
		replaceAll("  ", " ");
	replaceAll("\x01", "\n");
	setClipboardText(str);
}

void postKey(int keyCode, bool down, uint64_t flags) {
	CGEventRef e = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)keyCode, down);
	if (!e) return;
	CGEventSetFlags(e, (CGEventFlags)flags);
	CGEventSetIntegerValueField(e, kCGEventSourceUserData, HOGN_EVENT_MARK);
	CGEventPost(kCGHIDEventTap, e);
	CFRelease(e);
}

void simulatePaste() {
	postKey(kVK_ANSI_V, true, kCGEventFlagMaskCommand);
	postKey(kVK_ANSI_V, false, kCGEventFlagMaskCommand);
}

void pasteText(const std::string& text) {
	NSPasteboard* pb = NSPasteboard.generalPasteboard;
	NSMutableArray<NSPasteboardItem*>* saved = [NSMutableArray array];
	for (NSPasteboardItem* item in pb.pasteboardItems) {
		NSPasteboardItem* copy = [[NSPasteboardItem alloc] init];
		for (NSPasteboardType type in item.types) {
			NSData* data = [item dataForType:type];
			if (data) [copy setData:data forType:type];
		}
		[saved addObject:copy];
	}
	setClipboardText(text);
	simulatePaste();
	// Give the target app time to read the pasteboard before restoring it.
	NSInteger changeCount = pb.changeCount;
	dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(400 * NSEC_PER_MSEC)),
		dispatch_get_main_queue(), ^{
			if (pb.changeCount != changeCount) return;  // something else copied meanwhile
			[pb clearContents];
			if (saved.count) [pb writeObjects:saved];
		});
}

// Types the text through Unicode keyboard events, so any character (CJK,
// emoji, symbols) works regardless of the active keyboard layout.
void simulateWrite(std::string str) {
	NSString* text = [NSString stringWithUTF8String:str.c_str()];
	if (!text) return;
	// Newlines are sent as the Return key.
	NSUInteger len = text.length;
	NSUInteger i = 0;
	while (i < len) {
		NSRange r = [text rangeOfComposedCharacterSequenceAtIndex:i];
		NSString* piece = [text substringWithRange:r];
		if ([piece isEqualToString:@"\n"]) {
			postKey(kVK_Return, true);
			postKey(kVK_Return, false);
		} else {
			UniChar buf[8];
			NSUInteger n = MIN(piece.length, (NSUInteger)8);
			[piece getCharacters:buf range:NSMakeRange(0, n)];
			for (bool down : { true, false }) {
				CGEventRef e = CGEventCreateKeyboardEvent(NULL, 0, down);
				CGEventSetFlags(e, 0);
				CGEventKeyboardSetUnicodeString(e, n, buf);
				CGEventSetIntegerValueField(e, kCGEventSourceUserData, HOGN_EVENT_MARK);
				CGEventPost(kCGHIDEventTap, e);
				CFRelease(e);
			}
		}
		usleep(2000);
		i = NSMaxRange(r);
	}
}

void TouchBackspace(int letterNumber) {
	for (int i = 0; i < letterNumber; i++) {
		postKey(kVK_Delete, true);
		postKey(kVK_Delete, false);
		usleep(15000);
	}
}
