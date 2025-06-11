# HOG (Hotkey Operated Gear)

HOG is a lightweight Windows utility written in C++. It listens for user defined
keyboard sequences and performs different actions such as launching programs or
pasting text. All hotkeys and actions are described in a plain text
configuration file so you can customise behaviour without recompiling.

The repository includes a Visual Studio solution (`HOGN.sln`) that builds the
`HOGN.exe` executable. The example configuration file `HOG.example.conf` shows
all available commands.

## How It Works

1. **Global Hooks** – The program installs global keyboard and mouse hooks using
   the Windows API. When a key sequence matches an entry in the configuration
   file, the associated action is triggered.
2. **Configuration Loading** – On start up, HOG creates a folder at
   `C:\ProgramData\HOG` and loads `HOG.conf`. You may edit this file at any time
   and reload it with a hotkey so changes apply immediately.
3. **Built‑in Actions** – Several commands are provided out of the box such as
   reloading the configuration, quitting the program, generating a QR code from
   the clipboard or toggling mute.
4. **Custom Actions** – Besides the built‑in commands, you can open files or
   folders, launch applications, run scripts, visit URLs or insert predefined
   text snippets.

Internally the application stays in a loop processing Windows messages while the
hooks monitor input. When a matching sequence is detected the relevant function
is executed in the background.

## Building

HOG targets Windows and requires a C++ compiler that supports the Win32 API.
The easiest way is to open the Visual Studio solution and build the `Release`
configuration:

```bash
# from the repository root
"C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" HOGN.sln /p:Configuration=Release
```

The output `HOGN.exe` will appear in the `Release` directory.

## Usage Guide

1. Run `HOGN.exe`. The application places an icon in the system tray and starts
   listening for key sequences.
2. Edit the configuration file located at `C:\ProgramData\HOG\HOG.conf`. Each
   line is a hotkey followed by the action to perform.
3. Hotkeys consist of letters **A–Z** optionally combined with the left Ctrl
   (`*`) or right Ctrl (`+`) modifiers. For example `JFQR` means press `J` then
   `F` then `Q` then `R`.
4. To reload the file without restarting, press the hotkey bound to
   `CMD:RELOAD`.

### Configuration Format

```
JFRLD CMD:RELOAD                   // reload configuration
JFQT  CMD:QUIT                     // exit the application
JFQR  CMD:QRCODE                   // show clipboard text as QR code
JFPF  C:\Program Files             // open a folder
JFNP  notepad.exe                  // launch Notepad
JFSM  TEXT:Sample Text             // paste predefined text
```

Lines beginning with `//` are comments. See `HOG.example.conf` for a full list
of built‑in commands and detailed examples.

## 中文说明

HOG 是一个 Windows 下的全局热键工具。程序启动后在后台监听键盘输入，
当按键序列与 `HOG.conf` 中的规则匹配时执行相应动作。配置文件位于
`C:\ProgramData\HOG\HOG.conf`，可随时编辑并通过热键重新加载。

配置行格式为 `按键序列 操作`，支持打开程序、文件夹、运行脚本或执行
内置命令等。更多示例请参考仓库中的 `HOG.example.conf`。
