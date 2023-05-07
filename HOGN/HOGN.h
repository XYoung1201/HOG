#pragma once
#include<iostream>
#include<stdio.h>
#include<string>
#include <algorithm>
#include<fstream>
#include<time.h>
#include<windows.h>
#include <atlstr.h>
#include <io.h>
#include <direct.h>
#include <chrono>
#include <filesystem>
#include <map>
using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )

typedef struct windowWithProcessAndHWND {
	DWORD processID;
	HWND m_hwnd;
}wPH;

typedef struct letter {
	string* a;
	int num;
	struct letter* letters[28];
	letter(struct letter* o) {
		if (o == NULL)
			o = this;
		for (struct letter*& i : letters)
			i = o;
		a = NULL;
		num = 0;
	}
}ltr;

string para_path;
string para_dir;
struct letter *o, *mark;
bool not_up[28] = { false };
int char_down;
high_resolution_clock::time_point lastTime;
bool trayIconVisible;
HHOOK myhook;

std::string config_text = "//配置系统托盘图标显示\n"
"JFAI SET:TRAYICONENABLE:ON\n"
"//JFAI SET:TRAYICONENABLE:OFF\n"
"\n"
"//内置命令\n"
"JFRLD CMD:RELOAD\n"
"JFQT CMD:QUIT\n"
"JFQR CMD:QRCODE\n"
"JFCFG CMD:CONFIG\n"
"JFCAD CMD:CLOSEALLDIRECTORIES\n"
"JFMTT CMD:MUTETOGGLE\n"
"JFSD CMD:SHUTDOWN\n"
"JFRB CMD:REBOOT\n"
"JFCLSD CMD:CANCELSHUTDOWNORREBOOT\n"
"JFCLRB CMD:CANCELSHUTDOWNORREBOOT\n"
"\n"
"//系统路径\n"
"JFETC PATH:C:\\WINDOWS\\SYSTEM32\\DRIVERS\\ETC\n"
"JFHOG PATH:C:\\ProgramData\\HOG\n"
"JFQD PATH:C:\\ProgramData\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\n"
"\n"
"//专用文件夹\n"
"JFCX PATH:C:\\Program Files\\\n"
"\n"
"//系统应用\n"
"JFTXT PATH:C:\\Program Files\\WindowsApps\\Microsoft.WindowsNotepad_11.2303.40.0_x64__8wekyb3d8bbwe\\Notepad\\Notepad.exe\n"
"JFTSK PATH:C:\\Windows\\system32\\taskmgr.exe\n"
"JFHSZ PATH:SHELL:RecycleBinFolder\n"
"JFMD PATH:C:\\Windows\\system32\\cmd.exe\n"
"\n"
"//应用程序\n"
"//JFTK PATH:C:\\Program Files (x86)\\滴答清单\\TickTick.exe\n"
"//JFNTN PATH:C:\\Users\\huang\\AppData\\Local\\Programs\\Notion\\Notion.exe\n"
"//JFPDF PATH:C:\\Program Files\\Adobe\\Acrobat DC\\Acrobat\\Acrobat.exe\n"
"//JFRM PATH:C:\\Program Files\\UninstallTool\\UninstallToolPortable.exe\n"
"//JFXMD PATH:C:\\Program Files\\Xmind\\Xmind.exe\n"
"//JFXID PATH:C:\\Program Files (x86)\\Internet Download Manager\\IDMan.exe\n"
"//JFPS PATH:C:\\Program Files\\Adobe\\Adobe Photoshop 2023\\Photoshop.exe\n"
"//*+ PATH:C:\\Program Files\\Everything\\Everything.exe\n"
"//JFCM PATH:C:\\Program Files (x86)\\Microsoft\\Edge\\Application\\msedge.exe\n"
"//JFWX PATH:C:\\Program Files (x86)\\Tencent\\WeChat\\WeChat.exe\n"
"\n"
"//链接地址\n"
"JFGML PATH:https://mail.google.com\n"
"JFWML PATH:https://mail.163.com\n"
"JFXGD PATH:https://nwpu.edu.cn\n"
"JFTEL PATH:https://web.telegram.org\n"
"JFGPT PATH:https://chat.openai.com/chat\n"
"\n"
"//文本输入\n"
"*JFYX TEXT:aaaa@gmail.com";
