<p align="center"><img src="docs/icon.png" alt="HOG icon" width="96"></p>

<h1 align="center">HOG</h1>

<p align="center"><b>Hotkey Operated Gear · a background helper driven by letter sequences</b></p>
<p align="center">Type a few letters to insert a snippet, open an app or URL, control the system, or replay a keyboard-and-mouse macro.</p>

<p align="center">
  <a href="README.md">简体中文</a> · <b>English</b>
</p>

<p align="center">
  <a href="https://github.com/XYoung1201/HOG/releases/latest"><b>Download</b></a> ·
  <a href="#quick-start"><b>Quick Start</b></a> ·
  <a href="#configuration"><b>Configuration</b></a> ·
  <a href="https://github.com/XYoung1201/HOG/issues"><b>Issues</b></a>
</p>

<p align="center">
  <a href="https://github.com/XYoung1201/HOG/releases/latest"><img src="https://img.shields.io/github/v/release/XYoung1201/HOG?color=2563eb" alt="Latest release"></a>
  <img src="https://img.shields.io/badge/Windows-10%20%7C%2011-0078D4?logo=windows&logoColor=white" alt="Windows 10 / 11">
  <img src="https://img.shields.io/badge/macOS-12%2B-000000?logo=apple&logoColor=white" alt="macOS 12+">
  <img src="https://img.shields.io/badge/C%2B%2B-17%20%2F%2020-00599C?logo=cplusplus&logoColor=white" alt="C++17 / C++20">
</p>

HOG is a lightweight background utility with native Windows and macOS versions. It watches global keyboard input, and when the letters you just typed match a sequence from your config file (such as `JFQR` or `JFCFG`), it runs the matching action: type a snippet, open a file or URL, launch an app, toggle mute, schedule a shutdown, show the clipboard as a QR code, or replay a recorded macro.

It has no main window — just an icon in the system tray (Windows) or menu bar (macOS) — and reports what it did through a fading toast. Hotkeys are letter sequences rather than key chords, so they never compete with system or app shortcuts.

**One plain-text config for both platforms.** The same `CMD:`, `TEXT:` and `PATH:` lines work on Windows and macOS; when you switch machines, only the platform-specific paths need changing.

## Contents

