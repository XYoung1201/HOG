#include"HOGN.h"
#include"winRel.h"

void commandLoad(string word) {
	string first, second;
	first = word.substr(0, word.find(' '));
	second = word.substr(word.find(' ') + 1, word.length());

	for (char& c : first)
	{
		int grade;
		if (int(c) == 42)
			grade = 26;
		else if (int(c) == 43)
			grade = 27;
		else
			grade = int(c) >= 97 ? int(c) - 97 : int(c) - 65;
		if (mark->letters[grade] == o)
			mark->letters[grade] = new struct letter(o);
		mark = mark->letters[grade];
	}
	mark->a = new string;
	*mark->a = second;
	first.erase(std::remove(first.begin(), first.end(), '*'), first.end());
	first.erase(std::remove(first.begin(), first.end(), '+'), first.end());
	mark->num = first.length();
	mark = o;
}

void readPara() {
	char_down = 0;
	o = NULL;
	o = new struct letter(o);
	mark = o;
	ifstream in(para_path);
	string line;
	do {
		getline(in, line);
		if ((line[0] == '\/' && line[1] == '\/') || line == "")
			continue;
		commandLoad(line);
	} while (!in.eof());
	in.close();
	return;
}

void clearMem(struct letter* node) {
	for (int i = 0; i < 28; i++) {
		if (node->letters[i] != o)
			clearMem(node->letters[i]);
	}
	if (node->a != NULL)
		delete node->a;
	delete node;
}

void keyStateChange(int code,bool state) {
	if (code > 64 && code < 91)
		not_up[code - 65] = state;
	if (code == 162)
		not_up[26] = state;
	if (code == 163)
		not_up[27] = state;
}

bool getKeyState(int code) {
	if (code > 64 && code < 91)
		return not_up[code - 65];
	if (code == 162)
		return not_up[26];
	if (code == 163)
		return not_up[27];
}

struct letter* getNextLetter(struct letter* lastCode, int code) {
	if (code > 64 && code < 91) 
		return lastCode->letters[code - 65];
	if (code == 162) 
		return lastCode->letters[26];
	if (code == 163) 
		return lastCode->letters[27];
	if (code == 8)
		return lastCode;
	return o;
}

bool hasPrefix(const std::string& str, const std::string& prefix) {
	if (str.length() < prefix.length()) {
		return false;
	}
	return str.substr(0, prefix.length()) == prefix;
}

std::string extractContent(const std::string& str, const std::string& prefix) {
	if (hasPrefix(str, prefix)) {
		return str.substr(prefix.length());
	}
	return "";
}

void openTar(string target) {
	SHELLEXECUTEINFO myProcess = { 0 };
	myProcess.nShow = SW_SHOWMAXIMIZED;
	myProcess.fMask = SEE_MASK_NOCLOSEPROCESS;// | SEE_MASK_NOASYNC | SEE_MASK_WAITFORINPUTIDLE;
	myProcess.lpDirectory = CString("C:\\Users\\huang\\OneDrive");
	myProcess.lpFile = CString(target.c_str());
	myProcess.lpVerb = _T("open");
	myProcess.cbSize = sizeof(myProcess);
	ShellExecuteEx(&myProcess);
	EnumWindows(getRightHWND, (LPARAM)&myProcess);
	if (myProcess.hwnd != 0)
	{
		SetForegroundWindow(myProcess.hwnd);
		SetActiveWindow(myProcess.hwnd);
	}
	CloseHandle(myProcess.hProcess);
}

void runCommand(string command) {
	if (command == "QUIT") {
		int result = MessageBox(GetForegroundWindow(), CString("Do you want to quit HOGN?"), CString("Quit"), MB_OKCANCEL);
		if (result == 1) {
			UnhookWindowsHookEx(myhook);
			clearMem(o);
			exit(0);
		}
		return;
	}
	if (command == "CLOSEALLDIRECTORIES") {
		closeAllDirectories();
		return;
	}
	if (command == "RELOAD") {
		clearMem(o);
		readPara();
		MessageBox(GetForegroundWindow(), CString("Reload Successfully!"), CString("Success"), MB_OK);
		return;
	}
	if (command == "CONFIG") {
		openTar(para_path);
		return;
	}
	if (command == "QRCODE")
		printClipboard();
}

