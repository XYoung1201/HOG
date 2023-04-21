#include"HOGN.h"

void DrawTextWithLineSpacing(HDC hdc, const TCHAR* text, int count, RECT* rect, UINT format, int lineSpacing)
{
	// Create a new black brush to fill the background
	HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	// Create a new white brush to fill the foreground
	HBRUSH hForegroundBrush = CreateSolidBrush(RGB(255, 255, 255));
	// Create a new null brush to fill the character rectangle
	HBRUSH hNullBrush = CreateSolidBrush(RGB(255, 255, 255));

	// Calculate the text metrics
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	// Tokenize the input text by line breaks
	TCHAR* textCopy = _tcsdup(text);
	TCHAR* context = NULL;
	TCHAR* line = _tcstok_s(textCopy, _T("\n"), &context);
	int y = rect->top;

	// Draw each line with the specified line spacing
	while (line)
	{
		int lineLength = _tcslen(line);

		// Calculate the bounding rectangle for the current line
		RECT lineRect = { rect->left, y, rect->left + lineLength * tm.tmAveCharWidth, y + tm.tmHeight };

		// Draw each character in the line
		for (int i = 0; i < lineLength; i++)
		{
			TCHAR c = line[i];

			// If the character is 'a', fill the background with black and the foreground with white
			if (c == 'a')
			{
				// Select the black brush
				SelectObject(hdc, hBackgroundBrush);
				SetBkColor(hdc, RGB(0, 0, 0));
				SetTextColor(hdc, RGB(0, 0, 0));
			}
			// Otherwise, fill the background with white and the foreground with black
			else
			{
				// Select the white brush
				SelectObject(hdc, hForegroundBrush);
				SetBkColor(hdc, RGB(255, 255, 255));
				SetTextColor(hdc, RGB(255, 255, 255));
			}

			// Calculate the bounding rectangle for the current character
			RECT charRect = { lineRect.left + i * tm.tmAveCharWidth, lineRect.top, lineRect.left + (i + 1) * tm.tmAveCharWidth, lineRect.bottom };

			// Fill the character rectangle with the selected brush
			HBRUSH hOldBrush2 = (HBRUSH)SelectObject(hdc, hNullBrush);
			Rectangle(hdc, charRect.left, charRect.top, charRect.right, charRect.bottom);
			SelectObject(hdc, hOldBrush2);
			ExtTextOut(hdc, charRect.left, charRect.top, ETO_CLIPPED | ETO_OPAQUE, &charRect, &c, 1, NULL);
		}

		y += tm.tmHeight + lineSpacing;
		line = _tcstok_s(NULL, _T("\n"), &context);
	}

	free(textCopy);
	// Cleanup
	DeleteObject(hBackgroundBrush);
	DeleteObject(hForegroundBrush);
	DeleteObject(hNullBrush);
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
	{
		if (wParam == VK_ESCAPE) // 如果按下的键是 Esc 键
		{
			DestroyWindow(hWnd); // 关闭窗口
		}
	}
	break;
	case WM_PAINT:
	{
		TCHAR* displayText = (TCHAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);

		// 修改: 创建内存 DC 和位图以进行双缓冲绘制
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom); // 更正: 直接使用 rect.right 和 rect.bottom
		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);


		// Set the font size
		int fontSize = 8/2; // Set the desired font size in points
		int logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
		int fontHeight = 20/2;

		// Create a new font with the specified size
		HFONT hFont = CreateFont(
			fontHeight,
			0,
			0,
			0,
			FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			_T("Arial")); // You can change the font name here

		// Select the new font into the HDC
		HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

		// Set the inter-character spacing
		int characterSpacing = -3.6;
		SetTextCharacterExtra(hdc, characterSpacing);

		// Use a custom function to draw the text with custom line spacing
		int lineSpacing = -12;
		DrawTextWithLineSpacing(memDC, displayText, -1, &rect, DT_CENTER | DT_WORDBREAK | DT_NOCLIP, lineSpacing);

		// Restore the original font and delete the created font
		SelectObject(hdc, hOldFont);
		DeleteObject(hFont);

		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

		// 修改: 清理内存 DC 和位图资源
		SelectObject(memDC, oldBitmap);
		DeleteObject(memBitmap);
		DeleteDC(memDC);
		EndPaint(hWnd, &ps);
	}
	break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

