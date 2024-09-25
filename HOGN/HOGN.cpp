#include"HOGN.h"
#include "resource.h"
// 函数用于显示提示框
bool AskToUseDefaultPath() {
	int result = MessageBox(NULL,
		TEXT("使用默认配置文件？"),
		TEXT("保存配置"),
		MB_YESNO | MB_ICONQUESTION);

	return (result == IDYES); // 返回用户选择，Yes为true，No为false
}

// 函数用于打开文件选择对话框
std::string GetCustomSavePath() {
	OPENFILENAME ofn;       // 文件选择对话框结构
	char szFile[260] = { 0 }; // 存储选择的文件路径
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL; // 无父窗口
	ofn.lpstrFile = szFile; // 缓冲区存储文件名
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Config Files (*.conf)\0*.conf\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// 显示文件选择对话框
	if (GetSaveFileName(&ofn)) {
		return std::string(ofn.lpstrFile);
	}
	return ""; // 如果未选择文件返回空字符串
}

void configFileSet() {
	if (AskToUseDefaultPath()) {
		ofstream fout(config_path);
		fout << config_text;
		fout.close();
		ShellExecute(NULL, CString("open"), CString(config_path.c_str()), NULL, NULL, SW_SHOW);
	}
	else {
		std::string customPath = GetCustomSavePath();
		if (!customPath.empty()) {
			config_path = customPath;
			ShellExecute(NULL, CString("open"), CString(config_path.c_str()), NULL, NULL, SW_SHOW);
		}
		else {
			ofstream fout(config_path);
			fout << config_text;
			fout.close();
			ShellExecute(NULL, CString("open"), CString(config_path.c_str()), NULL, NULL, SW_SHOW);
		}
	}
}

// 函数1：读取文件内容
std::string ReadFileToString(const std::string& filePath) {
	std::ifstream file(filePath);  // 打开文件
	if (!file.is_open()) {
		return "";  // 打不开文件时返回空字符串
	}

	std::stringstream buffer;
	buffer << file.rdbuf();  // 读取文件内容到缓冲区
	return buffer.str();  // 返回文件内容的字符串
}

// 函数2：写入字符串到文件
bool WriteStringToFile(const std::string& filePath, const std::string& content) {
	std::ofstream file(filePath);  // 打开文件（自动覆盖原内容）
	if (!file.is_open()) {
		return false;  // 打不开文件时返回失败
	}

	file << content;  // 写入内容
	return true;  // 返回成功
}
// 记录鼠标点击
void recordClick(int button, int state) {
	actionRecord record;
	record.action = 0; // 鼠标点击
	record.value1 = state; // 0->down, 1->up
	record.value2 = button; // 0->left, 1->right
	actions.push_back(record);
}

void replayMouseClick(int button, int state) {
	INPUT input[1] = {};
	input[0].type = INPUT_MOUSE;
	if (button == 0) { // 左键
		input[0].mi.dwFlags = (state == 0) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
	}
	else if (button == 1) { // 右键
		input[0].mi.dwFlags = (state == 0) ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
	}
	SendInput(1, input, sizeof(INPUT));
}

// 记录键盘活动
void recordKeyboard(int keyChar, int state) {
	actionRecord record;
	record.action = 1; // 键盘
	record.value1 = state; // 0->down, 1->up
	record.value2 = keyChar; // 键码
	actions.push_back(record);
}

void replayKeyboardAction(int vkCode, int state) {
	INPUT input[1] = {};
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = vkCode;
	input[0].ki.dwFlags = (state == 0) ? 0 : KEYEVENTF_KEYUP;
	SendInput(1, input, sizeof(INPUT));
}

// 记录鼠标移动
void recordMove(int x, int y) {
	actionRecord record;
	record.action = 2; // 鼠标移动
	POINT initialPos; // 用于存储初始位置
	GetCursorPos(&initialPos);
	record.value1 = initialPos.x;
	record.value2 = initialPos.y;
	
	actions.push_back(record);
}

