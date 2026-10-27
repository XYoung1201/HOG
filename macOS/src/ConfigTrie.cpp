#include "ConfigTrie.h"
#include "Globals.h"

// Forward decl from Tray.h – avoids circular include.
void toggleTrayIconVisibility();

bool hasPrefix(const std::string& str, const std::string& prefix) {
	if (str.length() < prefix.length())
		return false;
	return str.compare(0, prefix.length(), prefix) == 0;
}

std::string extractContent(const std::string& str, const std::string& prefix) {
	if (hasPrefix(str, prefix))
		return str.substr(prefix.length());
	return "";
}

void loadSet(std::string set) {
	set = extractContent(set, "SET:");
	if (set.empty()) return;
	bool onoff = set.substr(set.rfind(':') + 1) == "ON";
	set = set.substr(0, set.find(':'));
	if (set == "TRAYICONENABLE") {
		if (trayIconVisible != onoff)
			toggleTrayIconVisibility();
		return;
	}
}

void handleSet(std::string set) {
	std::string instruct = set.substr(0, set.find(':'));
	if (instruct == "TRAYICONENABLE") {
		toggleTrayIconVisibility();
		return;
	}
}

void commandLoad(std::string word) {
	std::string first, second;
	first = word.substr(0, word.find(' '));
	second = word.substr(word.find(' ') + 1, word.length());

	for (char& c : first) {
		int grade;
		if (int(c) == 42)
			grade = 26;
		else if (int(c) == 43)
			grade = 27;
		else
			grade = int(c) >= 97 ? int(c) - 97 : int(c) - 65;
		if (grade < 0 || grade > 27)
			continue;
		if (mark->letters[grade] == o)
			mark->letters[grade] = new struct letter(o);
		mark = mark->letters[grade];
	}
	mark->a = new std::string;
	*mark->a = second;
	loadSet(second);
	first.erase(std::remove(first.begin(), first.end(), '*'), first.end());
	first.erase(std::remove(first.begin(), first.end(), '+'), first.end());
	mark->num = (int)first.length();
	mark = o;
}

void readPara() {
	for (bool& up : not_up)
		up = false;
	char_down = 0;
	o = NULL;
	o = new struct letter(o);
	mark = o;
	std::ifstream in(config_path);
	std::string line;
	do {
		getline(in, line);
		// Configs copied over from Windows carry CRLF line endings.
		if (!line.empty() && line.back() == '\r')
			line.pop_back();
		if ((line[0] == '/' && line[1] == '/') || line == "")
			continue;
		commandLoad(line);
	} while (!in.eof());
	in.close();
}

void clearMem(struct letter* node) {
	if (!node) return;
	for (int i = 0; i < 28; i++) {
		if (node->letters[i] != o)
			clearMem(node->letters[i]);
	}
	if (node->a != NULL)
		delete node->a;
	delete node;
}

void keyStateChange(int code, bool state) {
	if (code > 64 && code < 91)
		not_up[code - 65] = state;
	if (code == VK_LWIN)
		not_up[26] = state;
	if (code == VK_RWIN)
		not_up[27] = state;
}

bool getKeyState(int code) {
	if (code > 64 && code < 91)
		return not_up[code - 65];
	if (code == VK_LWIN)
		return not_up[26];
	if (code == VK_RWIN)
		return not_up[27];
	return false;
}

struct letter* getNextLetter(struct letter* lastCode, int code) {
	if (code > 64 && code < 91)
		return lastCode->letters[code - 65];
	if (code == VK_LWIN)
		return lastCode->letters[26];
	if (code == VK_RWIN)
		return lastCode->letters[27];
	if (code == 8)
		return lastCode;
	return o;
}