void ShowCustomWindow(const TCHAR* displayText)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	const TCHAR* windowClassName = _T("QRCode of Clipboard Text");

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WindowProcedure;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = windowClassName;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	HWND hWnd = CreateWindowEx(
		WS_EX_DLGMODALFRAME,
		windowClassName,
		_T("QRCode of Clipboard Text"),
		WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		300,
		300,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (!hWnd)
	{
		MessageBox(NULL, _T("Window Creation Failed"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
		return;
	}

	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)displayText);

	// Calculate the text rectangle
	HDC hdc = GetDC(hWnd);
	RECT textRect = { 0, 0, 3000, 0 };
	DrawText(hdc, displayText, -1, &textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
	ReleaseDC(hWnd, hdc);

	// Adjust the window size based on the text rectangle
	RECT windowRect = { 0, 0,(textRect.right - textRect.left)/1.15, (textRect.bottom - textRect.top)/4};
	AdjustWindowRectEx(&windowRect, GetWindowLong(hWnd, GWL_STYLE), FALSE, GetWindowLong(hWnd, GWL_EXSTYLE));
	// 获取屏幕尺寸
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 计算窗口在屏幕中的位置
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;
	SetWindowPos(hWnd, NULL, windowX, windowY, windowWidth, windowHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


static void printQr(const QrCode& qr) {
	string a = "";
	int border = 4;
	for (int y = -border; y < qr.getSize() + border; y++) {
		for (int x = -border; x < qr.getSize() + border; x++) {
			a += (qr.getModule(x, y) ? "a" : "b");
		}
		a += "\n";
	}
	ShowCustomWindow(CString(a.c_str()));
}

HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);
	
	return hModule;
}

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

BOOL CALLBACK getRightHWND(HWND hwnd,LPARAM lParam)
{
	SHELLEXECUTEINFO* myProcess = (SHELLEXECUTEINFO *)lParam;
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd,&lpdwProcessId);
    if(lpdwProcessId==GetProcessId(myProcess->hProcess))
    {
        myProcess->hwnd=hwnd;
        return FALSE;
    }
    return TRUE;
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

std::map<UINT, HANDLE> saveClipboardData() {
	std::map<UINT, HANDLE> savedData;

	if (!OpenClipboard(NULL)) {
		return savedData;
	}

	UINT format = 0;
	while ((format = EnumClipboardFormats(format)) != 0) {
		HANDLE data = GetClipboardData(format);
		if (data) {
			SIZE_T size = GlobalSize(data);
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
			if (hGlobal) {
				void* src = GlobalLock(data);
				void* dest = GlobalLock(hGlobal);
				memcpy(dest, src, size);
				GlobalUnlock(data);
				GlobalUnlock(hGlobal);
				savedData[format] = hGlobal;
			}
		}
	}

	CloseClipboard();
	return savedData;
}

void restoreClipboardData(const std::map<UINT, HANDLE>& savedData) {
	if (!OpenClipboard(NULL)) {
		return;
	}

	EmptyClipboard();

	for (const auto& data : savedData) {
		SetClipboardData(data.first, data.second);
	}

	CloseClipboard();
}

void setClipboardText(const std::string& text) {
	if (!OpenClipboard(NULL)) {
		return;
	}

	EmptyClipboard();

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
	if (!hGlobal) {
		CloseClipboard();
		return;
	}

	char* dest = static_cast<char*>(GlobalLock(hGlobal));
	memcpy(dest, text.c_str(), text.size() + 1);
	GlobalUnlock(hGlobal);

	SetClipboardData(CF_TEXT, hGlobal);
	CloseClipboard();
}

std::string getClipboardText() {
	std::string result = "";

	if (!OpenClipboard(NULL)) {
		return result;
	}

	if (!IsClipboardFormatAvailable(CF_UNICODETEXT) && !IsClipboardFormatAvailable(CF_TEXT))
	{
		CloseClipboard();
		return result;
	}

	HGLOBAL hGlobal = GetClipboardData(CF_TEXT);
	if (hGlobal) {
		const char* data = static_cast<const char*>(GlobalLock(hGlobal));
		if (data) {
			result = data;
			GlobalUnlock(hGlobal);
		}
	}

	CloseClipboard();
	return result;
}

void simulatePaste() {
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void openTar(string* tar) {
	string text = extractContent(*tar, "TEXT:");
	if (text != "") {
		std::map<UINT, HANDLE> savedClipboardData = saveClipboardData();
		setClipboardText(text);
		simulatePaste();
		Sleep(10);
		restoreClipboardData(savedClipboardData);
		return;
	}
	SHELLEXECUTEINFO myProcess = { 0 };
	myProcess.nShow = SW_SHOWMAXIMIZED;
	myProcess.fMask = SEE_MASK_NOCLOSEPROCESS;// | SEE_MASK_NOASYNC | SEE_MASK_WAITFORINPUTIDLE;
	CString temp = tar->c_str();
	myProcess.lpDirectory =CString("C:\\Users\\huang\\OneDrive");
	HWND hWnd = NULL;
	OpenClipboard(hWnd);
	if (IsClipboardFormatAvailable(CF_TEXT)) {
		HANDLE h = GetClipboardData(CF_TEXT);
		char* p = (char*)GlobalLock(h);
		if (filesystem::exists(p)) {
			myProcess.lpDirectory = CString(p);
		}
	}
	CloseClipboard();
	myProcess.lpFile = temp;
	if (tar->find('.') != string::npos) 
		myProcess.lpVerb = _T("open");
	else 
		myProcess.lpVerb = _T("explore");
	myProcess.cbSize = sizeof(myProcess);
	try {
		ShellExecuteEx(&myProcess);
		EnumWindows(getRightHWND, (LPARAM)&myProcess);
		if (myProcess.hwnd != 0)
		{
			SetForegroundWindow(myProcess.hwnd);
			SetActiveWindow(myProcess.hwnd);
		}
		CloseHandle(myProcess.hProcess);
	}
	catch (const char* msg) {
		MessageBox(GetForegroundWindow(), CString(msg), CString("ERROR"), MB_OK);
	}
	catch (...) {

	}
}

void TouchBackspace(letter* ltr) {
	for (int i = 0; i < ltr->num; i++) {
		keybd_event(8, 0, 0, 0);
		Sleep(20);
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
				TouchBackspace(mark);
				if (*mark->a == "SET:QUIT") {
					int result = MessageBox(GetForegroundWindow(), CString("Do you want to quit HOGN?"), CString("Quit"), MB_OKCANCEL);
					if (result == 1) {
						UnhookWindowsHookEx(myhook);
						clearMem(o);
						exit(0);
					}
				}
				else if (*mark->a == "SET:RELOAD") { 
					clearMem(o);
					readPara();
					MessageBox(GetForegroundWindow(), CString("Reload Successfully!"), CString("Success"), MB_OK);
				}
				else if (*mark->a == "SET:CONFIG")
					openTar(&para_path);
				else if (*mark->a == "SET:QRCODE") {
					const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;
					try {
						const QrCode qr = QrCode::encodeText(getClipboardText().c_str(), errCorLvl);
						printQr(qr);
					}
					catch (const qrcodegen::data_too_long& e) {
						//std::cerr << "Error: Data too long - " << e.what() << std::endl;
						MessageBox(NULL, _T("TEXT TOO LONG!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
					}
					catch (...) {

					}
					mark = o;
				}
				else if (!not_up[26])
					openTar(mark->a);
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
		fout << "//系统级配置" << endl;
		fout << "JFRLD SET:RELOAD" << endl;
		fout << "JFQT SET:QUIT" << endl;
		fout << "JFCFG SET:CONFIG" << endl;
		fout << "JFQR SET:QRCODE" << endl;
		fout << "//系统路径" << endl << endl;
		fout << "//专用文件夹" << endl << endl;
		fout << "//系统应用" << endl << endl;
		fout << "//应用程序" << endl << endl;
		fout << "//逻辑脚本" << endl << endl;
		fout << "//链接地址" << endl << endl;
		fout << "//文本速写" << endl << endl;
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

