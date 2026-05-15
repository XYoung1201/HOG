#include "ConfigFile.h"
#include "Globals.h"

bool AskToUseDefaultPath() {
	int result = MessageBox(NULL,
		TEXT("使用默认配置文件？"),
		TEXT("保存配置"),
		MB_YESNO | MB_ICONQUESTION);
	return result == IDYES;
}

std::string GetCustomSavePath() {
	OPENFILENAME ofn;
	char szFile[260] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Config Files (*.conf)\0*.conf\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetSaveFileName(&ofn))
		return std::string(ofn.lpstrFile);
	return "";
}

void configFileSet() {
	if (AskToUseDefaultPath()) {
		std::ofstream fout(config_path);
		fout << config_text;
		fout.close();
		ShellExecute(NULL, CString("open"), CString(config_path.c_str()), NULL, NULL, SW_SHOW);
	} else {
		std::string customPath = GetCustomSavePath();
		if (!customPath.empty()) {
			config_path = customPath;
			ShellExecute(NULL, CString("open"), CString(config_path.c_str()), NULL, NULL, SW_SHOW);
		} else {
			std::ofstream fout(config_path);
			fout << config_text;
			fout.close();
			ShellExecute(NULL, CString("open"), CString(config_path.c_str()), NULL, NULL, SW_SHOW);
		}
	}
}

std::string ReadFileToString(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) return "";
	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

bool WriteStringToFile(const std::string& filePath, const std::string& content) {
	std::ofstream file(filePath);
	if (!file.is_open()) return false;
	file << content;
	return true;
}

void firstInitialJudge() {
	if (_access(para_dir.c_str(), 0) == -1)
		if (_mkdir(para_dir.c_str()) == -1)
			return;
	if (_access(para_path.c_str(), 0) != -1)
		config_path = ReadFileToString(para_path);
	if (_access(config_path.c_str(), 0) == -1)
		configFileSet();
	WriteStringToFile(para_path, config_path);
}
