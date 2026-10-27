#include "Toast.h"
#include <condition_variable>

// ============================================================================
// Toast notification
// ============================================================================
// Non-blocking, layered, rounded-corner notification window. Lives on its own
// thread so it never stalls the main message loop or the global hooks.
//
// Calling ShowToastMessage from any thread updates the displayed text and
// restarts the fade-in / hold / fade-out animation.

namespace {

constexpr int TOAST_WIDTH         = 360;
constexpr int TOAST_HEIGHT        = 60;
constexpr int TOAST_CORNER        = 16;
constexpr int TOAST_BOTTOM_MARGIN = 140;
constexpr int TOAST_FADE_IN_MS    = 180;
constexpr int TOAST_HOLD_MS       = 1800;
constexpr int TOAST_FADE_OUT_MS   = 420;
constexpr BYTE TOAST_MAX_ALPHA    = 235;
constexpr UINT_PTR TOAST_TIMER_ID = 0x4001;
constexpr UINT TOAST_WM_TRIGGER   = WM_USER + 100;

HWND               g_toastHwnd = nullptr;
std::atomic<bool>  g_toastReady(false);
std::mutex         g_toastMutex;
std::condition_variable g_toastReadyCv;
std::wstring       g_toastMessage;
DWORD              g_toastStartTime = 0;

std::wstring AnsiToWide(const std::string& s) {
	if (s.empty()) return L"";
	int wlen = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);
	if (wlen <= 0) return L"";
	std::wstring w(wlen, L'\0');
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, &w[0], wlen);
	while (!w.empty() && w.back() == L'\0') w.pop_back();
	return w;
}

void RenderToast(HWND hwnd, BYTE alpha) {
	HDC hdcScreen = GetDC(NULL);
	HDC hdcMem = CreateCompatibleDC(hdcScreen);

	BITMAPINFO bmi = { 0 };
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = TOAST_WIDTH;
	bmi.bmiHeader.biHeight = -TOAST_HEIGHT;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;
	HBITMAP hBitmap = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

	if (pBits)
		memset(pBits, 0, TOAST_WIDTH * TOAST_HEIGHT * 4);

	std::wstring msg;
	{
		std::lock_guard<std::mutex> lock(g_toastMutex);
		msg = g_toastMessage;
	}

	{
		using namespace Gdiplus;
		Graphics graphics(hdcMem);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

		REAL radius = (REAL)TOAST_CORNER;
		REAL w = (REAL)TOAST_WIDTH;
		REAL h = (REAL)TOAST_HEIGHT;

		GraphicsPath path;
		path.AddArc(0.0f,             0.0f,             radius * 2, radius * 2, 180.0f, 90);
		path.AddArc(w - radius*2,  0.0f,             radius * 2, radius * 2, 270, 90);
		path.AddArc(w - radius*2,  h - radius*2,  radius * 2, radius * 2,   0.0f, 90);
		path.AddArc(0.0f,             h - radius*2,  radius * 2, radius * 2,  90, 90);
		path.CloseFigure();

		SolidBrush bgBrush(Color(alpha, 28, 28, 32));
		graphics.FillPath(&bgBrush, &path);

		Pen borderPen(Color((BYTE)(alpha * 0.4), 255, 255, 255), 1.0f);
		graphics.DrawPath(&borderPen, &path);

		FontFamily fontFamily(L"Segoe UI");
		Font font(&fontFamily, 17, FontStyleRegular, UnitPixel);
		SolidBrush textBrush(Color(alpha, 245, 245, 245));
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		format.SetLineAlignment(StringAlignmentCenter);
		format.SetFormatFlags(StringFormatFlagsNoWrap);
		format.SetTrimming(StringTrimmingEllipsisCharacter);

		RectF textRect(10, 0, w - 20, h);
		graphics.DrawString(msg.c_str(), -1, &font, textRect, &format, &textBrush);
	}

	// GDI+ outputs straight alpha; UpdateLayeredWindow + AC_SRC_ALPHA needs premultiplied.
	if (pBits) {
		DWORD* pixels = (DWORD*)pBits;
		int total = TOAST_WIDTH * TOAST_HEIGHT;
		for (int i = 0; i < total; i++) {
			DWORD c = pixels[i];
			BYTE a = (BYTE)((c >> 24) & 0xFF);
			if (a == 0) {
				pixels[i] = 0;
			} else if (a < 255) {
				BYTE r = (BYTE)((c >> 16) & 0xFF);
				BYTE g = (BYTE)((c >> 8) & 0xFF);
				BYTE b = (BYTE)(c & 0xFF);
				r = (BYTE)((r * a + 127) / 255);
				g = (BYTE)((g * a + 127) / 255);
				b = (BYTE)((b * a + 127) / 255);
				pixels[i] = ((DWORD)a << 24) | ((DWORD)r << 16) | ((DWORD)g << 8) | b;
			}
		}
	}

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	POINT ptDst;
	ptDst.x = (screenWidth - TOAST_WIDTH) / 2;
	ptDst.y = screenHeight - TOAST_HEIGHT - TOAST_BOTTOM_MARGIN;

	POINT ptSrc = { 0, 0 };
	SIZE size = { TOAST_WIDTH, TOAST_HEIGHT };
	BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

	SelectObject(hdcMem, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdcMem);
	ReleaseDC(NULL, hdcScreen);
}