void writeText(string text) {
	std::map<UINT, HANDLE> savedClipboardData = saveClipboardData();
	setClipboardText(text);
	simulatePaste();
	Sleep(10);
	restoreClipboardData(savedClipboardData);
}

void excuteTar(string* tar) {
	string text = extractContent(*tar, "TEXT:");
	string cmd = extractContent(*tar, "SET:");
	try {
		if (text != "")
			writeText(text);
		else if (cmd != "")
			runCommand(cmd);
		else
			openTar(*tar);
	}
	catch (...) {
		MessageBox(GetForegroundWindow(), CString("Configure Wrong!"), CString("ERROR"), MB_OK);
	}
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	PKBDLLHOOKSTRUCT press = (PKBDLLHOOKSTRUCT)lParam;
	if (nCode >= 0) {
		high_resolution_clock::time_point thisTime = high_resolution_clock::now();
		int interval_ms = (std::chrono::duration_cast<milliseconds>(thisTime - lastTime)).count();

		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			if (getKeyState)
				lastTime = thisTime;
			keyStateChange(press->vkCode, false);
			if (mark->a != NULL)
			{
				if(!isActiveWindowExplorer())
					TouchBackspace(mark->num);
				if (!not_up[26] && !not_up[27])
					excuteTar(mark->a);
				mark = o;
			}
		}

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			if(!getKeyState(press->vkCode))
				lastTime = thisTime;
			keyStateChange(press->vkCode, true);
			mark = getNextLetter(mark, press->vkCode);
		}

		if (interval_ms > 1000)
			mark = getNextLetter(o,press->vkCode);
	}
	return CallNextHookEx(myhook, nCode, wParam, lParam);
}

void firstInitialJudge() {
	para_path = "C:\\ProgramData\\HOG\\HOG.conf";
	para_dir = "C:\\ProgramData\\HOG";
	if (_access(para_dir.c_str(), 0) == -1)
		_mkdir(para_dir.c_str());
	if (_access(para_path.c_str(), 0) == -1) {
		ofstream fout(para_path);
		fout << "//Built-in functions: Open configuration file, reload configuration file, exit, display QR code of clipboard content, close all folders\n//内置功能：打开配置文件，重载配置文件，退出，显示粘贴板内容二维码，关闭所有文件夹" << endl;
		fout << "JFCFG SET:CONFIG" << endl;
		fout << "JFRLD SET:RELOAD" << endl;
		fout << "JFQT SET:QUIT" << endl;
		fout << "JFQR SET:QRCODE" << endl;
		fout << "JFCLSAD SET:CLOSEALLDIRECTORIES" << endl;
		fout << "//Link address: Target format is https://xxx\n//链接地址：目标格式为https://xxx" << endl << endl;
		fout << "JFGH https://github.com" << endl;
		fout << "//Text shorthand: Target format is TEXT:xxx\n//文本速写：目标格式为TEXT:xxx" << endl << endl;
		fout << "JFMAIL mygithubmail@github.com" << endl;
		fout << "JFCLSAD SET:CLOSEALLDIRECTORIES" << endl;
		fout << "// System path\n//系统路径" << endl << endl;
		fout << "// Dedicated folder\n//专用文件夹" << endl << endl;
		fout << "// System application\n//系统应用" << endl << endl;
		fout << "// Application\n//应用程序" << endl << endl;
		fout << "// Logic script\n//逻辑脚本" << endl << endl;
		fout.close();
		ShellExecute(NULL, CString("open"), CString(para_path.c_str()), NULL, NULL, SW_SHOW);
	}
}

int main() {
	firstInitialJudge();
	readPara();
	lastTime = high_resolution_clock::now();
	myhook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetCurrentModule(), 0);
	MSG msg;
	while(GetMessageW(&msg, NULL, 0, 0) != -1);
}