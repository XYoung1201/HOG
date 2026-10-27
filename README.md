<p align="center"><img src="docs/icon.png" alt="HOG 图标" width="96"></p>

<h1 align="center">HOG</h1>

<p align="center"><b>Hotkey Operated Gear · 用字母序列驱动的后台快捷工具</b></p>
<p align="center">随手敲几个字母，就能输入常用文本、打开程序与网址、控制系统，或回放一段键鼠宏。</p>

<p align="center">
  <b>简体中文</b> · <a href="README_EN.md">English</a>
</p>

<p align="center">
  <a href="https://github.com/XYoung1201/HOG/releases/latest"><b>下载</b></a> ·
  <a href="#快速上手"><b>快速上手</b></a> ·
  <a href="#配置说明"><b>配置说明</b></a> ·
  <a href="https://github.com/XYoung1201/HOG/issues"><b>问题反馈</b></a>
</p>

<p align="center">
  <a href="https://github.com/XYoung1201/HOG/releases/latest"><img src="https://img.shields.io/github/v/release/XYoung1201/HOG?color=2563eb" alt="最新版本"></a>
  <img src="https://img.shields.io/badge/Windows-10%20%7C%2011-0078D4?logo=windows&logoColor=white" alt="Windows 10 / 11">
  <img src="https://img.shields.io/badge/macOS-12%2B-000000?logo=apple&logoColor=white" alt="macOS 12+">
  <img src="https://img.shields.io/badge/C%2B%2B-17%20%2F%2020-00599C?logo=cplusplus&logoColor=white" alt="C++17 / C++20">
</p>

HOG 是一个常驻后台的轻量工具，提供 Windows 与 macOS 两个原生版本。它监听全局键盘输入，当你最近敲下的字母与配置文件里定义的序列（例如 `JFQR`、`JFCFG`）一致时，就执行对应动作：输入一段文本、打开文件或网址、启动程序、静音、定时关机、显示剪贴板二维码，或者回放一段录制好的键鼠宏。

它不弹主窗口，只在托盘（Windows）或菜单栏（macOS）放一个图标，用淡入淡出的提示条告诉你刚刚触发了什么。热键是字母序列而不是组合键，因此不会和系统或软件的快捷键抢位置。

**一份纯文本配置，两个平台通用。** 同样的 `CMD:`、`TEXT:`、`PATH:` 写法在 Windows 和 macOS 上都能识别，换电脑时只需要改掉与平台相关的路径。

## 目录

