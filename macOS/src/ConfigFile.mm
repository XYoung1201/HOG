#include "ConfigFile.h"
#include "Globals.h"
#import <Cocoa/Cocoa.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <sys/stat.h>
#include <unistd.h>

bool AskToUseDefaultPath() {
	[NSApp activateIgnoringOtherApps:YES];
	NSAlert* alert = [[NSAlert alloc] init];
	alert.messageText = @"save config";
	alert.informativeText = [NSString stringWithFormat:
		@"Use the default config file?\n\n%s\n\nChoose \"Use Existing…\" to pick a config file you already have.",
		config_path.c_str()];
	[alert addButtonWithTitle:@"Use Default"];
	[alert addButtonWithTitle:@"Use Existing…"];
	return [alert runModal] == NSAlertFirstButtonReturn;
}

std::string GetCustomSavePath() {
	[NSApp activateIgnoringOtherApps:YES];
	NSOpenPanel* panel = [NSOpenPanel openPanel];
	panel.title = @"Choose a HOG config file";
	panel.canChooseFiles = YES;
	panel.canChooseDirectories = NO;
	panel.allowsMultipleSelection = NO;
	if ([panel runModal] == NSModalResponseOK && panel.URL)
		return std::string(panel.URL.path.UTF8String);
	return "";
}

void openConfigInEditor() {
	NSURL* file = [NSURL fileURLWithPath:[NSString stringWithUTF8String:config_path.c_str()]];
	NSURL* editor = [NSWorkspace.sharedWorkspace URLForApplicationToOpenContentType:UTTypePlainText];
	if (editor)
		[NSWorkspace.sharedWorkspace openURLs:@[file] withApplicationAtURL:editor
		                        configuration:[NSWorkspaceOpenConfiguration configuration]
		                    completionHandler:nil];
}

void configFileSet() {
	if (!AskToUseDefaultPath()) {
		std::string customPath = GetCustomSavePath();
		if (!customPath.empty()) {
			config_path = customPath;
			openConfigInEditor();
			return;
		}
	}
	WriteStringToFile(config_path, config_text);
	openConfigInEditor();
}

std::string ReadFileToString(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

bool WriteStringToFile(const std::string& filePath, const std::string& content) {
	std::ofstream file(filePath);
	if (!file.is_open()) return false;
	file << content;
	return true;
}

void firstInitialJudge() {
	if (access(para_dir.c_str(), F_OK) == -1)
		if (mkdir(para_dir.c_str(), 0755) == -1)
			return;
	if (access(para_path.c_str(), F_OK) != -1) {
		std::string saved = ReadFileToString(para_path);
		while (!saved.empty() && (saved.back() == '\n' || saved.back() == '\r'))
			saved.pop_back();
		if (!saved.empty())
			config_path = saved;
	}
	if (access(config_path.c_str(), F_OK) == -1)
		configFileSet();
	WriteStringToFile(para_path, config_path);
}
