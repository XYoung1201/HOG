#include "Globals.h"

// Definitions of the project's global state. Paths are filled in by main().

std::string para_path;
std::string config_path;
std::string para_dir;

struct letter* o = nullptr;
struct letter* mark = nullptr;
bool not_up[28] = { false };
int char_down = 0;
high_resolution_clock::time_point lastTime;
int g_lastTriggerLength = 0;

bool trayIconVisible = false;

bool listening = false;
bool action_listening = false;
std::atomic<bool> action_doing(false);
std::atomic<bool> action_paused(false);

std::vector<struct actionRecord> actions;
std::mutex actions_mutex;

std::atomic<bool> g_watching_config(false);

// Default config written on first install. Same format as the Windows conf,
// except `*` / `+` mean left / right Command (⌘) instead of Ctrl.
const std::string config_text =
"//Built-in configuration: show / hide the menu bar icon\n"
"JFAI SET:TRAYICONENABLE:ON\n"
"//JFAI SET:TRAYICONENABLE:OFF\n"
"\n"
"//Built-in commands\n"
"JFRLD CMD:RELOAD\n"
"JFQT CMD:QUIT\n"
"JFQR CMD:QRCODE\n"
"JFCFG CMD:CONFIG\n"
"JFCAD CMD:CLOSEALLDIRECTORIES\n"
"JFMTT CMD:MUTETOGGLE\n"
"JFSD CMD:SHUTDOWN\n"
"JFRB CMD:REBOOT\n"
"JFCLSD CMD:CANCELSHUTDOWNORREBOOT\n"
"JFSCH CMD:GOOGLESEARCH\n"
"JFOCMD CMD:OPENCMDINCURRENTPATH\n"
"JFCTW CMD:CLOSETHISWINDOW\n"
"JFPDF CMD:COPYPDFTEXT\n"
"JFREC CMD:RECORD\n"
"JFACT CMD:ACTION\n"
"JFOSWT CMD:ONOFFHOGTOGGLE\n"
"\n"
"//Folders\n"
"JFDL PATH:~/Downloads\n"
"JFDT PATH:~/Desktop\n"
"JFHOG PATH:~/Library/Application Support/HOG\n"
"\n"
"//Applications\n"
"JFTXT PATH:/System/Applications/TextEdit.app\n"
"JFTRM PATH:/System/Applications/Utilities/Terminal.app\n"
"JFAM PATH:/System/Applications/Utilities/Activity Monitor.app\n"
"\n"
"//Links\n"
"JFGH PATH:https://github.com\n"
"JFGML PATH:https://mail.google.com\n"
"\n"
"//Text input\n"
"*JFYX TEXT:aaaa@gmail.com\n";
