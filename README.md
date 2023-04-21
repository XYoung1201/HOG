# HOG (Hotkey Operated Gear)

HOG (Hotkey Operated Gear) is a C++ based program that interacts with the Windows clipboard, allowing you to define custom key combinations and assign various actions to them. It runs in the background and listens for specific key sequences to execute corresponding actions. Here are its features and advantages:

1. **Background operation**: HOG runs in the background without interfering with your daily tasks or other applications. This feature allows you to use hotkeys without having to switch between applications or interrupt your workflow.

2. **Customizable hotkeys**: The program allows you to create your own hotkey combinations to perform actions such as opening files, launching applications, and opening websites. This customization ability enables you to tailor the hotkey system according to your specific needs and preferences.

3. **Easy-to-edit configuration file**: HOG uses a simple text-based configuration file that is easy to read and modify. You can quickly add or change hotkey combinations and their associated actions by editing this file.

4. **Supports various action types**: HOG supports a wide range of actions, including system-level configurations, opening system paths or dedicated folders, launching system applications or custom programs, executing logic scripts, opening link addresses, and using text shorthand.

5. **Quickly reloads configurations**: HOG provides a built-in hotkey combination for reloading the configuration file, allowing you to conveniently apply any changes you've made without having to restart the program.

6. **Lightweight and efficient**: The program is designed to be lightweight and efficient, ensuring minimal impact on system resources and overall performance.

7. **QR code generation**: HOG can convert text from the clipboard into a QR code and display it, providing a unique and convenient feature for quickly sharing information.

By offering these features and advantages, HOG can help increase your productivity and make common tasks more convenient and efficient through the use of custom hotkey combinations.

## Features

1. Perform actions based on keyboard input, such as opening files, folders, and applications.
2. Implement corresponding features through specific hotkey combinations.
3. Generate QR codes based on clipboard content.
4. Implement quick text input in any text box as needed.

## How to use

1. Clone or download this repository.
2. Compile the source code in a Windows environment using a C++-compatible compiler.
3. Run HOGN.exe, and the program will begin listening for keyboard input.
4. Operate according to the hotkey combinations defined in the configuration file.

## How to implement functions

After editing and saving the configuration file, pressing the specified hotkey combination will execute the corresponding action. For example, in the above example, pressing the `J`, `F`, `S`, `M` keys will paste "Sample Text" at the current cursor position.

Please note that when using hotkey combinations, avoid combinations that conflict with existing shortcuts in the operating system or other applications. To prevent conflicts, use unique hotkey combinations as much as possible.

## Detailed explanation of the configuration file

The configuration file is located at `C:\ProgramData\HOG\HOG.conf`. In this file, you can customize hotkey combinations (including the 26 letters on the keyboard and the left and right CTRL keys; when writing rules, the left CTRL is represented by `*`, and the right CTRL is represented by `+`) and corresponding actions. For example, you can define hotkey combinations to open specific files or folders or generate QR codes. The configuration file uses a simple text format and defines actions line by line. The configuration file (`C:\ProgramData\HOG\HOG.conf`) contains user-defined hotkey combinations and their corresponding actions. These combinations are represented in a simple text format, with each line representing one action.

In the file, you can add several types of actions:

1. Built-in functions
2. System paths
3. Dedicated folders
4. System applications
5. Custom applications
6. Logic scripts
7. Link addresses
8. Text shorthand

## How to configure the file

The configuration file can be edited using any text editor (such as Notepad). Follow these steps to configure the file:

1. Open a text editor, such as Notepad.
2. Open the `C:\ProgramData\HOG\HOG.conf` configuration file (or press `JFCFG` to open the configuration file).
3. Edit or add new hotkey combinations and actions, ensuring that there is a space between each combination and action.
4. Use `//` to add comments line by line to explain the function of each action.
5. Save and close the configuration file.
6. Use the predefined "reload" hotkey (in the example above, it is `JFRLD`) to reload the configuration file, or restart the program to apply the changes.

## Configuration file example

Here's a simple configuration file example:

```
// System-level configuration
JFRLD SET:RELOAD
JFQT SET:QUIT
JFCFG SET:CONFIG
JFQR SET:QRCODE

// System paths
JFPF C:\Program Files

// Dedicated folders
JFDW C:\Users\YourUsername\Documents\Work

// System applications
JFNP notepad.exe

// Custom applications
JFCH C:\Program Files\Google\Chrome\Application\chrome.exe

// Logic scripts
JFTS C:\Scripts\test_script.bat

// Link addresses
JFGH https://github.com

// Text shorthand
JFSM TEXT:Sample Text
```

