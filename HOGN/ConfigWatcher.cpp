#include "ConfigWatcher.h"
#include "Globals.h"

namespace {

ULONGLONG GetConfigFileModTime(const std::string& path) {
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &data))
		return 0;
	ULARGE_INTEGER uli;
	uli.LowPart = data.ftLastWriteTime.dwLowDateTime;
	uli.HighPart = data.ftLastWriteTime.dwHighDateTime;
	return uli.QuadPart;
}

void WatchConfigFileProc() {
	std::string path = config_path;
	ULONGLONG last = GetConfigFileModTime(path);

	DWORD startTick = GetTickCount();
	const DWORD timeoutMs = 10 * 60 * 1000; // 10 minutes

	while ((GetTickCount() - startTick) < timeoutMs) {
		Sleep(800);
		ULONGLONG current = GetConfigFileModTime(path);
		if (current == 0) continue;
		if (current == last) continue;

		// Debounce: require two consecutive equal readings before declaring done.
		ULONGLONG pending = current;
		for (int i = 0; i < 8; i++) {
			Sleep(700);
			ULONGLONG after = GetConfigFileModTime(path);
			if (after == 0) continue;
			if (after == pending) {
				if (g_mainHwnd)
					SendMessageA(g_mainHwnd, WM_USER_RELOAD_CONFIG, 0, 0);
				g_watching_config = false;
				return;
			}
			pending = after;
		}
		last = pending;
	}
	g_watching_config = false;
}

} // anonymous namespace

void StartConfigWatcher() {
	bool expected = false;
	if (g_watching_config.compare_exchange_strong(expected, true))
		std::thread(WatchConfigFileProc).detach();
}
