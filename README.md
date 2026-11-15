# C-Subject

Some small C++ projects and low-level utilities I have developed.

## Projects

- **STR.h**: A dual-mode string container (`ToSTR`/`MultiSTR`) handling both narrow (`char`) and wide (`wchar_t`) strings with lazy conversion and cached ownership. Supports Windows-specific encoding detection.
- **Stream.h** (coming soon): A custom console I/O engine built from scratch using `CHAR_INFO`, bypassing standard CRT buffering. Supports cross-line backspace.
- **ShellCmdLine.h**: A command-line parser for Windows, handling argument quoting and path splitting.
- **Time60**: A 60-base integer type (work in progress).

## Build
Tested with MinGW-w64 (GCC 16.2.0) and Clang/LLVM 22.0.0 on Windows 11.