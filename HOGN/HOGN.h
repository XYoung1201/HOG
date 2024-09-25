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

#include <windows.h>
#include <commdlg.h>
#include <iostream>


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
#define ID_TRAY_CHANGECONF 9008


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

struct actionRecord{
	int action;//0->click,1->keyboard,2->move
	int value1;//click&keyboard:0->down,1->up;move:x_axis
	int value2;//click:0->left,1->right;keyboard:keyChar;move:y_axis
};

string para_path;
string config_path;
string para_dir;
struct letter *o, *mark;
bool not_up[28];
int char_down;
high_resolution_clock::time_point lastTime;
bool trayIconVisible;
HHOOK myhook;
HHOOK mouseHook;
NOTIFYICONDATA nid;
bool listening;
vector<struct actionRecord> actions;
bool action_listening;
bool action_doing;

std::string config_text = 
"JFAI SET:TRAYICONENABLE:ON\n"
"//JFAI SET:TRAYICONENABLE:OFF\n"
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
"*JFYX TEXT:aaaa@gmail.com";

void readPara();
void runCommand(string set);
void toggleTrayIcon();