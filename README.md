# C-Subject

Some small C++ projects and low-level utilities I have developed.  
我开发的一些小型 C++ 项目及底层工具集。

---

## Projects / 模块

### [STR.h](STR.h)
A dual-mode string container (`ToSTR` / `MultiSTR`) handling both narrow (`char`) and wide (`wchar_t`) strings with lazy conversion and cached ownership. Supports Windows-specific encoding detection.  
一个支持宽窄字符双槽存储与惰性转换的极客级字符串引擎，自带 Windows 平台专属编码探测。

### Stream.h (Work in Progress)
A custom console I/O engine built from scratch using `CHAR_INFO`, bypassing standard CRT buffering. Supports cross-line backspace.  
手搓的控制台 I/O 引擎，基于 `CHAR_INFO` 绕过 CRT 缓冲，支持跨行退格。

### [ShellCmdLine.h](ShellCmdLine.h)
A command-line parser for Windows, handling argument quoting and path splitting.  
Windows 平台命令行解析器，支持参数引号与路径拆分。

### Time60 (Work in Progress)
A 60-base integer type.  
一个 60 进制整数类型。

---

## Build / 构建

Tested with MinGW-w64 (GCC 16.2.0) and Clang/LLVM 22.0.0 on Windows 11.  
已在 Windows 11 上使用 MinGW-w64 (GCC 16.2.0) 与 Clang/LLVM 22.0.0 测试通过。

---

## ⚠️ Warning / 注意事项

1. **Windows Only / 仅限 Windows**  
   Uses Win32 API (`MultiByteToWideChar`, etc.). Requires `#include <windows.h>`.

2. **C++17 Required / C++ 标准**  
   Uses `if constexpr`. Requires C++17 or later.

3. **Not Thread-Safe / 线程安全**  
   Uses `mutable` caching internally. Lock it yourself if you use it in multi-threaded code.

4. **Implicit Conversion / 隐式转换**  
   `ToSTR` automatically converts between narrow/wide strings. Be aware of unexpected memory copies.