double getzoom() {
	HWND hd = GetDesktopWindow();
	int zoom = GetDpiForWindow(hd);
	double dpi = 0;
	switch (zoom) {
	case 96:
		dpi = 1;
		break;
	case 120:
		dpi = 1.25;
		break;
	case 144:
		dpi = 1.5;
		break;
	case 192:
		dpi = 2;
		break;
	default:
		break;
	}
	return dpi;
}

void replayMouseMove(int x, int y) {
	//INPUT input[1] = {};
	//input[0].type = INPUT_MOUSE;
	//input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	//int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	//int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//double dpi = getzoom();

	//input[0].mi.dx = round(double(x * 65535)/(screenWidth*dpi));
	//input[0].mi.dy = round(double(y * 65555)/(screenHeight*dpi));

	//SendInput(1, input, sizeof(INPUT));
	SetCursorPos(x,y);
}

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
		int fontSize = 8 / 2; // Set the desired font size in points
		int logPixelsY = GetDeviceCaps(hdc, LOGPIXELSY);
		int fontHeight = 20 / 2;

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
			_T("Aria")); // You can change the font name here

		// Select the new font into the HDC
		HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

		// Set the inter-character spacing
		//int characterSpacing = -5;
		//SetTextCharacterExtra(hdc, characterSpacing);

		// Use a custom function to draw the text with custom line spacing
		int lineSpacing = -9;
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
	RECT windowRect = { 0, 0,(textRect.right - textRect.left) / 1.15, (textRect.bottom - textRect.top) / 2.3 };
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

HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);

	return hModule;
}

BOOL CALLBACK getRightHWND(HWND hwnd, LPARAM lParam)
{
	SHELLEXECUTEINFO* myProcess = (SHELLEXECUTEINFO*)lParam;
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == GetProcessId(myProcess->hProcess))
	{
		myProcess->hwnd = hwnd;
		return FALSE;
	}
	return TRUE;
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
				if(dest != NULL && src != NULL)
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

void EditPDFText(std::string str) {
	std::string to_replace("。\r\n");
	std::string replacement(".");
	std::size_t pos;

	while ((pos = str.find(to_replace)) != std::string::npos) 
		str.replace(pos, to_replace.length(),replacement);
	to_replace = (".\r\n");
	while ((pos = str.find(to_replace)) != std::string::npos) 
		str.replace(pos, to_replace.length(),replacement);

	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '  '), str.end());

	while ((pos = str.find(replacement)) != std::string::npos) 
		str.replace(pos, replacement.length(),to_replace);
	setClipboardText(str);
}

