#pragma once

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
#define WM_TRAYICON (WM_USER + 1)
#define IDI_ICON1 101
#define ID_TRAY_EXIT 9001
#define ID_TRAY_RELOAD 9002


using namespace std;
using std::uint8_t;
using qrcodegen::QrCode;
using qrcodegen::QrSegment;

void DrawTextWithLineSpacing(HDC hdc, const TCHAR* text, int count, RECT* rect, UINT format, int lineSpacing);
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ShowCustomWindow(const TCHAR* displayText);
HMODULE GetCurrentModule();
BOOL CALLBACK getRightHWND(HWND hwnd, LPARAM lParam);
std::map<UINT, HANDLE> saveClipboardData();
void restoreClipboardData(const std::map<UINT, HANDLE>& savedData);
void setClipboardText(const std::string& text);
std::string getClipboardText();
void simulatePaste();
void printClipboard();
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
void TouchBackspace(int letterNumber);
void closeAllDirectories();
bool isActiveWindowExplorer();
void toggleMute();
void cancelShutdown();
void shutdown();
void reboot();
void toggleTrayIconVisibility(bool& trayIconVisible);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void windowCreate();
void openTar(string target);