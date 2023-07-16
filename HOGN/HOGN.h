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

#include "qrcodegen.hpp"
#include <tchar.h>
#include <map>
#include <string>
#include <atlstr.h>
#include <iostream>
#include <Windows.h>
#include <MMDeviceAPI.h>
#include <EndpointVolume.h>
#include <atlbase.h>
#include <wchar.h>
#include <ShlObj.h>
#include <ShObjIdl.h>
#include <atlcom.h>
#include <UIAutomation.h>

#include <string>
#include <regex>
#include <cstdlib> // for std::getenv

#define WM_TRAYICON (WM_USER + 1)
#define WM_CONFIGMESSAGE (WM_USER + 2)
#define IDI_ICON 101
#define IDI_ICON_GRAY 108
#define ID_TRAY_EXIT 9001
#define ID_TRAY_RELOAD 9002
#define ID_TRAY_CONFIG 9003
#define ID_TRAY_PAUSE 9004
#define ID_TRAY_START 9005
#define ID_TRAY_QRCODE 9006
#define ID_TRAY_CLOSEDIRECTORIES 9007


using namespace std;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;
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
bool not_up[28];
int char_down;
high_resolution_clock::time_point lastTime;
bool trayIconVisible;
HHOOK myhook;
NOTIFYICONDATA nid;
bool listening;

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
"JFOCMD CMD:OPENCMDINCURRENTPATH\n"
"JFOSWT CMD:ONOFFHOGTOGGLE\n"
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

void readPara();
void runCommand(string set);
void toggleTrayIcon();