LRESULT CALLBACK ToastWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case TOAST_WM_TRIGGER:
		g_toastStartTime = GetTickCount();
		RenderToast(hwnd, 0);
		ShowWindow(hwnd, SW_SHOWNOACTIVATE);
		KillTimer(hwnd, TOAST_TIMER_ID);
		SetTimer(hwnd, TOAST_TIMER_ID, 16, NULL);
		return 0;
	case WM_TIMER:
		if (wParam == TOAST_TIMER_ID) {
			DWORD elapsed = GetTickCount() - g_toastStartTime;
			BYTE alpha;
			if (elapsed < (DWORD)TOAST_FADE_IN_MS) {
				alpha = (BYTE)((elapsed * TOAST_MAX_ALPHA) / TOAST_FADE_IN_MS);
			} else if (elapsed < (DWORD)(TOAST_FADE_IN_MS + TOAST_HOLD_MS)) {
				alpha = TOAST_MAX_ALPHA;
			} else if (elapsed < (DWORD)(TOAST_FADE_IN_MS + TOAST_HOLD_MS + TOAST_FADE_OUT_MS)) {
				DWORD fadeElapsed = elapsed - TOAST_FADE_IN_MS - TOAST_HOLD_MS;
				alpha = (BYTE)(TOAST_MAX_ALPHA - (fadeElapsed * TOAST_MAX_ALPHA) / TOAST_FADE_OUT_MS);
			} else {
				KillTimer(hwnd, TOAST_TIMER_ID);
				ShowWindow(hwnd, SW_HIDE);
				return 0;
			}
			RenderToast(hwnd, alpha);
		}
		return 0;
	case WM_CLOSE:
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ToastThreadProc() {
	HINSTANCE hInstance = GetModuleHandle(NULL);
	const TCHAR* className = _T("HOGNToastClass");

	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = ToastWndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = NULL;
	wc.lpszClassName = className;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);

	g_toastHwnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT,
		className,
		_T("HOGN Toast"),
		WS_POPUP,
		0, 0, TOAST_WIDTH, TOAST_HEIGHT,
		NULL, NULL, hInstance, NULL);

	{
		std::lock_guard<std::mutex> lk(g_toastMutex);
		g_toastReady = true;
	}
	g_toastReadyCv.notify_all();

	if (!g_toastHwnd)
		return;

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

} // anonymous namespace

void ShowToastMessage(const std::string& message) {
	// Spin up the toast thread on first use; subsequent calls just post.
	static std::once_flag toastInitFlag;
	std::call_once(toastInitFlag, []() {
		std::thread(ToastThreadProc).detach();
		std::unique_lock<std::mutex> lk(g_toastMutex);
		g_toastReadyCv.wait_for(lk, std::chrono::seconds(1),
			[]() { return g_toastReady.load(); });
	});

	{
		std::lock_guard<std::mutex> lock(g_toastMutex);
		g_toastMessage = AnsiToWide(message);
	}

	HWND h = g_toastHwnd;
	if (h)
		PostMessage(h, TOAST_WM_TRIGGER, 0, 0);
}