void simulatePaste() {
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void simulatePush(char c) {
	// VkKeyScan返回的是虚拟按键码和需要的修饰符的组合
	SHORT vkCode = VkKeyScan(c);
	// 分离出虚拟按键码和修饰符
	BYTE vk = LOBYTE(vkCode);
	BYTE shiftState = HIBYTE(vkCode);

	INPUT input[4]; // 可能需要发送多达4个事件（Shift按下和释放，字符按下和释放）
	int nInputs = 0; // 输入事件的实际数量

	ZeroMemory(input, sizeof(input));

	// 如果需要按下Shift键
	if (shiftState & 1) {
		input[nInputs].type = INPUT_KEYBOARD;
		input[nInputs].ki.wVk = VK_SHIFT;
		nInputs++;
	}

	// 字符键按下
	input[nInputs].type = INPUT_KEYBOARD;
	input[nInputs].ki.wVk = vk;
	nInputs++;

	// 字符键释放
	input[nInputs].type = INPUT_KEYBOARD;
	input[nInputs].ki.wVk = vk;
	input[nInputs].ki.dwFlags = KEYEVENTF_KEYUP;
	nInputs++;

	// 如果按下了Shift键，现在释放
	if (shiftState & 1) {
		input[nInputs].type = INPUT_KEYBOARD;
		input[nInputs].ki.wVk = VK_SHIFT;
		input[nInputs].ki.dwFlags = KEYEVENTF_KEYUP;
		nInputs++;
	}

	// 发送按键事件
	SendInput(nInputs, input, sizeof(INPUT));
}

void simulateWrite(string str) {
	for (char i : str) {
		simulatePush(i);
		Sleep(1);
	}
}

void printClipboard() {
	const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;
	try {
		const QrCode qr = QrCode::encodeText(getClipboardText().c_str(), errCorLvl);
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
	catch (const qrcodegen::data_too_long& e) {
		//std::cerr << "Error: Data too long - " << e.what() << std::endl;
		MessageBox(NULL, _T("TEXT TOO LONG!"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
	}
	catch (...) {

	}
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	// 获取窗口类名
	const int bufferSize = 256;
	char buffer[bufferSize];
	GetClassNameA(hwnd, buffer, bufferSize);

	// 如果找到的窗口是文件资源管理器窗口，关闭它
	if (strcmp(buffer, "CabinetWClass") == 0 || strcmp(buffer, "ExploreWClass") == 0)
	{
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	}

	return TRUE;
}

void TouchBackspace(int letterNumber) {
	for (int i = 0; i < letterNumber; i++) {
		keybd_event(8, 0, 0, 0);
		Sleep(20);
	}
}

void closeAllDirectories() {
	EnumWindows(EnumWindowsProc, 0);
}

bool isActiveWindowExplorer() {
	const int maxLength = 256;
	char className[maxLength];

	// 获取当前活动窗口的句柄
	HWND activeWindow = GetForegroundWindow();

	// 获取窗口类名
	GetClassName(activeWindow, className, maxLength);

	// 将 char 转换为 std::string 以进行比较
	std::string wClassName(className);

	// 检查窗口类名是否是 "CabinetWClass" 或 "ExploreWClass"
	if (wClassName == "CabinetWClass" || wClassName == "ExploreWClass") {
		return true;
	}
	else {
		return false;
	}
}

CComPtr<IAudioEndpointVolume> getAudioEndpointVolume() {
	HRESULT hr;
	CComPtr<IMMDeviceEnumerator> deviceEnumerator;
	CComPtr<IMMDevice> defaultDevice;
	CComPtr<IAudioEndpointVolume> endpointVolume;

	hr = CoInitialize(NULL);
	if (FAILED(hr)) {
		std::cerr << "Error initializing COM library" << std::endl;
		return nullptr;
	}

	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL, CLSCTX_ALL,
		__uuidof(IMMDeviceEnumerator),
		(void**)&deviceEnumerator
	);
	if (FAILED(hr)) {
		std::cerr << "Error creating device enumerator" << std::endl;
		return nullptr;
	}

	hr = deviceEnumerator->GetDefaultAudioEndpoint(
		eRender, eConsole, &defaultDevice
	);
	if (FAILED(hr)) {
		std::cerr << "Error getting default audio endpoint" << std::endl;
		return nullptr;
	}

	hr = defaultDevice->Activate(
		__uuidof(IAudioEndpointVolume),
		CLSCTX_ALL, NULL, (void**)&endpointVolume
	);
	if (FAILED(hr)) {
		std::cerr << "Error activating audio endpoint volume" << std::endl;
		return nullptr;
	}

	return endpointVolume;
}

void mute() {
	CComPtr<IAudioEndpointVolume> endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) {
		return;
	}

	HRESULT hr = endpointVolume->SetMute(TRUE, NULL);
	if (FAILED(hr)) {
		std::cerr << "Error muting volume" << std::endl;
	}
}

void unmute() {
	CComPtr<IAudioEndpointVolume> endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) {
		return;
	}

	HRESULT hr = endpointVolume->SetMute(FALSE, NULL);
	if (FAILED(hr)) {
		std::cerr << "Error unmuting volume" << std::endl;
	}
}

void setVolume(float volume) {
	CComPtr<IAudioEndpointVolume> endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) {
		return;
	}

	if (volume < 0.0f) {
		volume = 0.0f;
	}
	else if (volume > 1.0f) {
		volume = 1.0f;
	}

	HRESULT hr = endpointVolume->SetMasterVolumeLevelScalar(volume, NULL);
	if (FAILED(hr)) {
		std::cerr << "Error setting volume leve" << std::endl;
	}
}

