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
- `JFSM TEXT:Sample Text`: When pressing `J
- `, `F`, `S`, `M` keys, paste "Sample Text" at the current cursor position.

  After defining these actions in the configuration file, HOG will execute the corresponding actions when the specified hotkey combinations are pressed. Keep in mind to avoid conflicts with existing shortcuts in the operating system or other applications. Always try to use unique hotkey combinations to prevent any issues.

  Now you have a better understanding of the HOG (Hotkey Operated Gear) program, its features, and how to configure and use it. By leveraging custom hotkey combinations, you can streamline your daily tasks, improve your productivity, and make your workflow more efficient.