- `JFRLD SET:RELOAD`: When pressing `J`, `F`, `R`, `L`, `D` keys, reload the configuration file.

- `JFQT SET:QUIT`: When pressing `J`, `F`, `Q`, `T` keys, exit the program.

- `JFCFG SET:CONFIG`: When pressing `J`, `F`, `C`, `F`, `G` keys, open the configuration file.

- `JFQR SET:QRCODE`: When pressing `J`, `F`, `Q`, `R` keys, convert the text in the clipboard to a QR code and display it.

- `JFPF C:\Program Files`: When pressing `J`, `F`, `P`, `F` keys, open the `C:\Program Files` folder.

- `JFDW C:\Users\YourUsername\Documents\Work`: When pressing `J`, `F`, `D`, `W` keys, open the `C:\Users\YourUsername\Documents\Work` folder.

- `JFNP notepad.exe`: When pressing `J`, `F`, `N`, `P` keys, launch the Notepad program.

- `JFCH C:\Program Files\Google\Chrome\Application\chrome.exe`: When pressing `J`, `F`, `C`, `H` keys, launch Google Chrome.

- `JFTS C:\Scripts\test_script.bat`: When pressing `J`, `F`, `T`, `S` keys, execute the batch script `C:\Scripts\test_script.bat`.

- `JFGH https://github.com`: When pressing `J`, `F`, `G`, `H` keys, open the `https://github.com` website using the default browser.

- `JFSM TEXT:Sample Text`: When pressing `J`, `F`, `S`, `M` keys, paste "Sample Text" at the current cursor position.

  After defining these actions in the configuration file, HOG will execute the corresponding actions when the specified hotkey combinations are pressed. Keep in mind to avoid conflicts with existing shortcuts in the operating system or other applications. Always try to use unique hotkey combinations to prevent any issues.

  Now you have a better understanding of the HOG (Hotkey Operated Gear) program, its features, and how to configure and use it. By leveraging custom hotkey combinations, you can streamline your daily tasks, improve your productivity, and make your workflow more efficient.# HOG（Hotkey Operated Gear）

HOG（Hotkey Operated Gear）是一个用C++编写的程序，它可以与Windows剪贴板交互，让您定义自定义热键组合并分配各种操作。它在后台运行，监听特定的按键序列以执行相应的操作。以下是它的特点和优点：

1. **后台运行**：HOG在后台运行，不会干扰您的日常任务或其他应用程序。这个特点让您可以使用热键，而无需在应用程序之间切换或中断您的工作流程。

2. **自定义热键**：该程序允许您创建自己的热键组合，以执行打开文件、启动应用程序、打开网站等操作。这种定制能力使您能够根据自己的具体需求和喜好定制热键系统。

3. **易于编辑的配置文件**：HOG使用一个简单的基于文本的配置文件，易于阅读和修改。您可以通过编辑此文件快速添加或更改热键组合及其关联的操作。

4. **支持多种操作类型**：HOG支持广泛的操作，包括系统级配置、打开系统路径或专用文件夹、启动系统应用程序或自定义程序、执行逻辑脚本、打开链接地址以及使用文本速记。

5. **快速重新加载配置**：HOG提供了一个内置的热键组合，用于重新加载配置文件，使您可以方便地应用所做的任何更改，而无需重新启动程序。

6. **轻量级和高效**：该程序设计为轻量级和高效，确保对系统资源和整体性能的影响最小。

7. **二维码生成**：HOG可以将剪贴板中的文本转换为二维码并显示，为快速分享信息提供了一种独特且方便的功能。

通过提供这些特点和优点，HOG可以帮助提高您的生产力，使常见任务通过使用自定义热键组合变得更加方便和高效。

## 功能

1. 根据键盘输入执行操作，如打开文件、文件夹和应用程序。
2. 通过特定快捷键组合实现相应功能。
3. 根据剪贴板内容生成二维码。
4. 根据需要在任意文本框内实现文本快速输入。

## 如何使用

1. 克隆或下载此仓库。
2. 在Windows环境下使用支持C++的编译器编译源代码。
3. 运行HOG.exe，程序将开始监听键盘输入。
4. 按照配置文件中定义的快捷键组合进行操作。

## 如何实现功能