float getVolume() {
	CComPtr<IAudioEndpointVolume> endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) {
		return -1.0;
	}

	float currentVolume = 0.0f;
	HRESULT hr = endpointVolume->GetMasterVolumeLevelScalar(&currentVolume);
	if (FAILED(hr)) {
		std::cerr << "Error getting volume leve" << std::endl;
		return -1.0f;
	}

	return currentVolume;
}

void toggleMute() {
	CComPtr<IAudioEndpointVolume> endpointVolume = getAudioEndpointVolume();
	if (!endpointVolume) {
		return;
	}

	BOOL isMuted;
	HRESULT hr = endpointVolume->GetMute(&isMuted);
	if (FAILED(hr)) {
		std::cerr << "Error getting mute status" << std::endl;
		return;
	}

	if (isMuted) {
		unmute();
		MessageBox(GetForegroundWindow(), "Sound on!", CString("Prompt"), MB_OK);
	}
	else {
		mute();
		MessageBox(GetForegroundWindow(), "Sound off!", CString("Prompt"), MB_OK);
	}
}

void cancelShutdown() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		std::cerr << "OpenProcessToken failed" << std::endl;
		return;
	}

	// Get the LUID for the shutdown privilege
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS) {
		std::cerr << "AdjustTokenPrivileges failed" << std::endl;
		return;
	}

	// Cancel the scheduled shutdown
	if (!AbortSystemShutdown(NULL)) {
		std::cerr << "AbortSystemShutdown failed" << std::endl;
	}

	// Disable the shutdown privilege
	tkp.Privileges[0].Attributes = 0;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
}

void setShutdownPrivileges() {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		std::cerr << "OpenProcessToken failed" << std::endl;
		return;
	}

	// Get the LUID for the shutdown privilege
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS) {
		std::cerr << "AdjustTokenPrivileges failed" << std::endl;
		return;
	}
}

void shutdown() {
	setShutdownPrivileges();
	InitiateSystemShutdownEx(NULL, NULL, 20, TRUE, FALSE, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER);
}

void reboot() {
	setShutdownPrivileges();
	InitiateSystemShutdownEx(NULL, NULL, 20, TRUE, TRUE, SHTDN_REASON_MAJOR_OTHER | SHTDN_REASON_MINOR_OTHER);
}

std::string replaceEnvironmentVariables(const std::string& input) {
	std::string result = input;
	std::regex env_var_pattern(R"((%\w+%))");  // 正则表达式，匹配形如%VAR%的环境变量

	std::smatch match;
	while (std::regex_search(result, match, env_var_pattern)) {
		if (match.size() > 1) {
			std::string var = match[1].str();
			std::string var_name = var.substr(1, var.size() - 2);  // 移除两侧的'%'

			const char* var_value = std::getenv(var_name.c_str());
			if (var_value) {
				result.replace(match.position(), var.length(), var_value);  // 用环境变量的值替换
			}
			else {
				result.replace(match.position(), var.length(), "");  // 如果环境变量不存在，替换为空字符串
			}
		}
		else {
			break;
		}
	}

	return result;
}

