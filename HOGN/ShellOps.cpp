#include "ShellOps.h"

std::string replaceEnvironmentVariables(const std::string& input) {
	std::string result = input;
	std::regex env_var_pattern(R"((%\w+%))");
	std::smatch match;
	while (std::regex_search(result, match, env_var_pattern)) {
		if (match.size() <= 1) break;
		std::string var = match[1].str();
		std::string var_name = var.substr(1, var.size() - 2);
		const char* var_value = std::getenv(var_name.c_str());
		if (var_value)
			result.replace(match.position(), var.length(), var_value);
		else
			result.replace(match.position(), var.length(), "");
	}
	return result;
}

BOOL CALLBACK getRightHWND(HWND hwnd, LPARAM lParam) {
	SHELLEXECUTEINFO* myProcess = (SHELLEXECUTEINFO*)lParam;
	DWORD lpdwProcessId;
	GetWindowThreadProcessId(hwnd, &lpdwProcessId);
	if (lpdwProcessId == GetProcessId(myProcess->hProcess)) {
		myProcess->hwnd = hwnd;
		return FALSE;
	}
	return TRUE;
}

void openTar(std::string target, bool asAdmin) {
	SHELLEXECUTEINFO myProcess = { 0 };
	myProcess.nShow = SW_SHOWMAXIMIZED;
	myProcess.fMask = SEE_MASK_NOCLOSEPROCESS;
	myProcess.lpDirectory = _T("C:\\");
	CString temp = replaceEnvironmentVariables(target).c_str();
	myProcess.lpFile = temp;
	myProcess.lpVerb = asAdmin ? _T("runas") : _T("open");
	myProcess.cbSize = sizeof(myProcess);

	ShellExecuteEx(&myProcess);
	EnumWindows(getRightHWND, (LPARAM)&myProcess);
	if (myProcess.hwnd != 0) {
		SetForegroundWindow(myProcess.hwnd);
		SetActiveWindow(myProcess.hwnd);
	}
	if (myProcess.hProcess)
		CloseHandle(myProcess.hProcess);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM /*lParam*/) {
	const int bufferSize = 256;
	char buffer[bufferSize];
	GetClassNameA(hwnd, buffer, bufferSize);
	if (strcmp(buffer, "CabinetWClass") == 0 || strcmp(buffer, "ExploreWClass") == 0)
		SendMessage(hwnd, WM_CLOSE, 0, 0);
	return TRUE;
}

void closeAllDirectories() {
	EnumWindows(EnumWindowsProc, 0);
}
