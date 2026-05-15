#include "Globals.h"

// Definitions of the project's global state. Initialization of most of
// these happens in main(); a handful are initialized inline here because
// they need to be valid before main even starts (e.g. atomics).

std::string para_path;
std::string config_path;
std::string para_dir;

struct letter* o = nullptr;
struct letter* mark = nullptr;
bool not_up[28] = { false };
int char_down = 0;
high_resolution_clock::time_point lastTime;

HHOOK myhook = nullptr;
HHOOK mouseHook = nullptr;

NOTIFYICONDATA nid = {};
bool trayIconVisible = false;
HWND g_mainHwnd = nullptr;

bool listening = false;
bool action_listening = false;
std::atomic<bool> action_doing(false);
std::atomic<bool> action_paused(false);

std::vector<struct actionRecord> actions;
std::mutex actions_mutex;

std::atomic<bool> g_watching_config(false);

ULONG_PTR g_gdiplusToken = 0;

// Default config written on first install. The format mirrors the user's
// historical conf so existing installs keep working.
const std::string config_text =
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