void openTar(string target) {
	SHELLEXECUTEINFO myProcess = { 0 };
	myProcess.nShow = SW_SHOWMAXIMIZED;
	myProcess.fMask = SEE_MASK_NOCLOSEPROCESS;// | SEE_MASK_NOASYNC | SEE_MASK_WAITFORINPUTIDLE;
	myProcess.lpDirectory = _T("C:\\");
	CString temp = replaceEnvironmentVariables(target).c_str();
	myProcess.lpFile = temp;
	if (target == "CMD" || target == "POWERSHELL")
		myProcess.lpVerb = _T("runas");
	else
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

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	bool listeningState = listening;
	switch (msg) {
	case WM_CREATE: {
		ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hwnd;
		nid.uID = 14735;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = WM_TRAYICON;
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
		strcpy_s(nid.szTip, _countof(nid.szTip), "HOG"); // Use wcscpy_s to copy the wide string
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
		Shell_NotifyIcon(NIM_ADD, &nid);
	}
				  break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_TRAY_EXIT:
			listening = true;
			runCommand("QUIT");
			listening = listeningState;
			break;
		case ID_TRAY_RELOAD:
			listening = true;
			runCommand("RELOAD");
			listening = listeningState;
			break;
		case ID_TRAY_CONFIG:
			listening = true;
			runCommand("CONFIG");
			listening = listeningState;
			break;
		case ID_TRAY_PAUSE:
			if (listening != false)
				toggleTrayIcon();
			break;
		case ID_TRAY_START:
			if (listening != true)
				toggleTrayIcon();
			break;
		case ID_TRAY_QRCODE:
			listening = true;
			runCommand("QRCODE");
			listening = listeningState;
			break;
		case ID_TRAY_CLOSEDIRECTORIES:
			listening = true;
			runCommand("CLOSEALLDIRECTORIES");
			listening = listeningState;
			break;
		case ID_TRAY_CHANGECONF:
			runCommand("CHANGECONF");
			break;
		}
		break;
	case WM_TRAYICON:
		switch (lParam) {
		case WM_LBUTTONUP:
			openTar("C:\\ProgramData\\HOG\\HOG.conf");
			break;
		case WM_RBUTTONUP: {
			// Create a popup menu with multiple menu items
			HMENU hMenu = CreatePopupMenu();
			if(listening)
				AppendMenu(hMenu, MF_STRING, ID_TRAY_PAUSE, TEXT("Stop Listening"));
			else
				AppendMenu(hMenu, MF_STRING, ID_TRAY_START, TEXT("Start Listening"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_CONFIG, TEXT("Edit Config"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_RELOAD, TEXT("Reload Config"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_CHANGECONF, TEXT("Change Config"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_QRCODE, TEXT("Show Clipboard QRcode"));
			AppendMenu(hMenu, MF_STRING, ID_TRAY_CLOSEDIRECTORIES, TEXT("Close all Directories"));
			AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

			// Display the menu
			POINT pt;
			GetCursorPos(&pt);
			SetForegroundWindow(hwnd);

			// Add TPM_RETURNCMD to the TrackPopupMenu flags
			UINT selectedMenu = TrackPopupMenu(hMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hwnd, NULL);

			// If a menu item was selected, post the WM_COMMAND message with the selected command ID
			if (selectedMenu != 0) {
				SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(selectedMenu, 0), 0);
			}

			SendMessage(hwnd, WM_NULL, 0, 0);
			DestroyMenu(hMenu);
		}
						 break;
		}
		break;
	case WM_DESTROY:
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

void toggleTrayIconVisibility() {
	if (trayIconVisible) {
		Shell_NotifyIcon(NIM_DELETE, &nid);
	}
	else {
		Shell_NotifyIcon(NIM_ADD, &nid);
	}
	trayIconVisible = !trayIconVisible;
}

void toggleTrayIcon() {
	listening = !listening;
	if (listening) {
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
	else {
		nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_GRAY));
		Shell_NotifyIcon(NIM_MODIFY, &nid);
	}
}

void windowCreate() {
	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	// Register the window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CString("hiddenWindowClass");
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, CString("Window Registration Failed!"), CString("Error!"), MB_ICONEXCLAMATION | MB_OK);
	}

	// Create the hidden window
	hwnd = CreateWindowEx(0, wc.lpszClassName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);

	if (hwnd == NULL) {
		MessageBox(NULL, CString("Window Creation Failed!"), CString("Error!"), MB_ICONEXCLAMATION | MB_OK);
	}
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

void loadSet(string set) {
	set = extractContent(set, "SET:");
	if (set == "")
		return;
	bool onoff;
	set.substr(set.rfind(':') + 1) == "ON" ? onoff = true : onoff = false;
	set = set.substr(0, set.find(':'));
	if (set == "TRAYICONENABLE") {
		if (trayIconVisible != onoff)
			toggleTrayIconVisibility();
		return;
	}
}

void handleSet(string set) {
	string instruct = set.substr(0, set.find(':'));
	if (instruct == "TRAYICONENABLE") {
		toggleTrayIconVisibility();
		return;
	}
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
	loadSet(second);
	first.erase(std::remove(first.begin(), first.end(), '*'), first.end());
	first.erase(std::remove(first.begin(), first.end(), '+'), first.end());
	mark->num = first.length();
	mark = o;
}

void readPara() {
	for (bool& up : not_up)
		up = false;
	char_down = 0;
	o = NULL;
	o = new struct letter(o);
	mark = o;
	ifstream in(config_path);
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

int getClipboardNumber() {
	string clipboardText = getClipboardText();
	std::string::const_iterator it = clipboardText.begin();
	while (it != clipboardText.end() && std::isdigit(*it)) ++it;
	if (!clipboardText.empty() && it == clipboardText.end())
		return std::stoi(clipboardText); // 将字符串转换为整数
	else
		return 1;
}

void runCommand(string command) {
	if (listening) {
		if (command == "QUIT") {
			int result = MessageBox(GetForegroundWindow(), CString("Do you want to quit HOGN?"), CString("Quit"), MB_OKCANCEL);
			if (result == 1) {
				UnhookWindowsHookEx(myhook);
				UnhookWindowsHookEx(mouseHook);

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
		if (command == "CHANGECONF") {
			std::string customPath = GetCustomSavePath();
			if (!customPath.empty()) {
				config_path = customPath;
				runCommand("RELOAD");
				WriteStringToFile(para_path, config_path);
			}
			return;
		}
		if (command == "CONFIG") {
			openTar(config_path);
			return;
		}
		if (command == "MUTETOGGLE") {
			toggleMute();
			return;
		}
		if (command == "SHUTDOWN") {
			shutdown();
			return;
		}
		if (command == "REBOOT") {
			reboot();
			return;
		}
		if (command == "CANCELSHUTDOWNORREBOOT") {
			cancelShutdown();
			return;
		}
		if (command == "QRCODE") {
			printClipboard();
			return;
		}
		if (command == "OPENCMDINCURRENTPATH") {
			MessageBox(GetForegroundWindow(), "", CString("Success"), MB_OK);
			return;
		}
		if (command == "GOOGLESEARCH") {
			openTar(string("https://google.com/search?q=") + getClipboardText());
			return;
		}
		if (command == "CLOSETHISWINDOW") {
			HWND hwnd = GetForegroundWindow();
			if (hwnd != NULL)
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			return;
		}
		if (command == "COPYPDFTEXT") {
			EditPDFText(getClipboardText());
			return;
		}
		if (command == "RECORD") {
			action_listening = !action_listening;
			if (action_listening) {
				actions.clear();
			}
			else {
				actions.erase(actions.end() - mark->num * 3 + 1, actions.end());
				actions.erase(actions.begin(), actions.begin() + 1);
			}
			return;
		}
		if (command == "ACTION") {
			action_doing = true;
			int last_action_num = 2;
			int repeat_num = getClipboardNumber();
			while (repeat_num-- && action_doing) {
				for (const auto& record : actions) {
					switch (record.action) {
					case 0: // Mouse click
						if (last_action_num != 0)
							Sleep(300);
						else
							Sleep(100);
						replayMouseClick(record.value2, record.value1);
						last_action_num = 0;
						break;
					case 1: // Keyboard
						if (last_action_num != 1)
							Sleep(300);
						else
							Sleep(100);
						replayKeyboardAction(record.value2, record.value1);
						last_action_num = 1;
						break;
					case 2: // Mouse move
						Sleep(10);
						replayMouseMove(record.value1, record.value2);
						last_action_num = 2;
						break;
					}
				}
			}
			return;
		}
	}
	if (command == "ONOFFHOGTOGGLE") {
		listening = !listening;
		return;
	}

	MessageBox(GetForegroundWindow(), CString("WRONG BUILD-IN FUNCTION!"), CString("ERROR"), MB_OKCANCEL);
}

void writeText(string text) {
	//std::map<UINT, HANDLE> savedClipboardData = saveClipboardData();
	//setClipboardText(text);
	//simulatePaste();
	//Sleep(10);
	//restoreClipboardData(savedClipboardData);
	simulateWrite(text);
}

void excuteTar(string* tar) {
	string text = extractContent(*tar, "TEXT:");
	string cmd = extractContent(*tar, "CMD:");
	string set = extractContent(*tar, "SET:");
	string path = extractContent(*tar,"PATH:");
	try {
		if (text != "" && listening)
			writeText(text);
		else if (cmd != "")
			runCommand(cmd);
		else if (set != "" && listening)
			handleSet(set);
		else if (path != "" && listening)
			openTar(path);
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
		int state = 0;
		if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
			state = 0;
			if (getKeyState)
				lastTime = thisTime;
			keyStateChange(press->vkCode, false);
			if (mark->a != NULL)
			{
				if (!isActiveWindowExplorer() && listening) {
					TouchBackspace(mark->num);
				}
				if (!not_up[26] && !not_up[27] && !action_doing) {
					excuteTar(mark->a);
					action_doing = false;
				}
				mark = o;
			}
		}

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			if (press->vkCode == 0x51) {
				//如果按下的是Q键，退出循环回放模式
				action_doing = false;
			}
			state = 1;
			if(!getKeyState(press->vkCode))
				lastTime = thisTime;
			keyStateChange(press->vkCode, true);
			mark = getNextLetter(mark, press->vkCode);
		}

		if (action_listening)
			recordKeyboard(press->vkCode, state);

		if (interval_ms > 1000)
			mark = getNextLetter(o,press->vkCode);
	}
	return CallNextHookEx(myhook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		PMOUSEHOOKSTRUCT mouse = (PMOUSEHOOKSTRUCT)lParam;
		switch (wParam) {
		case WM_LBUTTONDOWN:
			if (action_listening)
				recordClick(0, 0); // 左键按下
			break;
		case WM_LBUTTONUP:
			if (action_listening)
				recordClick(0, 1); // 左键释放
			break;
		case WM_RBUTTONDOWN:
			if (action_listening)
				recordClick(1, 0); // 右键按下
			break;
		case WM_RBUTTONUP:
			if (action_listening)
				recordClick(1, 1); // 右键释放
			break;
		case WM_MOUSEMOVE:
			if (action_listening)
				recordMove(mouse->pt.x, mouse->pt.y); // 鼠标移动
			break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void firstInitialJudge() {
	if (_access(para_dir.c_str(), 0) == -1)
		if (_mkdir(para_dir.c_str()) == -1)
			return;
	if (_access(para_path.c_str(), 0) != -1)
		config_path = ReadFileToString(para_path);
	if (_access(config_path.c_str(), 0) == -1) {
		configFileSet();
	}
	WriteStringToFile(para_path, config_path);
}

int main() {
	HANDLE hMutex = CreateMutex(NULL, TRUE, "Global\\HOGNMutex");

	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		MessageBox(GetForegroundWindow(), _T("An instance of this application is already running!"), _T("Warning"), MB_OK);
		return -1;
	}
	trayIconVisible = true;
	listening = true;
	action_listening = false;
	action_doing = false;
	lastTime = high_resolution_clock::now();
	
	MSG msg;
	para_path = "C:\\ProgramData\\HOG\\config";
	config_path = "C:\\ProgramData\\HOG\\HOG.conf";
	para_dir = "C:\\ProgramData\\HOG";
	firstInitialJudge();
	windowCreate();
	readPara();
	myhook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetCurrentModule(), 0);
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetCurrentModule(), 0);
	while (GetMessageW(&msg, NULL, 0, 0) != -1);
}