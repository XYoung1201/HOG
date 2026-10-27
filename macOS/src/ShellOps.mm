#include "ShellOps.h"
#import <Cocoa/Cocoa.h>

std::string replaceEnvironmentVariables(const std::string& input) {
	std::string result = input;
	if (!result.empty() && result[0] == '~') {
		const char* home = std::getenv("HOME");
		if (home && (result.size() == 1 || result[1] == '/'))
			result.replace(0, 1, home);
	}
	// %VAR% (kept for configs carried over from Windows), ${VAR} and $VAR.
	std::regex env_var_pattern(R"(%(\w+)%|\$\{(\w+)\}|\$([A-Za-z_]\w*))");
	std::string out;
	auto begin = std::sregex_iterator(result.begin(), result.end(), env_var_pattern);
	std::size_t last = 0;
	for (auto it = begin; it != std::sregex_iterator(); ++it) {
		const std::smatch& m = *it;
		std::string name = m[1].matched ? m[1].str() : m[2].matched ? m[2].str() : m[3].str();
		const char* value = std::getenv(name.c_str());
		out += result.substr(last, m.position() - last);
		out += value ? value : "";
		last = m.position() + m.length();
	}
	out += result.substr(last);
	return out;
}

bool runAppleScript(const std::string& source, std::string* result) {
	NSString* src = [NSString stringWithUTF8String:source.c_str()];
	NSAppleScript* script = [[NSAppleScript alloc] initWithSource:src];
	NSDictionary* error = nil;
	NSAppleEventDescriptor* desc = [script executeAndReturnError:&error];
	if (error) {
		NSLog(@"HOGN AppleScript error: %@", error);
		return false;
	}
	if (result)
		*result = desc.stringValue ? std::string(desc.stringValue.UTF8String) : "";
	return true;
}

namespace {

std::string AppleScriptQuote(const std::string& s) {
	std::string out = "\"";
	for (char c : s) {
		if (c == '"' || c == '\\') out += '\\';
		out += c;
	}
	return out + "\"";
}

std::string ShellQuote(const std::string& s) {
	std::string out = "'";
	for (char c : s) {
		if (c == '\'') out += "'\\''";
		else out += c;
	}
	return out + "'";
}

bool IsExecutableScript(NSString* path) {
	BOOL isDir = NO;
	NSFileManager* fm = NSFileManager.defaultManager;
	if (![fm fileExistsAtPath:path isDirectory:&isDir] || isDir) return false;
	NSString* ext = path.pathExtension.lowercaseString;
	if ([@[@"sh", @"command", @"zsh", @"bash", @"py"] containsObject:ext]) return true;
	return [fm isExecutableFileAtPath:path] && ext.length == 0;
}

} // anonymous namespace

void openTar(std::string target, bool asAdmin) {
	std::string expanded = replaceEnvironmentVariables(target);
	NSString* t = [NSString stringWithUTF8String:expanded.c_str()];
	if (!t.length) return;

	if (asAdmin) {
		std::string cmd = IsExecutableScript(t)
			? ShellQuote(expanded)
			: "/usr/bin/open " + ShellQuote(expanded);
		// osascript in its own process so the password prompt never blocks
		// the event tap on the main thread.
		NSTask* task = [[NSTask alloc] init];
		task.executableURL = [NSURL fileURLWithPath:@"/usr/bin/osascript"];
		std::string src = "do shell script " + AppleScriptQuote(cmd) + " with administrator privileges";
		task.arguments = @[@"-e", [NSString stringWithUTF8String:src.c_str()]];
		[task launchAndReturnError:nil];
		return;
	}

	// URLs (http, mailto, x-apple.systempreferences:, ...).
	NSURL* url = [NSURL URLWithString:t];
	if (url.scheme.length > 1 && ![t hasPrefix:@"/"]) {
		[NSWorkspace.sharedWorkspace openURL:url];
		return;
	}

	// Executable scripts run in Terminal so their output is visible.
	if (IsExecutableScript(t)) {
		NSURL* fileURL = [NSURL fileURLWithPath:t];
		NSURL* terminal = [NSURL fileURLWithPath:@"/System/Applications/Utilities/Terminal.app"];
		[NSWorkspace.sharedWorkspace openURLs:@[fileURL]
		                 withApplicationAtURL:terminal
		                        configuration:[NSWorkspaceOpenConfiguration configuration]
		                    completionHandler:nil];
		return;
	}

	if ([NSFileManager.defaultManager fileExistsAtPath:t]) {
		NSWorkspaceOpenConfiguration* cfg = [NSWorkspaceOpenConfiguration configuration];
		cfg.activates = YES;
		[NSWorkspace.sharedWorkspace openURL:[NSURL fileURLWithPath:t]
		                       configuration:cfg
		                   completionHandler:nil];
		return;
	}

	// Bare application names, e.g. "PATH:Safari".
	NSURL* app = [NSWorkspace.sharedWorkspace URLForApplicationWithBundleIdentifier:t];
	for (NSString* dir in @[@"/Applications", @"/System/Applications",
	                        @"/System/Applications/Utilities", [@"~/Applications" stringByExpandingTildeInPath]]) {
		if (app) break;
		NSString* candidate = [dir stringByAppendingPathComponent:[t stringByAppendingPathExtension:@"app"]];
		if ([NSFileManager.defaultManager fileExistsAtPath:candidate])
			app = [NSURL fileURLWithPath:candidate];
	}
	if (app) {
		[NSWorkspace.sharedWorkspace openApplicationAtURL:app
		                                    configuration:[NSWorkspaceOpenConfiguration configuration]
		                                completionHandler:nil];
		return;
	}
	ShowToastMessage("Not found: " + expanded);
}

void closeAllDirectories() {
	runAppleScript("tell application \"Finder\" to close every window");
}

// Opens a Terminal window at the folder of the frontmost Finder window
// (or the Desktop when no Finder window is open).
void openTerminalInCurrentPath() {
	std::string dir;
	runAppleScript(
		"tell application \"Finder\"\n"
		"  if (count of Finder windows) > 0 then\n"
		"    return POSIX path of (target of front Finder window as alias)\n"
		"  else\n"
		"    return POSIX path of (desktop as alias)\n"
		"  end if\n"
		"end tell", &dir);
	if (dir.empty()) {
		const char* home = std::getenv("HOME");
		dir = home ? home : "/";
	}
	NSURL* folder = [NSURL fileURLWithPath:[NSString stringWithUTF8String:dir.c_str()] isDirectory:YES];
	NSURL* terminal = [NSURL fileURLWithPath:@"/System/Applications/Utilities/Terminal.app"];
	[NSWorkspace.sharedWorkspace openURLs:@[folder]
	                 withApplicationAtURL:terminal
	                        configuration:[NSWorkspaceOpenConfiguration configuration]
	                    completionHandler:nil];
}
