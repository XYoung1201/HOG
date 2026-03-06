# ⚙️ HOG (Hotkey Operated Gear)

> 一个以 **按键序列（Key Sequences）** 为核心交互模型的 Windows 全局热键工具。
> A Windows global hotkey utility built around **key sequences** rather than traditional shortcut chords.

![Platform](https://img.shields.io/badge/platform-Windows-blue)
![Language](https://img.shields.io/badge/language-C%2B%2B-00599C)
![Win32 API](https://img.shields.io/badge/framework-Win32_API-2C5AA0)
![Status](https://img.shields.io/badge/status-actively_usable-success)

---

## 中文介绍

### 1. 项目简介

**HOG（Hotkey Operated Gear）** 是一款面向 Windows 平台的轻量级全局热键工具，使用 **C++ + Win32 API** 原生开发。
它最大的特点，不是去和传统快捷键工具竞争“谁能绑定更多 `Ctrl + Alt + Shift + X`”，而是直接换了一种更聪明、更少冲突、更符合肌肉记忆的交互方式：

**HOG 使用“按键序列”触发动作。**

也就是说，你不需要同时按下一堆修饰键，只需要像输入 Vim 命令一样，依次按下例如：

* `J F Q R`
* `J F N P`
* `J F C F G`

即可触发不同动作。

这让 HOG 在实际使用中呈现出非常鲜明的优势：

* **冲突更少**：避开大量系统级与软件级组合键冲突。
* **记忆更自然**：序列命名可以更语义化，更像“助记口令”。
* **更适合高频工作流**：打开程序、打开目录、插入常用文本、执行系统命令，一套逻辑统一完成。
* **无需脚本语言门槛**：核心能力通过配置文件即可完成，不必先学习一门自动化 DSL。

一句话概括：

> **HOG 不是“又一个快捷键软件”，而是一种更偏工程化、更偏低冲突、更偏长期肌肉记忆的 Windows 操作增强方案。**

---

### 2. 为什么 HOG 值得关注

在同类软件里，HOG 的辨识度非常高，因为它抓住了一个很少被做好的方向：

#### 不是“更复杂”，而是“更顺手”

很多自动化工具最终会走向两个极端：

* 要么功能非常基础，只能改键、映射几个组合键；
* 要么功能极强，但需要写大量脚本，维护成本高。

HOG 走的是第三条路线：

* 用 **原生 C++** 保持轻量；
* 用 **配置文件驱动** 保持易用；
* 用 **顺序按键模型** 解决传统热键的冲突和记忆问题；
* 用 **内置命令 + 路径启动 + 文本输入 + 若干系统能力** 覆盖最常见的高频操作。

这意味着：

* 对普通用户来说，它比脚本型工具更容易上手；
* 对重度用户来说，它比传统快捷键映射器更有“体系感”；
* 对追求效率的人来说，它具备非常好的“日常常驻工具”气质。

---

### 3. 核心特性

#### 3.1 按键序列触发，而不是传统组合键

HOG 支持类似 `JFQR` 这样的序列式热键，而不是只依赖 `Ctrl + Shift + ...` 这类组合式按键。

这类设计的价值非常直接：

* 降低组合键冲突概率；
* 避免多修饰键带来的手部负担；
* 更容易构建一套“前缀一致、语义统一”的个人快捷系统；
* 很适合喜欢 Vim 风格、命令感输入的用户。

#### 3.2 纯文本配置，修改成本低

所有规则通过配置文件定义，无需重新编译。
首次运行后，程序会自动创建配置文件，例如：

```txt
C:\ProgramData\HOG\HOG.conf
```

你可以直接用任何文本编辑器维护规则，并通过热重载立即生效。

#### 3.3 支持多类动作

HOG 并不只是“启动程序”，它至少覆盖了几类非常高频的动作类型：

* **PATH:** 打开程序、文件、文件夹、系统路径
* **TEXT:** 快速输入预设文本
* **CMD:** 调用内置命令
* **SET:** 进行部分内置行为设置（如托盘图标显示状态）

这让它既能当作启动器，也能当作短语输入器，还能当作轻量系统控制台。

#### 3.4 托盘后台运行，适合长期常驻

HOG 采用托盘常驻模式，提供右键菜单进行常用操作，例如：

* Start / Stop Listening
* Edit Config
* Reload Config
* Change Config
* Show Clipboard QRcode
* Close all Directories
* Exit

这使得 HOG 在“极简后台工具”和“可视可控工具”之间取得了很好的平衡。

#### 3.5 即时热重载

配置文件修改后，不需要重启程序。
只要触发绑定到 `CMD:RELOAD` 的序列命令，即可重新加载配置。

这种反馈链路非常适合频繁调参和迭代快捷工作流。

#### 3.6 内置实用命令而非只做热键转发

从当前仓库配置样例和代码实现来看，HOG 已经内置了一批非常实用的命令能力，例如：

* 重载配置
* 退出程序
* 打开配置文件
* 展示剪贴板文本二维码
* 切换系统静音
* 关闭所有资源管理器目录窗口
* 计划关机 / 重启 / 取消关机重启
* Google 搜索剪贴板内容
* 切换程序监听开关
* 打开当前路径命令行（配置默认项中可见）

这说明 HOG 并不是“只负责监听按键，再把任务全交给外部脚本”，而是已经具备了相当完整的工具属性。

#### 3.7 文本输入与自动化能力

除了启动外部程序，HOG 还支持将预设文本快速写入当前输入位置，这对如下场景非常实用：

* 邮箱地址 / 电话 / 地址快速输入
* 常用回复语句
* 代码模板 / 命令模板
* 固定格式文本片段

对于需要减少重复输入的用户，这一点非常有价值。

#### 3.8 剪贴板二维码展示

这是 HOG 很有特色的一项功能。
它能够读取剪贴板文本，并直接生成二维码窗口显示。

这个能力在以下场景非常方便：

* 电脑向手机快速传递链接
* 临时传输命令、配置串、验证码说明
* 局域网调试时快速分享文本内容

相比单纯“复制粘贴”，这种设计更像一个真正服务于工作流的辅助工具。

#### 3.9 键盘 / 鼠标操作录制与回放（较新版本）

根据当前仓库的发布说明，较新的版本已经增加：

* `CMD:RECORD`：开始 / 停止录制
* `CMD:ACTION`：回放已录制的操作

这意味着 HOG 已经不再只是“热键 -> 动作”的静态映射工具，而开始具备轻量宏录制能力。
这一步非常重要，因为它让项目从“快捷触发器”进一步走向“轻量自动化器”。

> 注：发布说明中也明确指出，该功能当前仍有边界，例如暂不支持部分功能键记录，且录制期间不支持触发本程序快捷键。

#### 3.10 环境变量路径展开

从代码实现来看，HOG 会对路径中的环境变量进行展开，例如：

```txt
%USERPROFILE%
%APPDATA%
```

这让配置的可移植性和泛化能力更强，也更适合编写长期维护的个人配置。

---

### 4. 与主流同类软件对比

下面这张表不是为了“贬低别的软件”，而是帮助更准确地理解 **HOG 的定位**。

| 维度      | HOG                      | AutoHotkey     | PowerToys Keyboard Manager | FastKeys        |
| ------- | ------------------------ | -------------- | -------------------------- | --------------- |
| 核心触发模型  | **按键序列**（Sequence-first） | 热键 + 热字符串 + 脚本 | 键位/快捷键重映射                  | 快捷键、文本扩展、启动器、手势 |
| 上手方式    | 纯文本配置，直接写规则              | 需要学习脚本语法       | GUI 为主，最易上手                | 功能丰富，配置面较大      |
| 冲突控制    | **天然更少冲突**               | 仍以组合键体系为主      | 依赖传统快捷键体系                  | 主要还是传统快捷方式      |
| 自动化深度   | 中高：足够覆盖高频工作流             | **最高**：几乎无限扩展  | 中低：以映射和基础快捷为主              | 高：面向成套自动化       |
| 学习成本    | **低**                    | 中到高            | **最低**                     | 中等              |
| 资源与心智负担 | **轻量、聚焦**                | 灵活但维护成本更高      | 易用但目标更通用                   | 功能广，但产品面更大      |
| 最适合谁    | 想构建低冲突、高频、长期可记忆快捷系统的人    | 需要复杂自动化脚本的人    | 想快速改键/改快捷键的普通用户            | 想要一体化自动化套件的人    |

#### 更直白一点地说

* **如果你要的是“无限自动化能力”**：AutoHotkey 更强。
* **如果你要的是“官方 GUI 改键工具”**：PowerToys 更直接。
* **如果你要的是“功能大而全的一体化效率工具”**：FastKeys 很成熟。
* **如果你要的是“低冲突、序列优先、配置简单、原生轻量、真正适合常驻”的热键系统**：**HOG 的产品辨识度非常高。**

这正是 HOG 最难得的地方：

> 它没有盲目追求“什么都做”，而是把“按键序列驱动的日常工作流”这件事做出了非常明确的风格。

---

### 5. 适用场景

HOG 非常适合以下用户：

* 希望摆脱传统组合键冲突的人
* 喜欢 Vim / 命令式操作逻辑的人
* 想把常用程序、文件夹、脚本统一收敛到一套热键系统里的人
* 需要高频输入固定文本的人
* 想把 Windows 打造成“个人工作流操作台”的效率型用户
* 希望用最小维护成本获得长期收益的人

典型场景包括：

* 开发者快速打开项目目录、命令行、编辑器
* 办公用户快速输入固定格式文本和模板回复
* 内容创作者快速启动剪辑、脑图、文档、下载器等工具
* 技术用户通过序列键快速完成系统命令和管理动作

---

### 6. 快速开始

#### 6.1 直接运行

前往仓库 Releases 页面，下载可执行文件并运行。

程序启动后会进入系统托盘并开始监听。

#### 6.2 首次配置

首次运行后，默认配置文件通常位于：

```txt
C:\ProgramData\HOG\HOG.conf
```

你也可以通过界面或菜单选择其他配置路径。

#### 6.3 修改规则

使用任意文本编辑器打开配置文件，按如下格式编写：

```txt
[按键序列] [动作]
```

例如：

```txt
JFRLD CMD:RELOAD
JFQT  CMD:QUIT
JFQR  CMD:QRCODE
JFTXT PATH:C:\Windows\System32\notepad.exe
JFMAIL TEXT:your_email@example.com
```

#### 6.4 应用配置

保存配置后，触发绑定到 `CMD:RELOAD` 的序列即可热重载。

---

### 7. 配置语法说明

#### 7.1 基本格式

```txt
[Key Sequence] [Action]
```

#### 7.2 序列示例

```txt
JFQR
```

表示依次按下：

```txt
J -> F -> Q -> R
```

#### 7.3 修饰键

当前 README / 配置说明中可见：

* `*` 表示左 Ctrl
* `+` 表示右 Ctrl

例如：

```txt
*JFYX TEXT:aaaa@gmail.com
```

#### 7.4 动作前缀

* `CMD:` 内置命令
* `PATH:` 程序 / 文件 / 文件夹 / Shell 路径
* `TEXT:` 文本输入
* `SET:` 内置设置项

---

### 8. 配置示例

下面给出一个更适合 README 展示的示例：

```txt
// ===== 基础控制 =====
JFRLD CMD:RELOAD
JFQT  CMD:QUIT
JFCFG CMD:CONFIG
JFHSW CMD:ONOFFHOGTOGGLE

// ===== 系统工具 =====
JFQR  CMD:QRCODE
JFMTT CMD:MUTETOGGLE
JFCAD CMD:CLOSEALLDIRECTORIES
JFSCH CMD:GOOGLESEARCH

// ===== 启动程序 / 打开目录 =====
JFNP  PATH:notepad.exe
JFTSK PATH:C:\Windows\System32\Taskmgr.exe
JFHOG PATH:C:\ProgramData\HOG
JFDOC PATH:%USERPROFILE%\Documents

// ===== 常用文本 =====
JFMAIL TEXT:your_email@example.com
JFADDR TEXT:Shanghai, China
JFGH   TEXT:https://github.com/XYoung1201/HOG

// ===== 录制与回放（较新版本） =====
JFRC CMD:RECORD
JFFT CMD:ACTION
```

这个示例也体现了 HOG 的一个优势：

> 规则本身就很像“个人操作系统词典”，结构清晰、语义清晰、迁移成本低。

---

### 9. 项目结构

```txt
HOG/
├─ HOGN/
│  ├─ HOGN.cpp          # 主逻辑实现
│  ├─ HOGN.h            # 头文件与全局定义
│  ├─ qrcodegen.cpp     # 二维码生成实现
│  ├─ qrcodegen.hpp     # 二维码生成头文件
│  ├─ resource.h        # 资源定义
│  ├─ HOGN.rc           # Win32 资源文件
│  └─ *.ico             # 托盘图标资源
├─ HOG.example.conf     # 配置示例
├─ HOGN.sln             # Visual Studio 解决方案
└─ README.md
```

从结构上可以看出，这个项目非常明确：

* **目标单一**：围绕全局序列热键展开；
* **依赖克制**：原生 Win32 + C++；
* **功能分层清楚**：配置、托盘、二维码、动作执行等模块相对独立；
* **适合继续扩展**：尤其适合继续增加动作类型、配置校验、导入导出、录制优化等能力。

---

### 10. 构建说明

建议使用：

* **Visual Studio 2022**
* Windows 桌面开发工具链

基本步骤：

```bash
git clone https://github.com/XYoung1201/HOG.git
```

然后：

1. 使用 Visual Studio 打开 `HOGN.sln`
2. 选择 `Release` 配置
3. 编译生成可执行文件

---

### 11. HOG 的优秀之处，浓缩成一句话

如果只用一句话评价这个项目，我会这样描述：

> **HOG 不是功能最花哨的热键工具，但它很可能是“按键序列驱动效率工作流”这个方向里，最有产品气质、最有辨识度、也最值得长期打磨的一类实现。**

它优秀的地方不只在于“能做什么”，还在于它对交互模型做出了清晰选择：

* 不依赖复杂脚本才能用；
* 不被传统组合键束缚；
* 不追求臃肿；
* 而是把 **低冲突、强记忆、可配置、可常驻** 这几件真正影响日常体验的事情做扎实。

这正是一个实用主义项目最有价值的地方。

---

## English Introduction

### 1. Overview

**HOG (Hotkey Operated Gear)** is a lightweight Windows global hotkey utility built with **C++ and the Win32 API**.

What makes HOG stand out is not simply that it can launch apps or paste text. Its real innovation lies in its **interaction model**:

> **HOG uses key sequences instead of traditional shortcut chords.**

Instead of pressing combinations like `Ctrl + Alt + Shift + X`, you define and trigger actions through sequential input, such as:

* `J F Q R`
* `J F N P`
* `J F C F G`

This gives HOG a very distinct identity among Windows productivity tools:

* fewer shortcut conflicts,
* better mnemonic design,
* lower finger strain,
* and a workflow that feels closer to a command language than a pile of remapped keys.

In short:

> **HOG is not just another hotkey tool. It is a sequence-first workflow layer for Windows.**

---

### 2. Why HOG is interesting

Most tools in this category fall into one of two buckets:

1. **Very simple remappers** that only support basic key or shortcut reassignment.
2. **Very powerful automation platforms** that require users to learn and maintain scripts.

HOG takes a compelling middle path:

* **native C++ implementation** for a lightweight runtime,
* **configuration-file driven behavior** for easy maintenance,
* **sequence-based triggering** for low-conflict operation,
* **built-in commands + path launching + text insertion** for real daily usability.

That makes HOG attractive to a surprisingly broad audience:

* casual users who want more than basic remapping,
* power users who do not want to maintain large automation scripts,
* and workflow-focused users who want a personal command layer on top of Windows.

---

### 3. Core Features

#### 3.1 Sequence-based hotkeys

This is HOG’s defining feature.

Instead of relying only on chorded shortcuts, HOG supports sequence-style triggers such as:

```txt
JFQR
```

which means pressing:

```txt
J -> F -> Q -> R
```

This design is valuable because it:

* reduces collision with existing application shortcuts,
* avoids excessive modifier-key usage,
* enables mnemonic naming conventions,
* and supports a more command-oriented workflow.

#### 3.2 Plain-text configuration

HOG is highly practical because its behavior is defined in a simple configuration file rather than in compiled code.

On first run, the configuration is typically created at:

```txt
C:\ProgramData\HOG\HOG.conf
```

This makes HOG easy to inspect, edit, back up, and iterate on.

#### 3.3 Multiple action types

HOG supports several useful action categories:

* `PATH:` launch a program, file, folder, or shell path
* `TEXT:` insert predefined text
* `CMD:` execute built-in commands
* `SET:` apply certain built-in settings

That means HOG is not just a launcher. It is also a text utility, a quick system controller, and a lightweight workflow trigger engine.

#### 3.4 Tray-based background runtime

HOG runs quietly in the system tray and exposes convenient controls, including:

* Start / Stop Listening
* Edit Config
* Reload Config
* Change Config
* Show Clipboard QRcode
* Close all Directories
* Exit

This is a great balance between being silent and being controllable.

#### 3.5 Instant hot reload

You can edit the configuration file and reload it immediately via `CMD:RELOAD`, without restarting the app.

That fast feedback loop is one of the reasons HOG feels practical instead of cumbersome.

#### 3.6 Useful built-in commands

Based on the current repository, config examples, and release notes, HOG includes built-in commands for tasks such as:

* reload config
* quit the application
* open the configuration file
* show clipboard text as a QR code
* toggle system mute
* close all Explorer directory windows
* scheduled shutdown / reboot / cancel shutdown
* Google search clipboard content
* toggle listening state
* command-related helper actions in configuration defaults

This gives HOG real utility even before users start integrating external scripts.

#### 3.7 Text insertion

Text insertion support makes HOG immediately useful for:

* email addresses,
* fixed replies,
* templates,
* repetitive commands,
* structured phrases,
* and personal snippets.

This is one of the easiest ways to turn HOG into a daily productivity gain.

#### 3.8 Clipboard QR code display

This is one of HOG’s most distinctive built-in features.

It can read clipboard text and render it as a QR code window, which is very practical for:

* sending links from PC to phone,
* sharing temporary commands,
* transferring configuration strings,
* or quickly exposing machine-generated text to another device.

#### 3.9 Keyboard/mouse recording and playback

According to the latest release notes, newer versions add:

* `CMD:RECORD` to start/stop recording
* `CMD:ACTION` to replay recorded operations

This is a major step because it pushes HOG beyond static hotkey mapping and into **lightweight macro automation**.

> Note: the release notes also mention current limitations, such as incomplete support for some function-key recording scenarios.

#### 3.10 Environment-variable-aware paths

HOG expands Windows environment variables in paths, such as:

```txt
%USERPROFILE%
%APPDATA%
```

This improves portability and makes configuration cleaner and easier to maintain.

---

### 4. Comparison with mainstream tools

The goal of this comparison is not to say that HOG is “better at everything”. It is to show where HOG fits.

| Dimension                 | HOG                                                     | AutoHotkey                                   | PowerToys Keyboard Manager         | FastKeys                                         |
| ------------------------- | ------------------------------------------------------- | -------------------------------------------- | ---------------------------------- | ------------------------------------------------ |
| Trigger model             | **Key sequences**                                       | Hotkeys + hotstrings + scripting             | Key/shortcut remapping             | Shortcuts + text expansion + launcher + gestures |
| Setup style               | Plain-text config                                       | Script-driven                                | GUI-driven                         | Feature-rich configuration                       |
| Shortcut conflict profile | **Naturally low-conflict**                              | Still mostly shortcut/chord-oriented         | Traditional shortcut model         | Mostly traditional shortcut model                |
| Automation depth          | Medium to high                                          | **Very high**                                | Low to medium                      | High                                             |
| Learning curve            | **Low**                                                 | Medium to high                               | **Lowest**                         | Medium                                           |
| Product style             | Focused, lightweight, sequence-first                    | Extremely flexible, script-centric           | Official, simple, general-purpose  | Broad all-in-one productivity suite              |
| Best for                  | Users who want a memorable, low-conflict workflow layer | Users who need advanced scripting automation | Users who just want easy remapping | Users who want an integrated automation toolbox  |

#### A practical reading of this comparison

* Choose **AutoHotkey** if you need maximum scripting power.
* Choose **PowerToys Keyboard Manager** if you want a simple official remapping UI.
* Choose **FastKeys** if you want a broad all-in-one productivity suite.
* Choose **HOG** if you want a **sequence-first, conflict-resistant, highly usable hotkey layer** with a very clear product identity.

That identity is HOG’s greatest strength.

---

### 5. Typical Use Cases

HOG is especially good for users who:

* are tired of shortcut conflicts,
* like Vim-style command logic,
* want a unified launcher/text/snippet/system-control layer,
* need repetitive text insertion,
* prefer native tools over script-heavy setups,
* and want long-term productivity gains from a small utility.

Example scenarios:

* opening project folders, terminals, editors, and docs with sequences,
* inserting repeated professional phrases,
* launching work apps with semantic command-like triggers,
* controlling system tasks from the keyboard without memorizing dozens of chorded shortcuts.

---

### 6. Quick Start

#### 6.1 Run the app

Download the executable from the Releases page and launch it.

HOG will start listening in the background and appear in the system tray.

#### 6.2 Edit the config

The default configuration is typically located at:

```txt
C:\ProgramData\HOG\HOG.conf
```

Open it in any text editor and define your rules.

#### 6.3 Example

```txt
JFRLD CMD:RELOAD
JFQT  CMD:QUIT
JFQR  CMD:QRCODE
JFNP  PATH:notepad.exe
JFMAIL TEXT:your_email@example.com
```

#### 6.4 Apply changes

Save the file and trigger the sequence bound to `CMD:RELOAD`.

---

### 7. Configuration Syntax

#### Basic format

```txt
[Key Sequence] [Action]
```

#### Modifier hints

Current documentation/config examples indicate:

* `*` = Left Ctrl
* `+` = Right Ctrl

#### Action prefixes

* `CMD:` built-in commands
* `PATH:` paths / executables / folders
* `TEXT:` text insertion
* `SET:` built-in settings

---

### 8. Example Configuration

```txt
// ===== Cor
```