- [Features](#features)
- [Use Cases](#use-cases)
- [Comparison with Similar Tools](#comparison-with-similar-tools)
- [Quick Start](#quick-start)
- [Configuration](#configuration)
- [Macro Recording and Playback](#macro-recording-and-playback)
- [Tray and Menu Bar](#tray-and-menu-bar)
- [Windows vs. macOS](#windows-vs-macos)
- [FAQ](#faq)
- [Building from Source](#building-from-source)
- [Feedback and Contributing](#feedback-and-contributing)

## Features

| Feature | What it does | Why it helps |
|---|---|---|
| Letter-sequence hotkeys | Trigger actions with sequences like `JF…` that never occur in normal typing, with optional left/right modifier prefixes | No chords to reserve, easy to remember, no conflicts with other apps |
| Many action types | Type text, run built-in commands, open files / folders / apps / URLs, run as administrator, toggle settings | One entry point for most repetitive tasks |
| Trigger cleanup | Deletes the letters you typed once a hotkey fires — also with a Chinese input method active | Use hotkeys in any text field without leaving a trace |
| Keyboard & mouse macros | Record keys, clicks, moves and drags; repeat N times from the clipboard; `P` pauses, `Q` stops | Hand mechanical repetition to the computer |
| Clipboard tools | QR code from clipboard text, reflow text copied from PDFs, Google the clipboard | Move text to your phone, paste paper excerpts cleanly |
| System control | Toggle mute, shut down / reboot in 20 s (cancellable), close all Explorer / Finder windows | No hunting through settings |
| Live config reload | After opening the config for editing, saving reloads it automatically | Changes take effect immediately |
| Non-blocking toasts | A rounded toast near the bottom of the screen confirms each action and fades out | Feedback without interruptions |

## Use Cases

| Need | Suggested setup |
|---|---|
| Typing the same email, phone number or address | One `TEXT:` hotkey per snippet, e.g. `*JFYX TEXT:you@example.com` |
| Jumping between favourite apps, folders and sites | Bind them with `PATH:` to memorable sequences |
| Copying paragraphs out of PDFs | Copy, trigger `CMD:COPYPDFTEXT`, paste a clean paragraph |
| Sending text or a link to your phone | Copy, trigger `CMD:QRCODE`, scan |
| Repetitive clicking / form filling | Record once with `CMD:RECORD`, put a count on the clipboard, replay with `CMD:ACTION` |

## Comparison with Similar Tools

This table shows where HOG fits; it doesn't claim HOG is better at everything.

| Dimension | HOG | AutoHotkey | PowerToys Keyboard Manager | FastKeys |
|---|---|---|---|---|
| Trigger model | **Letter sequences** | Hotkeys + hotstrings + scripts | Key / shortcut remapping | Shortcuts, text expansion, launcher, gestures |
| Setup | Plain text, one rule per line | Scripting language | GUI | Feature-rich configuration |
| Shortcut conflicts | **Naturally rare** | Mostly chord-based | Traditional shortcuts | Mostly traditional shortcuts |
| Automation depth | Medium to high | **Very high** | Low to medium | High |
| Learning curve | **Low** | Medium to high | **Lowest** | Medium |
| Platforms | **Windows + macOS** | Windows only | Windows only | Windows only |
| Best for | A memorable, low-conflict, always-on shortcut layer | Advanced scripted automation | Simple remapping | An all-in-one productivity suite |

Pick AutoHotkey for maximum scripting power, PowerToys for simple remapping, FastKeys for a broad suite — and HOG for a **sequence-first, conflict-free, simple, lightweight** hotkey layer.

## Quick Start

### 1. Download

Get the archive for your platform from [Releases](https://github.com/XYoung1201/HOG/releases/latest):

| Platform | File | Install |
|---|---|---|
| Windows 10 / 11 (x64) | `HOGN-Windows-x64.zip` | Unzip and run `HOGN.exe` |
| Windows 11 on Arm (ARM64, e.g. Windows in Parallels Desktop on a MacBook) | `HOGN-Windows-ARM64.zip` | Unzip and run `HOGN.exe` |
| macOS 12+ (Apple silicon and Intel) | `HOGN-macOS.zip` | Unzip and drag `HOGN.app` into Applications |

> Windows Defender may flag it and macOS may refuse to open it at first — see the [FAQ](#faq). The full source is in this repository.

### 2. First launch

**Windows**: the HOG icon appears in the tray. HOG creates `C:\ProgramData\HOG\`, asks whether to use the default config file, and opens it in your default editor.

**macOS**:

1. Open `HOGN.app`; a hand icon appears in the menu bar. HOG asks whether to use the default config (`~/Library/Application Support/HOG/HOG.conf`) or an existing file.
2. In **System Settings → Privacy & Security**, turn on HOGN under both **Accessibility** and **Input Monitoring**. About two seconds later a `HOG is running` toast appears — no restart needed.

### 3. Try a hotkey

In any text field type `J` `F` `Q` `R`: the letters disappear and a QR code of your clipboard text pops up. Then type `JFCFG` to open the config, add your own hotkeys, and save — they take effect right away.

## Configuration

The config is a UTF-8 text file. Default locations:

| Platform | Config file | File recording the active config path |
|---|---|---|
| Windows | `C:\ProgramData\HOG\HOG.conf` | `C:\ProgramData\HOG\config` |
| macOS | `~/Library/Application Support/HOG/HOG.conf` | `~/Library/Application Support/HOG/config` |

Use **Change Config** in the tray / menu bar, or `CMD:CHANGECONF`, to point HOG at another file.

### Line format

```text
<HOTKEY> <ACTION>
```

- **HOTKEY**: one or more letters `A`–`Z` (case-insensitive), optionally prefixed with modifiers:

  | Prefix | Windows | macOS |
  |---|---|---|
  | `*` | Left Ctrl | Left ⌘ Command |
  | `+` | Right Ctrl | Right ⌘ Command |

  A modifier is tapped (pressed and released), not held. While a modifier is held nothing fires, so shortcuts like ⌘C or Ctrl+V are unaffected.
- **ACTION**: one of the prefixes below.
- Lines starting with `//` are comments; blank lines are ignored. The macOS version also reads CRLF files saved on Windows.

```text
JFRLD CMD:RELOAD                     // type J F R L D to reload the config
*JFYX TEXT:you@example.com           // tap left Ctrl/⌘, then type J F Y X to insert an email
JFGH  PATH:https://github.com        // open a URL in the default browser
```

### Action prefixes

| Prefix | Effect | Windows | macOS |
|---|---|---|---|
| `TEXT:` | Insert text | Simulated keystrokes | Pasted via the clipboard, which is then restored; supports CJK and emoji and works with input methods such as Pinyin |
| `PATH:` | Open a target | `ShellExecute`: files, folders, programs, URLs, `SHELL:RecycleBinFolder`…; expands `%VAR%` | Files, folders, `.app` bundles, app names (`PATH:Safari`), URLs; expands `~`, `$VAR`, `%VAR%`; `.sh` / `.command` scripts run in Terminal |
| `PATH_A:` | Open as administrator | `runas` verb (UAC prompt) | Asks for your password, runs via `do shell script … with administrator privileges` |
| `CMD:` | Run a built-in command | See below | See below |
| `SET:` | Toggle a setting | `SET:TRAYICONENABLE:ON` / `OFF` shows or hides the icon | Same |

### Built-in commands

| Command | Effect | On macOS |
|---|---|---|
| `RELOAD` | Reload the config | Same |
| `CONFIG` | Open the config in an editor; reload on save | Opens in the default plain-text editor |
| `CHANGECONF` | Pick another config file and reload | Same |
| `QUIT` | Confirm and exit | Same |
| `ONOFFHOGTOGGLE` | Pause / resume listening (works while paused) | Same |
| `QRCODE` | Show the clipboard text as a QR code | Same; Esc closes it |
| `MUTETOGGLE` | Toggle system mute | Same |
| `SHUTDOWN` / `REBOOT` | Shut down / reboot in 20 s | Same 20 s countdown |
| `CANCELSHUTDOWNORREBOOT` | Cancel a pending shutdown / reboot | Same |
| `CLOSEALLDIRECTORIES` | Close all File Explorer windows | Closes all Finder windows |
| `CLOSETHISWINDOW` | Close the foreground window | Sends ⌘W |
| `GOOGLESEARCH` | Google the clipboard text | Same |
| `COPYPDFTEXT` | Reflow text copied from a PDF (join broken lines, keep sentence breaks, collapse spaces) | Same |
| `OPENCMDINCURRENTPATH` | Open a shell in the current folder | Not implemented on Windows yet; on macOS opens Terminal in the front Finder window's folder |
| `RECORD` | Start / stop recording a macro | Same |
| `ACTION` | Replay the macro; a number on the clipboard sets the repeat count (default 1) | Same |
| `ACTIONSTOP` / `ACTIONPAUSE` / `ACTIONRESUME` / `ACTIONPAUSETOGGLE` | Stop / pause / resume / toggle pause | Same |

A complete example config (Windows paths) is in [`HOG.example.conf`](HOG.example.conf). The default config the macOS version writes on first launch already uses Mac paths.

## Macro Recording and Playback

1. Bind two hotkeys:

   ```text
   JFMR CMD:RECORD     // first trigger starts recording, second stops it
   JFMP CMD:ACTION     // replay
   ```

2. Trigger `JFMR` and use the keyboard and mouse normally. The macOS version also records ⌘ ⌥ ⇧ modifiers and drags.
3. Trigger `JFMR` again; the letters of the stop hotkey are trimmed from the recording.
4. To repeat, put a number such as `5` on the clipboard, then trigger `JFMP`.

Playback runs on its own thread. During playback press the physical `P` key to pause / resume and `Q` to stop; HOG ignores its own synthesized input, so a macro cannot stop itself. Recording and playback are mutually exclusive.

## Tray and Menu Bar

| Menu item | Effect |
|---|---|
| Stop / Start Listening | Pause or resume listening; the icon turns grey while paused |
| Edit Config | Open the config and start auto-reload |
| Reload Config | Reload the config now |
| Change Config | Pick another config file |
| Show Clipboard QRcode | Same as `CMD:QRCODE` |
| Close all Directories | Same as `CMD:CLOSEALLDIRECTORIES` |
| Exit | Confirm and quit |

On Windows, right-click the tray icon for the menu and left-click it to open the config; on macOS, click the menu bar icon.

## Windows vs. macOS

| Aspect | Windows | macOS |
|---|---|---|
| Icon | System tray | Menu bar |
| `*` / `+` | Left / right Ctrl | Left / right ⌘ Command |
| Input monitoring | `WH_KEYBOARD_LL` / `WH_MOUSE_LL` hooks | `CGEventTap`; needs Accessibility and Input Monitoring |
| `TEXT:` | Simulated keystrokes | Clipboard paste, then restore |
| Window where trigger letters aren't deleted | File Explorer | Finder |
| Single instance | Global mutex | File lock in the config folder |

## FAQ

**Windows Defender flags the exe.**
HOG uses a global keyboard hook and synthesized input, which antivirus heuristics often misread. The source is public; allow the file in Defender or build it yourself.

**macOS says the app "can't be opened" or "is damaged".**
The release build isn't notarized by Apple. Run this once in Terminal:

```bash
xattr -dr com.apple.quarantine /Applications/HOGN.app
```

**Hotkeys do nothing on macOS.**
Check that HOGN is on under both **Accessibility** and **Input Monitoring** in System Settings → Privacy & Security. If it's on but still ignored (for example after replacing the app with a newer build), remove the HOGN entry with − and add `/Applications/HOGN.app` again with +.

**With a Chinese input method on, the trigger letters aren't fully deleted.**
Use version 3.1 or later: HOG now waits until the target window and the input method have processed the backspaces before running the action.

**macOS asks whether HOGN may control Finder / System Events.**
That's macOS's automation consent, needed by `CLOSEALLDIRECTORIES`, `OPENCMDINCURRENTPATH`, `MUTETOGGLE`, `SHUTDOWN` and similar commands. Allow it once.

**Hotkeys clash with normal typing.**
HOG matches everything you type, so choose sequences that never appear in regular text — e.g. start them all with `JF`. A pause of more than one second between letters restarts matching.

## Building from Source

### Windows

1. Open `HOGN.sln` in **Visual Studio 2022** (toolset v143).
2. Build `Release | x64` to get `HOGN.exe`, or `Release | ARM64` for Windows on Arm (needs the VS ARM64 build tools and ATL). Only the Windows SDK and GDI+ are needed.

### macOS

Only the Xcode Command Line Tools are required (`xcode-select --install`):

```bash
cd macOS
make install      # builds build/HOGN.app and copies it to /Applications
make run          # or run it straight from the build folder
```

The app is signed with a requirement tied to its bundle id, so privacy permissions survive rebuilds.

### Project layout

| Path | Contents |
|---|---|
| [`HOGN/`](HOGN/) | Windows version: Win32 + GDI+, split into modules such as `Hooks`, `Command`, `ConfigTrie`, `Macro`, `Toast`, `Tray` |
| [`macOS/src/`](macOS/src/) | macOS version: Objective-C++ with modules mirroring the Windows ones and the same hotkey trie |
| [`HOGN/qrcodegen.*`](HOGN/qrcodegen.hpp) | QR code library shared by both versions |
| [`.github/workflows/release.yml`](.github/workflows/release.yml) | Builds Windows x64 / ARM64 and macOS and uploads them to a Release when a `v*` tag is pushed |

## Feedback and Contributing

Bug reports and feature ideas are welcome in [Issues](https://github.com/XYoung1201/HOG/issues); pull requests too.

[![Star History Chart](https://api.star-history.com/svg?repos=XYoung1201/HOG&type=Timeline)](https://star-history.com/#XYoung1201/HOG&Timeline)
