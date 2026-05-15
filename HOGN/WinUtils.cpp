#include "WinUtils.h"

double getzoom() {
	HWND hd = GetDesktopWindow();
	int zoom = GetDpiForWindow(hd);
	switch (zoom) {
	case 96:  return 1.0;
	case 120: return 1.25;
	case 144: return 1.5;
	case 192: return 2.0;
	default:  return 1.0;
	}
}

HMODULE GetCurrentModule() {
	HMODULE hModule = NULL;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)GetCurrentModule,
		&hModule);
	return hModule;
}

void DrawTextWithLineSpacing(HDC hdc, const TCHAR* text, int /*count*/, RECT* rect, UINT /*format*/, int lineSpacing) {
	HBRUSH hBackgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
	HBRUSH hForegroundBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH hNullBrush = CreateSolidBrush(RGB(255, 255, 255));

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	TCHAR* textCopy = _tcsdup(text);
	TCHAR* context = NULL;
	TCHAR* line = _tcstok_s(textCopy, _T("\n"), &context);
	int y = rect->top;

	while (line) {
		int lineLength = (int)_tcslen(line);
		RECT lineRect = { rect->left, y, rect->left + lineLength * tm.tmAveCharWidth, y + tm.tmHeight };

		for (int i = 0; i < lineLength; i++) {
			TCHAR c = line[i];
			if (c == 'a') {
				SelectObject(hdc, hBackgroundBrush);
				SetBkColor(hdc, RGB(0, 0, 0));
				SetTextColor(hdc, RGB(0, 0, 0));
			} else {
				SelectObject(hdc, hForegroundBrush);
				SetBkColor(hdc, RGB(255, 255, 255));
				SetTextColor(hdc, RGB(255, 255, 255));
			}
			RECT charRect = { lineRect.left + i * tm.tmAveCharWidth, lineRect.top, lineRect.left + (i + 1) * tm.tmAveCharWidth, lineRect.bottom };
			HBRUSH hOldBrush2 = (HBRUSH)SelectObject(hdc, hNullBrush);
			Rectangle(hdc, charRect.left, charRect.top, charRect.right, charRect.bottom);
			SelectObject(hdc, hOldBrush2);
			ExtTextOut(hdc, charRect.left, charRect.top, ETO_CLIPPED | ETO_OPAQUE, &charRect, &c, 1, NULL);
		}

		y += tm.tmHeight + lineSpacing;
		line = _tcstok_s(NULL, _T("\n"), &context);
	}

	free(textCopy);
	DeleteObject(hBackgroundBrush);
	DeleteObject(hForegroundBrush);
	DeleteObject(hNullBrush);
}

bool isActiveWindowExplorer() {
	const int maxLength = 256;
	char className[maxLength];
	HWND activeWindow = GetForegroundWindow();
	GetClassName(activeWindow, className, maxLength);
	std::string wClassName(className);
	return wClassName == "CabinetWClass" || wClassName == "ExploreWClass";
}
