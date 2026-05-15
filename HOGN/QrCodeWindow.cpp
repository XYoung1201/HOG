#include "QrCodeWindow.h"
#include "WinUtils.h"
#include "Clipboard.h"

static LRESULT CALLBACK QrWindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		break;
	case WM_PAINT: {
		TCHAR* displayText = (TCHAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rect;
		GetClientRect(hWnd, &rect);

		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

		int fontHeight = 20 / 2;
		HFONT hFont = CreateFont(
			fontHeight, 0, 0, 0,
			FW_NORMAL, FALSE, FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			_T("Aria"));
		HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

		int lineSpacing = -9;
		DrawTextWithLineSpacing(memDC, displayText, -1, &rect, DT_CENTER | DT_WORDBREAK | DT_NOCLIP, lineSpacing);

		SelectObject(hdc, hOldFont);
		DeleteObject(hFont);

		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, SRCCOPY);

		SelectObject(memDC, oldBitmap);
		DeleteObject(memBitmap);
		DeleteDC(memDC);
		EndPaint(hWnd, &ps);
	} break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

void ShowCustomWindow(const TCHAR* displayText) {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	const TCHAR* windowClassName = _T("QRCode of Clipboard Text");

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = QrWindowProcedure;
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
		CW_USEDEFAULT, CW_USEDEFAULT,
		300, 300,
		NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return;

	SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)displayText);

	HDC hdc = GetDC(hWnd);
	RECT textRect = { 0, 0, 3000, 0 };
	DrawText(hdc, displayText, -1, &textRect, DT_CENTER | DT_WORDBREAK | DT_CALCRECT);
	ReleaseDC(hWnd, hdc);

	RECT windowRect = { 0, 0, (LONG)((textRect.right - textRect.left) / 1.15), (LONG)((textRect.bottom - textRect.top) / 2.3) };
	AdjustWindowRectEx(&windowRect, GetWindowLong(hWnd, GWL_STYLE), FALSE, GetWindowLong(hWnd, GWL_EXSTYLE));
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;
	SetWindowPos(hWnd, NULL, windowX, windowY, windowWidth, windowHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void printClipboard() {
	const QrCode::Ecc errCorLvl = QrCode::Ecc::LOW;
	try {
		const QrCode qr = QrCode::encodeText(getClipboardText().c_str(), errCorLvl);
		std::string a;
		int border = 4;
		for (int y = -border; y < qr.getSize() + border; y++) {
			for (int x = -border; x < qr.getSize() + border; x++)
				a += (qr.getModule(x, y) ? "a" : "b");
			a += "\n";
		}
		ShowCustomWindow(CString(a.c_str()));
	}
	catch (const qrcodegen::data_too_long&) {
		ShowToastMessage("Text too long for QR code");
	}
	catch (...) {
	}
}