在编辑和保存配置文件后，按下指定的快捷键组合，程序将执行对应的操作。例如，在上述示例中，按下`J`, `F`, `S`, `M`键将在当前光标位置粘贴“Sample Text”。

请注意，在使用快捷键组合时，避免使用与操作系统或其他应用程序中的现有快捷键冲突的组合。为了防止冲突，请尽量使用独特的快捷键组合。

## 配置文件详解

配置文件位于`C:\ProgramData\HOG\HOG.conf`。在此文件中，您可以自定义快捷键组合（包括键盘上26个字母以及左右CTRL键，在编写规则时左CTRL用`*`代替，右CTRL用`+`代替）以及对应的操作。例如，可以定义打开特定文件或文件夹的快捷键组合，或生成二维码。配置文件使用简单的文本格式，并以行为单位定义操作。配置文件（`C:\ProgramData\HOG\HOG.conf`）包含了用户自定义的快捷键组合及对应的操作。这些组合通过简单的文本格式表示，每行代表一个操作。

文件中，可以添加以下几种类型的操作：

1. 内置功能
2. 系统路径
3. 专用文件夹
4. 系统应用
5. 应用程序
6. 逻辑脚本
7. 链接地址
8. 文本速写

## 如何配置文件

配置文件可以通过任何文本编辑器（例如Notepad）进行编辑。按照以下步骤配置文件：

1. 打开文本编辑器，如Notepad。
2. 打开`C:\ProgramData\HOG\HOG.conf`配置文件（或者按`JFCFG`唤出配置文件）。
3. 编辑或添加新的快捷键组合和操作，确保每个组合与操作之间用空格隔开。
4. 使用`//`按行添加注释以解释每个操作的功能。
5. 保存并关闭配置文件。
6. 使用预定义的“重新加载”快捷键（在上述示例中为`JFRLD`）重新加载配置文件，或者重启程序以应用更改。

## 配置文件示例

以下是一个简单的配置文件示例：

```
// 系统级配置
JFRLD SET:RELOAD
JFQT SET:QUIT
JFCFG SET:CONFIG
JFQR SET:QRCODE

// 系统路径
JFPF C:\Program Files

// 专用文件夹
JFDW C:\Users\YourUsername\Documents\Work

// 系统应用
JFNP notepad.exe

// 应用程序
JFCH C:\Program Files\Google\Chrome\Application\chrome.exe

// 逻辑脚本
JFTS C:\Scripts\test_script.bat

// 链接地址
JFGH https://github.com

// 文本速写
JFSM TEXT:Sample Text
```

- `JFRLD SET:RELOAD`：当按下`J`, `F`, `R`, `L`, `D`键时，重新加载配置文件。
- `JFQT SET:QUIT`：当按下`J`, `F`, `Q`, `T`键时，退出程序。
- `JFCFG SET:CONFIG`：当按下`J`, `F`, `C`, `F`, `G`键时，打开配置文件。
- `JFQR SET:QRCODE`：当按下`J`, `F`, `Q`, `R`键时，将剪贴板中的文本转换为QR码并显示。
- `JFPF C:\Program Files`：当按下`J`, `F`, `P`, `F`键时，打开`C:\Program Files`文件夹。
- `JFDW C:\Users\YourUsername\Documents\Work`：当按下`J`, `F`, `D`, `W`键时，打开`C:\Users\YourUsername\Documents\Work`文件夹。
- `JFNP notepad.exe`：当按下`J`, `F`, `N`, `P`键时，启动记事本程序。
- `JFCH C:\Program Files\Google\Chrome\Application\chrome.exe`：当按下`J`, `F`, `C`, `H`键时，启动谷歌浏览器。
- `JFTS C:\Scripts\test_script.bat`：当按下`J`, `F`, `T`, `S`键时，运行批处理脚本`C:\Scripts\test_script.bat`。
- `JFGH https://github.com`：当按下`J`, `F`, `G`, `H`键时，使用默认浏览器打开`https://github.com`网址。
- `JFSM TEXT:Sample Text`：当按下`J`, `F`, `S`, `M`键时，将“Sample Text”粘贴到当前光标位置。

通过以上示例，您可以了解如何在配置文件中自定义快捷键组合和操作。请确保遵循配置文件中的格式，并使用空格分隔每个快捷键组合和操作。您可以随时根据自己的需求和喜好修改配置文件，从而使HOG提供更个性化的功能。记得在修改配置文件后，使用预定义的“重新加载”快捷键或重启程序以应用更改。