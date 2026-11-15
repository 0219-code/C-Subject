<br>
# C-Subject

Some small C++ projects and low-level utilities I have developed.

## Projects

- **STR.h**: A dual-mode string container (`ToSTR`/`MultiSTR`) handling both narrow (`char`) and wide (`wchar_t`) strings with lazy conversion and cached ownership. Supports Windows-specific encoding detection.
- **Stream.h** (coming soon): A custom console I/O engine built from scratch using `CHAR_INFO`, bypassing standard CRT buffering. Supports cross-line backspace.
- **ShellCmdLine.h**: A command-line parser for Windows, handling argument quoting and path splitting.
- **Time60**: A 60-base integer type (work in progress).

## Build
Tested with MinGW-w64 (GCC 16.2.0) and Clang/LLVM 22.0.0 on Windows 11.
<br>
# C-Subject

Windows 底层的野生 C++ 工具集，封装了各种 Windows API 和字面量。

## 模块
- **STR.h**: 一个支持宽窄字符双槽存储与惰性转换的极客级字符串引擎 (`ToSTR`/`MultiSTR`)。

## 注意事项 (Warning)
1. **仅限 Windows**：本库大量使用了 Win32 API (`MultiByteToWideChar` 等)，需要包含 `<windows.h>`。
2. **C++ 标准**：使用了 `if constexpr` 等特性，请务必使用 `C++17` 或更高版本编译。
3. **线程安全**：内部使用了 `mutable` 缓存转换结果，**非线程安全**，多线程操作时请自行加锁。
4. **隐式转换**：`ToSTR` 会自动进行宽窄字符转换，但有可能会导致意外的内存拷贝，请留意。
5. <br>
