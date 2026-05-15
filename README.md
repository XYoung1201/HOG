# HOG (Hotkey Operated Gear)

[![Star History Chart](https://api.star-history.com/svg?repos=XYoung1201/HOG&type=Timeline)](https://star-history.com/#XYoung1201/HOG&Timeline)

> 中文版说明在英文版之后。Chinese version follows the English one.

HOG is a lightweight Win32 background utility written in C++. It listens to every keystroke through a low-level keyboard hook and, whenever the recently typed letters match a prefix you defined in a config file, runs an action: type a snippet of text, open a file or URL, launch a program, control system audio, capture/replay a mouse-and-keyboard macro, and more.

It lives in the system tray, never opens a window of its own, and gives feedback through a fading toast notification so it never breaks your flow.

---

## Highlights

- **Custom letter-sequence hotkeys.** Hotkeys are short letter sequences (e.g. `JFRLD`, `JFCFG`) rather than chord shortcuts, so they don't clash with system or app shortcuts. Optional `*` / `+` modifiers add left/right `Ctrl`.
- **Many action types** — type text, run a built-in command, open a file/URL/folder, open a path as admin, or apply a setting.
- **Macro recording & playback** with a dedicated worker thread, so multi-round repeats stay responsive. Pause / resume / stop hotkeys (`P` / `Q`) work mid-playback. Replay also accepts a repeat count from the clipboard.
- **Non-blocking toast notifications** — every triggered action surfaces a fading, rounded toast near the bottom of the screen instead of a blocking `MessageBox`.
- **Live config reload** — after you open the config in your editor, HOG watches the file and automatically reloads when you save.
- **Clipboard utilities** — generate a QR code from clipboard text, reflow PDF-copied text, paste-and-replace.
- **System control** — toggle mute, set volume, scheduled shutdown / reboot / cancel, close all File-Explorer windows, etc.
- **Tray menu** — pause/resume listening, edit/reload/change config, show QR, close directories, quit.
- **Runs as administrator** — the project is configured to request elevation on launch so it can trigger admin-only actions.

---

## Quick start

1. Clone the repo.
2. Open `HOGN/HOGN.vcxproj` in **Visual Studio 2022** (toolset v143 is configured). The `x64` build is the recommended target; both `Win32` and `x64` builds are set up.
3. Build `Release | x64`.
4. Run `HOGN.exe`. The HOG icon appears in the system tray; HOG is now listening.
5. On first launch HOG will create `C:\ProgramData\HOG\` and ask whether to use the default config path or pick a custom one. After that it opens the config in your default editor — edit it, save, and the new bindings take effect automatically.

---

## Configuration file

The config file lives at `C:\ProgramData\HOG\HOG.conf` by default. You can move it via the tray's **Change Config** menu or by binding `CMD:CHANGECONF`. The location HOG actually uses is recorded in `C:\ProgramData\HOG\config`.

### Line format

```
<HOTKEY> <ACTION>
```

- `HOTKEY` is one or more uppercase letters `A`–`Z`, optionally prefixed with `*` (left `Ctrl`) and/or `+` (right `Ctrl`).
- `ACTION` is one of the action prefixes below.
- Lines beginning with `//` are comments and are skipped. Blank lines are skipped.

Examples:

```text
JFRLD CMD:RELOAD                            // sequence J F R L D triggers a reload
*JFYX TEXT:aaaa@gmail.com                   // hold LCtrl, then type J F Y X to paste an email
*+    PATH:C:\Program Files\Everything\Everything.exe   // LCtrl + RCtrl by themselves
```

### Action prefixes

| Prefix    | Effect                                                          |
|-----------|-----------------------------------------------------------------|
| `CMD:`    | Run a built-in command (see table below).                        |
| `TEXT:`   | Type the literal text via simulated keystrokes.                  |
| `PATH:`   | Open the target with the shell (`ShellExecute`) — files, folders, URLs, even `SHELL:` aliases (e.g. `SHELL:RecycleBinFolder`). Environment variables like `%USERPROFILE%` are expanded. |
| `PATH_A:` | Same as `PATH:` but launches with the `runas` verb (elevation prompt). |
| `SET:`    | Apply a runtime setting. Currently supported: `SET:TRAYICONENABLE:ON`/`:OFF`. |

### Built-in commands (`CMD:`)

| Command                  | What it does                                                                                  |
|--------------------------|-----------------------------------------------------------------------------------------------|
| `RELOAD`                 | Reload the config file.                                                                       |
| `CONFIG`                 | Open the config in your default editor; HOG auto-reloads when you save.                       |
| `CHANGECONF`             | Pick a new config file path and reload.                                                       |
| `QUIT`                   | Ask for confirmation then exit HOG.                                                           |
| `ONOFFHOGTOGGLE`         | Pause/resume the keyboard listener (works even while paused).                                 |
| `QRCODE`                 | Show a popup containing a QR code of the current clipboard text.                              |
| `MUTETOGGLE`             | Toggle system mute.                                                                           |
| `SHUTDOWN`               | Schedule a shutdown in 20 s.                                                                  |
| `REBOOT`                 | Schedule a reboot in 20 s.                                                                    |
| `CANCELSHUTDOWNORREBOOT` | Cancel a pending shutdown/reboot.                                                             |
| `CLOSEALLDIRECTORIES`    | Close every open File-Explorer window.                                                        |
| `CLOSETHISWINDOW`        | Send `WM_CLOSE` to the current foreground window.                                             |
| `GOOGLESEARCH`           | Open `https://google.com/search?q=<clipboard>` in your default browser.                       |
| `COPYPDFTEXT`            | Reflow text on the clipboard that was copied from a PDF (joins broken lines, collapses spaces) and put it back on the clipboard. |
| `OPENCMDINCURRENTPATH`   | Reserved (stub).                                                                              |
| `RECORD`                 | Start/stop recording a keyboard + mouse macro.                                                |
| `ACTION`                 | Replay the recorded macro. The clipboard is read as the repeat count (1 if it isn't a number). |
| `ACTIONSTOP`             | Stop the currently playing macro.                                                             |
| `ACTIONPAUSE`            | Pause the currently playing macro.                                                            |
| `ACTIONRESUME`           | Resume a paused macro.                                                                        |
| `ACTIONPAUSETOGGLE`      | Toggle pause/resume on the currently playing macro.                                           |

### Settings (`SET:`)

| Setting             | Values        | Effect                                              |
|---------------------|---------------|-----------------------------------------------------|
| `TRAYICONENABLE`    | `ON` / `OFF`  | Show or hide the tray icon. `ON` is the default.    |

---

## Macros — recording and playback

1. Bind two hotkeys in your config:
   ```text
   JFMR CMD:RECORD     // press once to start recording, press again to stop
   JFMP CMD:ACTION     // replay the recorded macro
   ```
2. Trigger `JFMR` to start recording — a toast confirms. Move the mouse and type as normal; HOG captures every key and click.
3. Trigger `JFMR` again to stop. Consecutive mouse-move events are coalesced into one record so playback isn't bogged down by intermediate cursor samples.
4. Optionally put a number on your clipboard (e.g. `5`) — `ACTION` reads it as the repeat count.
5. Trigger `JFMP`. Playback runs in a background thread.

While a macro is playing you can press these **hardware keys** (they only react during playback, and they ignore HOG's own synthesized input):

| Key | Effect                |
|-----|-----------------------|
| `Q` | Stop the playback.    |
| `P` | Pause / resume.       |

You can also bind any of `CMD:ACTIONSTOP`, `CMD:ACTIONPAUSE`, `CMD:ACTIONRESUME`, `CMD:ACTIONPAUSETOGGLE` to your own hotkeys (e.g. through the tray menu — see below).

### Conflict rules

- Recording is blocked while a macro is playing.
- Playback is blocked while a macro is recording.
- Events synthesized by HOG itself (`SendInput`) are filtered from the recording stream so replaying a macro while recording can't create a feedback loop.

---

## Tray icon

- **Left-click**: open the default config file.
- **Right-click**: a popup menu with:
  - **Stop / Start Listening** — pause or resume the global hook.
  - **Edit Config** — open the active config file (and start the auto-reload watcher).
  - **Reload Config** — re-parse the config without opening an editor.
  - **Change Config** — pick a different config file path.
  - **Show Clipboard QRcode** — same as `CMD:QRCODE`.
  - **Close all Directories** — same as `CMD:CLOSEALLDIRECTORIES`.
  - **Exit** — confirm and quit.

---

## Toast notifications

Every triggered action shows a small rounded, semi-transparent toast at the bottom-centre of the screen. It fades in (~180 ms), holds (~1.8 s), and fades out (~420 ms). Rapid actions reuse the same window and reset the animation — newest message replaces previous.

Examples of messages you'll see:

- `Reloaded config`
- `Config opened`
- `Macro recording started` / `Macro recording stopped`
- `Macro playback started` / `paused` / `resumed` / `stopped` / `finished`
- `Triggered: text input`
- `Opening: C:\Program Files`
- `Opening (admin): C:\Path\To\App.exe`

---

## Example config

A complete example with comments lives in [`HOG.example.conf`](HOG.example.conf). A condensed version:

```text
// ----- Tray + built-in toggles -----
JFAI   SET:TRAYICONENABLE:ON
JFHSWT CMD:ONOFFHOGTOGGLE

// ----- Built-in commands -----
JFRLD CMD:RELOAD
JFQT  CMD:QUIT
JFQR  CMD:QRCODE
JFCFG CMD:CONFIG
JFCAD CMD:CLOSEALLDIRECTORIES
JFMTT CMD:MUTETOGGLE
JFSD  CMD:SHUTDOWN
JFRB  CMD:REBOOT
JFCLSD CMD:CANCELSHUTDOWNORREBOOT
JFSCH CMD:GOOGLESEARCH

// ----- Macros -----
JFMR  CMD:RECORD
JFMP  CMD:ACTION
JFMS  CMD:ACTIONSTOP
JFMPP CMD:ACTIONPAUSETOGGLE

// ----- Paths / launchers -----
JFETC PATH:C:\Windows\System32\drivers\etc
JFHSZ PATH:SHELL:RecycleBinFolder
JFMD  PATH:C:\Windows\System32\cmd.exe
JFTSK PATH:C:\Windows\System32\taskmgr.exe
JFCM  PATH:C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe

// ----- Run elevated -----
JFXTOOL PATH_A:C:\Tools\admin-only.exe

// ----- URLs -----
JFGML PATH:https://mail.google.com
JFGPT PATH:https://chat.openai.com/chat

// ----- Text snippets (LCtrl + JFYX) -----
*JFYX TEXT:aaaa@gmail.com
```

---

## Building from source

- **Toolchain**: Visual Studio 2022 (v143). x64 builds use `/std:c++20`; the Win32 builds compile with the toolset's default standard and rely only on features that are available there.
- **Dependencies**: only the Windows SDK and GDI+ (linked via `#pragma comment(lib, "gdiplus.lib")`). No vcpkg/Conan/Nuget packages.
- **Output**: `HOGN.exe`. The project's manifest requests administrator elevation; you'll see the UAC prompt on first launch.

---

## Project layout

The code is split across small, focused translation units under [`HOGN/`](HOGN/):

| File                                                    | Responsibility                                                                 |
|---------------------------------------------------------|--------------------------------------------------------------------------------|
| [`HOGN.cpp`](HOGN/HOGN.cpp)                             | `main()` — installs hooks, runs the message pump.                              |
| [`HOGN.h`](HOGN/HOGN.h)                                 | Project-wide includes, types (`letter`, `actionRecord`), message ids.          |
| [`Globals.{h,cpp}`](HOGN/Globals.h)                     | Global state (`extern` decls + single definitions).                            |
| [`Hooks.cpp`](HOGN/Hooks.cpp)                           | Low-level keyboard / mouse hook callbacks.                                     |
| [`Macro.cpp`](HOGN/Macro.cpp)                           | Record + threaded playback.                                                    |
| [`ConfigFile.cpp`](HOGN/ConfigFile.cpp)                 | First-run setup, file I/O, save-dialog.                                        |
| [`ConfigTrie.cpp`](HOGN/ConfigTrie.cpp)                 | Build and walk the hotkey prefix trie.                                         |
| [`ConfigWatcher.cpp`](HOGN/ConfigWatcher.cpp)           | Post-edit auto-reload watcher.                                                 |
| [`Command.cpp`](HOGN/Command.cpp)                       | `runCommand` (O(1) command dispatch) and `excuteTar`.                          |
| [`Toast.cpp`](HOGN/Toast.cpp)                           | Layered, rounded toast notification.                                           |
| [`Tray.cpp`](HOGN/Tray.cpp)                             | Hidden main window, tray icon, popup menu.                                     |
| [`QrCodeWindow.cpp`](HOGN/QrCodeWindow.cpp)             | Clipboard-text → QR popup.                                                     |
| [`WinUtils.cpp`](HOGN/WinUtils.cpp)                     | DPI, foreground-class check, text drawing helpers.                             |
| [`Clipboard.cpp`](HOGN/Clipboard.cpp)                   | Clipboard ops + keystroke simulation.                                          |
| [`SoundControl.cpp`](HOGN/SoundControl.cpp)             | Volume / mute (`IAudioEndpointVolume`).                                        |
| [`SystemControl.cpp`](HOGN/SystemControl.cpp)           | Shutdown / reboot / privileges.                                                |
| [`ShellOps.cpp`](HOGN/ShellOps.cpp)                     | `openTar`, env-var substitution, explorer enumeration.                         |

---

## Tips & caveats

- **Pick unusual sequences.** Because HOG matches everything you type, choose hotkeys like `JF…` that you'd never type in normal English. The leading letters in the example config are all `JF` for this reason.
- **Backspace cleanup.** When a hotkey fires HOG sends backspaces to delete the letters you just typed, so the trigger leaves no trace in the text field. This is disabled when the foreground window is a File Explorer (where backspace navigates up).
- **One key sequence per binding.** The prefix tree means `JFCFG` and `JFCFGX` can both exist; HOG fires the longest match that completes when you release a key with no further continuation.
- **Modifier keys.** `*` requires left-`Ctrl`, `+` requires right-`Ctrl`. They appear in the sequence (e.g. `*JFYX`); other modifiers (Shift, Alt, Win) are not part of the trie syntax.
- **Clipboard repeat count.** `CMD:ACTION` reads the clipboard for the number of repetitions. Put `5` on the clipboard to run 5 rounds; non-numeric clipboard = 1 round.

---

# HOG（Hotkey Operated Gear，中文版）

HOG 是一个用 C++ 编写的、运行于 Windows 后台的轻量级工具。它通过全局低级键盘钩子监听每一次按键，当你最近输入的字母与配置文件里定义的某条前缀匹配时，就触发对应的操作：写入文本片段、打开文件 / URL / 文件夹、启动应用、控制系统音量、录制并回放鼠标键盘宏，等等。

HOG 常驻系统托盘，不会主动弹窗，所有反馈都通过一个淡入淡出的提示条 (toast) 呈现，不打断你的工作流。

---

## 主要特性

- **按字母序列触发的快捷键**。不同于 `Ctrl+Shift+X` 这种组合键，HOG 的快捷键是诸如 `JFRLD`、`JFCFG` 这样的字母序列，不会与系统或其他软件冲突。可选用 `*` / `+` 作为左 / 右 `Ctrl` 前缀。
- **多种操作类型**：输入文字、执行内置命令、打开文件 / URL / 文件夹、以管理员身份打开、应用运行时设置。
- **宏录制与回放**，回放在独立线程中执行，多轮重复时也能即时响应暂停 / 停止热键（`P` / `Q`）。`ACTION` 还会读取剪贴板里的数字作为重复次数。
- **非阻塞 Toast 通知** —— 所有触发的动作都会通过底部居中的淡入淡出提示条反馈，不再使用阻塞式 `MessageBox`。
- **配置文件自动重载** —— 一旦你通过 HOG 打开配置文件，HOG 会监听该文件，保存后自动重载。
- **剪贴板工具**：将剪贴板文字生成二维码、整理 PDF 复制的换行文本、剪贴板内容粘贴。
- **系统控制**：静音切换、定时关机 / 重启 / 取消、关闭所有资源管理器窗口等。
- **托盘菜单**：暂停 / 恢复监听、编辑 / 重载 / 切换配置文件、显示二维码、关闭所有目录、退出。
- **请求管理员权限运行**：工程默认在启动时申请管理员权限，便于触发需要提权的操作。

---

## 快速上手

1. 克隆本仓库。
2. 在 **Visual Studio 2022**（工具集 v143）中打开 `HOGN/HOGN.vcxproj`。推荐 `x64` 配置（`Win32` 也可用）。
3. 选择 `Release | x64` 构建。
4. 运行 `HOGN.exe`，托盘图标出现，HOG 开始监听。
5. 首次运行时 HOG 会创建 `C:\ProgramData\HOG\` 并询问使用默认配置路径还是自选路径。随后会自动打开配置文件等你编辑；保存后新配置自动生效。

---

## 配置文件

默认路径 `C:\ProgramData\HOG\HOG.conf`。你可以通过托盘菜单的 **Change Config** 或 `CMD:CHANGECONF` 切换；HOG 当前使用的配置路径会写在 `C:\ProgramData\HOG\config` 里。

### 行格式

```
<快捷键> <动作>
```

- `快捷键`：大写字母 `A`–`Z` 组成的序列，可在最前方加 `*`（左 `Ctrl`）和/或 `+`（右 `Ctrl`）。
- `动作`：见下文的动作前缀。
- 以 `//` 开头的行视为注释；空行被忽略。

例：

```text
JFRLD CMD:RELOAD                                          // 按 J F R L D 重载配置
*JFYX TEXT:aaaa@gmail.com                                 // 按住左 Ctrl，再按 J F Y X 输入邮箱
*+    PATH:C:\Program Files\Everything\Everything.exe     // 同时按住左右 Ctrl
```

### 动作前缀

| 前缀       | 作用                                                                 |
|------------|--------------------------------------------------------------------|
| `CMD:`     | 执行下表的内置命令。                                                  |
| `TEXT:`    | 通过模拟按键输入这段文字。                                            |
| `PATH:`    | 用 `ShellExecute` 打开目标：文件、文件夹、URL、`SHELL:` 别名都行。会自动展开 `%VAR%` 形式的环境变量。 |
| `PATH_A:`  | 与 `PATH:` 相同，但使用 `runas` 动词（弹出提权 UAC）。                |
| `SET:`     | 运行时设置。目前支持 `SET:TRAYICONENABLE:ON`/`:OFF`。                  |

### 内置命令（`CMD:`）

| 命令                       | 作用                                                                                |
|----------------------------|------------------------------------------------------------------------------------|
| `RELOAD`                   | 重新加载配置。                                                                      |
| `CONFIG`                   | 在默认编辑器中打开配置；保存后 HOG 自动重载。                                          |
| `CHANGECONF`               | 选择新的配置文件路径并重载。                                                          |
| `QUIT`                     | 弹出确认后退出 HOG。                                                                 |
| `ONOFFHOGTOGGLE`           | 暂停 / 恢复键盘监听（即使处于暂停状态也能用）。                                       |
| `QRCODE`                   | 把剪贴板里的文字生成二维码弹窗展示。                                                  |
| `MUTETOGGLE`               | 切换系统静音。                                                                      |
| `SHUTDOWN`                 | 20 秒后关机。                                                                       |
| `REBOOT`                   | 20 秒后重启。                                                                       |
| `CANCELSHUTDOWNORREBOOT`   | 取消已经计划的关机 / 重启。                                                          |
| `CLOSEALLDIRECTORIES`      | 关闭所有打开的资源管理器窗口。                                                        |
| `CLOSETHISWINDOW`          | 向前台窗口发送 `WM_CLOSE`。                                                          |
| `GOOGLESEARCH`             | 在默认浏览器中打开 `https://google.com/search?q=<剪贴板>`。                          |
| `COPYPDFTEXT`              | 整理剪贴板中从 PDF 复制的文本（合并断行、压缩多余空白），结果写回剪贴板。               |
| `OPENCMDINCURRENTPATH`     | 预留（占位实现）。                                                                  |
| `RECORD`                   | 开始 / 停止录制鼠标和键盘宏。                                                        |
| `ACTION`                   | 回放刚录制的宏。剪贴板中如果是纯数字，则被解释为重复次数（否则按 1 次执行）。           |
| `ACTIONSTOP`               | 停止正在播放的宏。                                                                  |
| `ACTIONPAUSE`              | 暂停正在播放的宏。                                                                  |
| `ACTIONRESUME`             | 恢复被暂停的宏。                                                                    |
| `ACTIONPAUSETOGGLE`        | 在暂停 / 恢复之间切换。                                                              |

### 设置（`SET:`）

| 设置项             | 取值          | 作用                              |
|------------------|--------------|---------------------------------|
| `TRAYICONENABLE` | `ON` / `OFF` | 显示或隐藏托盘图标。默认 `ON`。      |

---

## 宏的录制与回放

1. 在配置里绑定两条快捷键：
   ```text
   JFMR CMD:RECORD     // 第一次按开始录制，再按一次结束
   JFMP CMD:ACTION     // 回放刚录制的宏
   ```
2. 按 `JFMR`，提示条会确认开始录制。期间正常用鼠标和键盘即可，HOG 会捕获每一次按键和点击。
3. 再次按 `JFMR` 结束录制。连续的鼠标移动会被自动合并为一条记录，避免回放时被中间帧拖慢。
4. （可选）把一个数字（例如 `5`）复制到剪贴板，`ACTION` 会把它当作重复次数。
5. 按 `JFMP` 开始回放，回放运行在独立线程，不阻塞钩子。

回放期间，下面这些**真实物理键**会被识别（仅在播放中生效，并且会过滤 HOG 自己合成的按键）：

| 按键 | 作用              |
|----|------------------|
| `Q` | 停止回放。           |
| `P` | 暂停 / 恢复。         |

你也可以把 `CMD:ACTIONSTOP`、`CMD:ACTIONPAUSE`、`CMD:ACTIONRESUME`、`CMD:ACTIONPAUSETOGGLE` 绑到你喜欢的快捷键上。

### 互斥规则

- 录制期间禁止启动回放。
- 回放期间禁止启动录制。
- HOG 自己合成的按键（`SendInput`）会从录制流中过滤掉，避免"边录边放"形成无限循环。

---

## 托盘图标

- **左键单击**：打开默认配置文件。
- **右键单击**：弹出菜单，包括：
  - **Stop / Start Listening**：暂停或恢复键盘监听。
  - **Edit Config**：打开当前配置文件并启动自动重载监听。
  - **Reload Config**：直接重新加载配置文件。
  - **Change Config**：切换到另一个配置文件路径。
  - **Show Clipboard QRcode**：等同 `CMD:QRCODE`。
  - **Close all Directories**：等同 `CMD:CLOSEALLDIRECTORIES`。
  - **Exit**：确认后退出。

---

## Toast 通知

每次触发动作，屏幕底部居中会出现一条圆角半透明的小条幅，约 180 ms 淡入、停留 1.8 s、420 ms 淡出。快速连续触发时复用同一个窗口、重置动画——后到的消息会替换前一个。

你会看到的提示包括：

- `Reloaded config`
- `Config opened`
- `Macro recording started` / `Macro recording stopped`
- `Macro playback started` / `paused` / `resumed` / `stopped` / `finished`
- `Triggered: text input`
- `Opening: C:\Program Files`
- `Opening (admin): C:\Path\To\App.exe`

---

## 配置示例

完整带注释的示例在仓库内 [`HOG.example.conf`](HOG.example.conf)。下面是一个简化版：

```text
// ----- 托盘 / 内置开关 -----
JFAI   SET:TRAYICONENABLE:ON
JFHSWT CMD:ONOFFHOGTOGGLE

// ----- 内置命令 -----
JFRLD CMD:RELOAD
JFQT  CMD:QUIT
JFQR  CMD:QRCODE
JFCFG CMD:CONFIG
JFCAD CMD:CLOSEALLDIRECTORIES
JFMTT CMD:MUTETOGGLE
JFSD  CMD:SHUTDOWN
JFRB  CMD:REBOOT
JFCLSD CMD:CANCELSHUTDOWNORREBOOT
JFSCH CMD:GOOGLESEARCH

// ----- 宏 -----
JFMR  CMD:RECORD
JFMP  CMD:ACTION
JFMS  CMD:ACTIONSTOP
JFMPP CMD:ACTIONPAUSETOGGLE

// ----- 路径 / 启动器 -----
JFETC PATH:C:\Windows\System32\drivers\etc
JFHSZ PATH:SHELL:RecycleBinFolder
JFMD  PATH:C:\Windows\System32\cmd.exe
JFTSK PATH:C:\Windows\System32\taskmgr.exe
JFCM  PATH:C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe

// ----- 提权运行 -----
JFXTOOL PATH_A:C:\Tools\admin-only.exe

// ----- 网址 -----
JFGML PATH:https://mail.google.com
JFGPT PATH:https://chat.openai.com/chat

// ----- 文本片段（左 Ctrl + JFYX） -----
*JFYX TEXT:aaaa@gmail.com
```

---

## 从源码构建

- **工具链**：Visual Studio 2022（v143）。x64 配置使用 `/std:c++20`；Win32 配置使用工具集默认标准，代码已避免使用其不支持的特性。
- **依赖**：仅 Windows SDK 和 GDI+（已通过 `#pragma comment(lib, "gdiplus.lib")` 静态引入），无需 vcpkg / Conan / NuGet。
- **产物**：`HOGN.exe`。清单已请求管理员权限，首次运行会出现 UAC。

---

## 工程结构

代码按职责拆分在 [`HOGN/`](HOGN/) 目录下：

| 文件                                                     | 职责                                                          |
|--------------------------------------------------------|-------------------------------------------------------------|
| [`HOGN.cpp`](HOGN/HOGN.cpp)                              | `main()` —— 安装钩子并跑消息泵。                                |
| [`HOGN.h`](HOGN/HOGN.h)                                  | 项目级头文件、类型 (`letter`、`actionRecord`)、消息 ID 等。          |
| [`Globals.{h,cpp}`](HOGN/Globals.h)                      | 全局状态的 `extern` 声明 + 唯一定义。                              |
| [`Hooks.cpp`](HOGN/Hooks.cpp)                            | 低级键鼠钩子回调。                                              |
| [`Macro.cpp`](HOGN/Macro.cpp)                            | 录制 + 后台线程回放。                                            |
| [`ConfigFile.cpp`](HOGN/ConfigFile.cpp)                  | 首次安装、文件 IO、保存对话框。                                    |
| [`ConfigTrie.cpp`](HOGN/ConfigTrie.cpp)                  | 构建并遍历快捷键前缀树。                                          |
| [`ConfigWatcher.cpp`](HOGN/ConfigWatcher.cpp)            | 编辑后自动重载的文件监听。                                        |
| [`Command.cpp`](HOGN/Command.cpp)                        | `runCommand`（O(1) 命令分派）和 `excuteTar`。                     |
| [`Toast.cpp`](HOGN/Toast.cpp)                            | 圆角分层 toast 通知。                                            |
| [`Tray.cpp`](HOGN/Tray.cpp)                              | 隐藏主窗口、托盘图标、弹出菜单。                                    |
| [`QrCodeWindow.cpp`](HOGN/QrCodeWindow.cpp)              | 剪贴板文本 → 二维码弹窗。                                         |
| [`WinUtils.cpp`](HOGN/WinUtils.cpp)                      | DPI、前台窗口类名判断、文字绘制辅助。                                |
| [`Clipboard.cpp`](HOGN/Clipboard.cpp)                    | 剪贴板操作 + 按键模拟。                                          |
| [`SoundControl.cpp`](HOGN/SoundControl.cpp)              | 音量 / 静音（`IAudioEndpointVolume`）。                          |
| [`SystemControl.cpp`](HOGN/SystemControl.cpp)            | 关机 / 重启 / 权限。                                            |
| [`ShellOps.cpp`](HOGN/ShellOps.cpp)                      | `openTar`、环境变量替换、资源管理器枚举。                            |

---

## 使用小贴士

- **选用"反常"的字母组合**。HOG 监听一切按键，因此最好用平时不会出现的字母组合（示例配置全部以 `JF` 开头就是这个原因）。
- **退格自清理**。命中前缀后 HOG 会自动按下与前缀长度相同次数的退格，把你刚刚打的字母从文本框里删掉。资源管理器窗口除外（退格在那里是返回上级）。
- **同一条前缀只能映射一个动作**。但 `JFCFG` 和 `JFCFGX` 可以共存：HOG 会在你停止继续输入后选最长匹配。
- **修饰键**：`*` 表示左 `Ctrl`，`+` 表示右 `Ctrl`；其它修饰键（`Shift`、`Alt`、`Win`）不在快捷键语法里。
- **剪贴板重复次数**：`CMD:ACTION` 会把剪贴板当作整数读取作为重复次数；非数字则默认 1 次。
