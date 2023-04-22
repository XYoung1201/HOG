#include "winRel.h"

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
		if (wParam == VK_ESCAPE) // ������µļ��� Esc ��
		{
			DestroyWindow(hWnd); // �رմ���
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

		// �޸�: �����ڴ� DC ��λͼ�Խ���˫�������
		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom); // ����: ֱ��ʹ�� rect.right �� rect.bottom
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
			_T("Arial")); // You can change the font name here

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

		// �޸�: �����ڴ� DC ��λͼ��Դ
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
	// ��ȡ��Ļ�ߴ�
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// ���㴰������Ļ�е�λ��
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
	// ��ȡ��������
	const int bufferSize = 256;
	char buffer[bufferSize];
	GetClassNameA(hwnd, buffer, bufferSize);

	// ����ҵ��Ĵ������ļ���Դ���������ڣ��ر���
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
	wchar_t className[maxLength];

	// ��ȡ��ǰ����ڵľ��
	HWND activeWindow = GetForegroundWindow();

	// ��ȡ��������
	GetClassName(activeWindow, className, maxLength);

	// �� wchar_t ת��Ϊ std::wstring �Խ��бȽ�
	std::wstring wClassName(className);

	// ��鴰�������Ƿ��� "CabinetWClass" �� "ExploreWClass"
	if (wClassName == L"CabinetWClass" || wClassName == L"ExploreWClass") {
		return true;
	}
	else {
		return false;
	}
}