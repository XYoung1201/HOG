#include "Clipboard.h"

std::map<UINT, HANDLE> saveClipboardData() {
	std::map<UINT, HANDLE> savedData;
	if (!OpenClipboard(NULL))
		return savedData;

	UINT format = 0;
	while ((format = EnumClipboardFormats(format)) != 0) {
		HANDLE data = GetClipboardData(format);
		if (!data) continue;
		SIZE_T size = GlobalSize(data);
		HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, size);
		if (!hGlobal) continue;
		void* src = GlobalLock(data);
		void* dest = GlobalLock(hGlobal);
		if (dest && src)
			memcpy(dest, src, size);
		GlobalUnlock(data);
		GlobalUnlock(hGlobal);
		savedData[format] = hGlobal;
	}
	CloseClipboard();
	return savedData;
}

void restoreClipboardData(const std::map<UINT, HANDLE>& savedData) {
	if (!OpenClipboard(NULL))
		return;
	EmptyClipboard();
	for (const auto& data : savedData)
		SetClipboardData(data.first, data.second);
	CloseClipboard();
}

void setClipboardText(const std::string& text) {
	if (!OpenClipboard(NULL))
		return;
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
	std::string result;
	if (!OpenClipboard(NULL))
		return result;
	if (!IsClipboardFormatAvailable(CF_UNICODETEXT) && !IsClipboardFormatAvailable(CF_TEXT)) {
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

int getClipboardNumber() {
	std::string clipboardText = getClipboardText();
	auto it = clipboardText.begin();
	while (it != clipboardText.end() && std::isdigit(static_cast<unsigned char>(*it))) ++it;
	if (!clipboardText.empty() && it == clipboardText.end())
		return std::stoi(clipboardText);
	return 1;
}

void EditPDFText(std::string str) {
	std::string to_replace("。\r\n");
	std::string replacement(".");
	std::size_t pos;

	while ((pos = str.find(to_replace)) != std::string::npos)
		str.replace(pos, to_replace.length(), replacement);
	to_replace = (".\r\n");
	while ((pos = str.find(to_replace)) != std::string::npos)
		str.replace(pos, to_replace.length(), replacement);

	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '  '), str.end());

	while ((pos = str.find(replacement)) != std::string::npos)
		str.replace(pos, replacement.length(), to_replace);
	setClipboardText(str);
}

void simulatePaste() {
	keybd_event(VK_CONTROL, 0, 0, 0);
	keybd_event('V', 0, 0, 0);
	keybd_event('V', 0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
}

void simulatePush(char c) {
	// VkKeyScan returns the vk code in the low byte and the required shift
	// state in the high byte. Build up to four INPUTs in one SendInput call:
	//   [shift down], char down, char up, [shift up]
	SHORT vkCode = VkKeyScan(c);
	BYTE vk = LOBYTE(vkCode);
	BYTE shiftState = HIBYTE(vkCode);

	INPUT input[4];
	int nInputs = 0;
	ZeroMemory(input, sizeof(input));

	if (shiftState & 1) {
		input[nInputs].type = INPUT_KEYBOARD;
		input[nInputs].ki.wVk = VK_SHIFT;
		nInputs++;
	}
	input[nInputs].type = INPUT_KEYBOARD;
	input[nInputs].ki.wVk = vk;
	nInputs++;
	input[nInputs].type = INPUT_KEYBOARD;
	input[nInputs].ki.wVk = vk;
	input[nInputs].ki.dwFlags = KEYEVENTF_KEYUP;
	nInputs++;
	if (shiftState & 1) {
		input[nInputs].type = INPUT_KEYBOARD;
		input[nInputs].ki.wVk = VK_SHIFT;
		input[nInputs].ki.dwFlags = KEYEVENTF_KEYUP;
		nInputs++;
	}
	SendInput(nInputs, input, sizeof(INPUT));
}

void simulateWrite(std::string str) {
	for (char c : str) {
		simulatePush(c);
		Sleep(1);
	}
}

void TouchBackspace(int letterNumber) {
	for (int i = 0; i < letterNumber; i++) {
		keybd_event(8, 0, 0, 0);
		Sleep(20);
	}
}