- [产品特性](#产品特性)
- [适用场景](#适用场景)
- [与同类工具对比](#与同类工具对比)
- [快速上手](#快速上手)
- [配置说明](#配置说明)
- [宏的录制与回放](#宏的录制与回放)
- [托盘与菜单栏](#托盘与菜单栏)
- [Windows 与 macOS 的差异](#windows-与-macos-的差异)
- [常见问题](#常见问题)
- [从源码构建](#从源码构建)
- [反馈与贡献](#反馈与贡献)

## 产品特性

| 特性 | 可以做什么 | 带来的价值 |
|---|---|---|
| 字母序列热键 | 用 `JF…` 这类日常不会连续敲出的字母组合触发动作，可加左右修饰键前缀 | 不占用组合键，记忆成本低，和任何软件都不冲突 |
| 多种动作 | 输入文本、执行内置命令、打开文件 / 文件夹 / 程序 / 网址、以管理员身份运行、切换设置 | 一个入口覆盖日常的大部分重复操作 |
| 自动清理触发字母 | 热键触发后自动退格删除刚刚敲下的字母，开着中文输入法也能删干净 | 在任何输入框里使用都不留痕迹 |
| 键鼠宏 | 录制键盘、点击、移动与拖拽，按剪贴板里的数字重复回放，`P` 暂停、`Q` 停止 | 把机械的重复操作交给程序 |
| 剪贴板工具 | 剪贴板文字生成二维码、整理从 PDF 复制的断行文本、用剪贴板内容搜索 Google | 手机扫码传文字、粘贴论文段落更顺手 |
| 系统控制 | 切换静音、20 秒后关机 / 重启并可取消、关闭所有资源管理器 / Finder 窗口 | 不用找设置面板 |
| 配置热重载 | 打开配置编辑后，保存即自动重新加载 | 改完立即生效，不必重启程序 |
| 非阻塞提示 | 每次触发都在屏幕下方显示一条圆角提示，自动淡出 | 知道发生了什么，又不打断当前工作 |

## 适用场景

| 使用需求 | 推荐做法 |
|---|---|
| 频繁输入邮箱、电话、地址等固定内容 | 为每条内容配一个 `TEXT:` 热键，例如 `*JFYX TEXT:you@example.com` |
| 常用程序、文件夹、网站来回切换 | 用 `PATH:` 把它们挂到好记的字母序列上 |
| 读论文时复制 PDF 段落 | 复制后触发 `CMD:COPYPDFTEXT`，再粘贴就是连贯的段落 |
| 手机和电脑之间传一段文字或链接 | 复制后触发 `CMD:QRCODE`，用手机扫码 |
| 重复的点击 / 填表操作 | `CMD:RECORD` 录一遍，剪贴板放入次数，`CMD:ACTION` 回放 |

## 与同类工具对比

这张表用来说明 HOG 的定位，不是说它在所有方面都更强。

| 维度 | HOG | AutoHotkey | PowerToys Keyboard Manager | FastKeys |
|---|---|---|---|---|
| 触发方式 | **字母序列** | 热键 + 热字符串 + 脚本 | 键位 / 快捷键重映射 | 快捷键、文本扩展、启动器、手势 |
| 配置方式 | 纯文本，一行一条规则 | 需要学习脚本语法 | 图形界面 | 功能多，配置项也多 |
| 快捷键冲突 | **天然很少** | 仍以组合键为主 | 传统快捷键体系 | 以传统快捷键为主 |
| 自动化深度 | 中高，覆盖高频操作 | **最高**，几乎无限扩展 | 中低 | 高 |
| 学习成本 | **低** | 中到高 | **最低** | 中等 |
| 跨平台 | **Windows + macOS** | 仅 Windows | 仅 Windows | 仅 Windows |
| 最适合 | 想要低冲突、好记、长期常驻的快捷层的人 | 需要复杂脚本自动化的人 | 只想简单改键的人 | 想要一体化效率套件的人 |

需要无限的脚本能力选 AutoHotkey，只想改键选 PowerToys，想要大而全的套件选 FastKeys；想要一套**序列优先、不抢快捷键、配置简单、轻量常驻**的热键层，就是 HOG。

## 快速上手

### 1. 下载

在 [Releases](https://github.com/XYoung1201/HOG/releases/latest) 页面下载对应平台的压缩包：

| 平台 | 文件 | 安装 |
|---|---|---|
| Windows 10 / 11（x64） | `HOGN-Windows-x64.zip` | 解压后直接运行 `HOGN.exe` |
| Windows 11 on Arm（ARM64，如 MacBook 上 Parallels Desktop 中的 Windows） | `HOGN-Windows-ARM64.zip` | 解压后直接运行 `HOGN.exe` |
| macOS 12 及以上（Apple 芯片与 Intel 通用） | `HOGN-macOS.zip` | 解压后把 `HOGN.app` 拖到「应用程序」 |

> Windows Defender 可能误报，macOS 可能提示「无法打开」，原因和处理方法见[常见问题](#常见问题)。程序源码完全公开。

### 2. 首次运行

**Windows**：启动后托盘出现 HOG 图标。程序会创建 `C:\ProgramData\HOG\`，询问是否使用默认配置文件，然后用默认编辑器打开它。

**macOS**：

1. 打开 `HOGN.app`，菜单栏出现手形图标。程序会询问使用默认配置（`~/Library/Application Support/HOG/HOG.conf`），还是选择一个已有的配置文件。
2. 在「系统设置 → 隐私与安全性」中，为 HOGN 打开 **辅助功能** 和 **输入监控** 两项权限。授权后约 2 秒，屏幕下方提示 `HOG is running`，无需重启。

### 3. 试一个热键

在任意输入框里依次敲 `J` `F` `Q` `R`：这几个字母会被自动删除，并弹出当前剪贴板文字的二维码。随后敲 `JFCFG` 打开配置文件，添加自己的热键，保存后自动生效。

## 配置说明

配置文件是 UTF-8 纯文本，默认位置：

| 平台 | 配置文件 | 记录当前配置路径的文件 |
|---|---|---|
| Windows | `C:\ProgramData\HOG\HOG.conf` | `C:\ProgramData\HOG\config` |
| macOS | `~/Library/Application Support/HOG/HOG.conf` | `~/Library/Application Support/HOG/config` |

可以通过托盘 / 菜单栏的 **Change Config** 或 `CMD:CHANGECONF` 改用其他位置的配置文件。

### 行格式

```text
<热键> <动作>
```

- **热键**：一个或多个字母 `A`–`Z`（不区分大小写），前面可以加修饰键前缀：

  | 前缀 | Windows | macOS |
  |---|---|---|
  | `*` | 左 Ctrl | 左 ⌘ Command |
  | `+` | 右 Ctrl | 右 ⌘ Command |

  修饰键是「先按一下再松开」，不是按住；按住修饰键时不会触发任何热键，因此不影响 ⌘C、Ctrl+V 之类的正常快捷键。
- **动作**：见下方动作前缀。
- 以 `//` 开头的行是注释，空行会被忽略。Windows 下保存的 CRLF 换行在 macOS 版里同样可以读取。

```text
JFRLD CMD:RELOAD                     // 依次敲 J F R L D，重新加载配置
*JFYX TEXT:you@example.com           // 先按一下左 Ctrl/⌘，再敲 J F Y X，输入邮箱
JFGH  PATH:https://github.com        // 用默认浏览器打开网址
```

### 动作前缀

| 前缀 | 作用 | Windows | macOS |
|---|---|---|---|
| `TEXT:` | 输入一段文本 | 模拟按键逐字输入 | 经剪贴板粘贴后恢复原剪贴板，支持中文、emoji，开着拼音输入法也能用 |
| `PATH:` | 打开目标 | `ShellExecute`：文件、文件夹、程序、网址、`SHELL:RecycleBinFolder` 等；展开 `%VAR%` | 文件、文件夹、`.app`、应用名（如 `PATH:Safari`）、网址；展开 `~`、`$VAR`、`%VAR%`；`.sh` / `.command` 脚本在「终端」中运行 |
| `PATH_A:` | 以管理员身份打开 | `runas`，弹出 UAC | 输入密码后通过 `do shell script … with administrator privileges` 运行 |
| `CMD:` | 执行内置命令 | 见下表 | 见下表 |
| `SET:` | 切换设置 | `SET:TRAYICONENABLE:ON` / `OFF` 显示或隐藏图标 | 同左 |

### 内置命令

| 命令 | 作用 | macOS 上的实现 |
|---|---|---|
| `RELOAD` | 重新加载配置 | 同 Windows |
| `CONFIG` | 用编辑器打开配置，保存后自动重载 | 用默认纯文本编辑器打开 |
| `CHANGECONF` | 选择另一个配置文件并重载 | 同 Windows |
| `QUIT` | 确认后退出 | 同 Windows |
| `ONOFFHOGTOGGLE` | 暂停 / 恢复监听（暂停时依然可用） | 同 Windows |
| `QRCODE` | 显示剪贴板文字的二维码 | 同 Windows，按 Esc 关闭 |
| `MUTETOGGLE` | 切换系统静音 | 同 Windows |
| `SHUTDOWN` / `REBOOT` | 20 秒后关机 / 重启 | 同样倒计时 20 秒 |
| `CANCELSHUTDOWNORREBOOT` | 取消待执行的关机 / 重启 | 同 Windows |
| `CLOSEALLDIRECTORIES` | 关闭所有资源管理器窗口 | 关闭所有 Finder 窗口 |
| `CLOSETHISWINDOW` | 关闭当前前台窗口 | 发送 ⌘W |
| `GOOGLESEARCH` | 用 Google 搜索剪贴板内容 | 同 Windows |
| `COPYPDFTEXT` | 整理从 PDF 复制的文本（合并断行、保留句末换行、压缩空格） | 同 Windows |
| `OPENCMDINCURRENTPATH` | 在当前目录打开命令行 | Windows 版暂未实现；macOS 在最前 Finder 窗口所在目录打开「终端」 |
| `RECORD` | 开始 / 停止录制键鼠宏 | 同 Windows |
| `ACTION` | 回放宏，剪贴板中的数字为重复次数（非数字则为 1） | 同 Windows |
| `ACTIONSTOP` / `ACTIONPAUSE` / `ACTIONRESUME` / `ACTIONPAUSETOGGLE` | 停止 / 暂停 / 继续 / 切换暂停 | 同 Windows |

完整的配置示例见 [`HOG.example.conf`](HOG.example.conf)（Windows 路径）。macOS 版首次运行生成的默认配置已经换成了 Mac 上的路径。

## 宏的录制与回放

1. 在配置中绑定两个热键：

   ```text
   JFMR CMD:RECORD     // 第一次触发开始录制，再次触发停止
   JFMP CMD:ACTION     // 回放
   ```

2. 触发 `JFMR` 开始录制，正常操作键盘和鼠标。macOS 版还会记录 ⌘ ⌥ ⇧ 等修饰键和拖拽。
3. 再次触发 `JFMR` 停止，用于停止录制的那几个字母会被自动去掉。
4. 如需重复，在剪贴板里放一个数字（例如 `5`），再触发 `JFMP`。

回放在独立线程中进行。回放期间按实体键 `P` 暂停 / 继续，按 `Q` 停止；HOG 自己发出的按键会被过滤，宏不会把自己停掉。录制时不能回放，回放时也不能录制。

## 托盘与菜单栏

| 菜单项 | 作用 |
|---|---|
| Stop / Start Listening | 暂停或恢复监听，暂停时图标变灰 |
| Edit Config | 打开配置并开启自动重载 |
| Reload Config | 立即重新加载配置 |
| Change Config | 选择其他配置文件 |
| Show Clipboard QRcode | 同 `CMD:QRCODE` |
| Close all Directories | 同 `CMD:CLOSEALLDIRECTORIES` |
| Exit | 确认后退出 |

Windows 托盘图标右键打开菜单，左键打开配置文件；macOS 点击菜单栏图标打开菜单。

## Windows 与 macOS 的差异

| 项目 | Windows | macOS |
|---|---|---|
| 图标位置 | 系统托盘 | 菜单栏 |
| `*` / `+` | 左 / 右 Ctrl | 左 / 右 ⌘ Command |
| 键盘监听 | `WH_KEYBOARD_LL` / `WH_MOUSE_LL` 低级钩子 | `CGEventTap`，需要辅助功能与输入监控权限 |
| `TEXT:` 输入方式 | 模拟按键 | 剪贴板粘贴并恢复 |
| 不删除触发字母的窗口 | 资源管理器 | Finder |
| 单实例保护 | 全局互斥量 | 配置目录下的文件锁 |

## 常见问题

**Windows Defender 报毒？**
HOG 需要全局键盘钩子和模拟按键，这类行为容易被误判。程序源码完全公开，可以在 Defender 中允许该文件，或者自行编译。

**macOS 提示「无法打开」或「已损坏」？**
发布包没有经过 Apple 公证。在「终端」中执行一次下面的命令即可：

```bash
xattr -dr com.apple.quarantine /Applications/HOGN.app
```

**macOS 上热键没有反应？**
确认「系统设置 → 隐私与安全性」的 **辅助功能** 与 **输入监控** 中 HOGN 都已打开。如果开关是开的但仍无效（例如替换了新版本），先用 − 删除 HOGN 条目，再用 + 重新添加 `/Applications/HOGN.app`。

**开着中文输入法时，触发字母没有删干净？**
请使用 3.1 及以上版本。HOG 会等目标窗口和输入法处理完退格之后再执行动作。

**第一次使用某些命令时弹出「想要控制 Finder / System Events」？**
这是 macOS 对自动化操作的确认，`CLOSEALLDIRECTORIES`、`OPENCMDINCURRENTPATH`、`MUTETOGGLE`、`SHUTDOWN` 等命令需要它，允许一次即可。

**热键和正常打字冲突？**
HOG 会匹配你敲下的所有字母，所以请选日常不会连续出现的序列，例如统一以 `JF` 开头。两个字母之间间隔超过 1 秒会重新开始匹配。

## 从源码构建

### Windows

1. 用 **Visual Studio 2022**（工具集 v143）打开 `HOGN.sln`。
2. 选择 `Release | x64` 生成，得到 `HOGN.exe`；Windows on Arm 选 `Release | ARM64`（需要安装 VS 的 ARM64 生成工具和 ATL）。只依赖 Windows SDK 和 GDI+。

### macOS

只需要 Xcode 命令行工具（`xcode-select --install`），不需要完整的 Xcode：

```bash
cd macOS
make install      # 生成 build/HOGN.app 并复制到 /Applications
make run          # 或者直接从 build 目录运行
```

程序使用与 bundle id 绑定的签名要求，重新编译后已授予的权限依然有效。

### 工程结构

| 路径 | 内容 |
|---|---|
| [`HOGN/`](HOGN/) | Windows 版：Win32 + GDI+，按功能拆分为 `Hooks`、`Command`、`ConfigTrie`、`Macro`、`Toast`、`Tray` 等模块 |
| [`macOS/src/`](macOS/src/) | macOS 版：Objective-C++，模块与 Windows 版一一对应，热键前缀树逻辑相同 |
| [`HOGN/qrcodegen.*`](HOGN/qrcodegen.hpp) | 两个版本共用的二维码库 |
| [`.github/workflows/release.yml`](.github/workflows/release.yml) | 推送 `v*` 标签时自动编译 Windows x64 / ARM64 与 macOS 并上传到 Release |

## 反馈与贡献

遇到问题或有新功能想法，欢迎在 [Issues](https://github.com/XYoung1201/HOG/issues) 中提出，也欢迎提交 Pull Request。

[![Star History Chart](https://api.star-history.com/svg?repos=XYoung1201/HOG&type=Timeline)](https://star-history.com/#XYoung1201/HOG&Timeline)
