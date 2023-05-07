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