#pragma once

#include "STR.h"
#include <deque>
#include <windows.h>
#include <shellapi.h>

using namespace std;
typedef int ARGC;

class ShellLocalFreeDeleter;
struct ShellLocalFreeDeleter {
    void operator()(LPWSTR* p) const noexcept {
        if (p) LocalFree(p);
    }
};

class ShellCmdLine;
struct ShellCmdLine {
    ShellCmdLine();
    ARGC argc;
    unique_ptr<LPWSTR[], ShellLocalFreeDeleter> argv;
    MultiSTR lpFile = "";
    MultiSTR lpParameters = "";
    MultiSTR lpDir = "";
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    MultiSTR lpCmdLine = "";
    INT nCmdShow = SW_SHOWNORMAL;
    STARTUPINFOW si = { sizeof(si) };
    BOOL AnalyzeCmdLine(MultiCSTR &cmdLine);
    SHELLEXECUTEINFOW GetCmdLine() const;
    MultiSTR GetCmdLineAsSTR();
};
inline BOOL ShellCmdLine::AnalyzeCmdLine(MultiCSTR &cmdLine) {
    argc = 0;
    argv.reset();
    lpParameters.clear();
    argv.reset(CommandLineToArgvW(cmdLine, &argc));
    if (argv == nullptr) {
        sei.cbSize = 0;
        ret FALSE;
    }
    sei.cbSize = sizeof(sei);
    sei.lpVerb = L"open";
    for (int i = 1; i < argc; i++) {
        if (i > 1) lpParameters += L" ";
        if (wcsstr(argv[i], L" ") || wcsstr(argv[i], L"\t")) {
            lpParameters += L"\"";
            lpParameters += argv[i];
            lpParameters += L"\"";
        } else lpParameters += argv[i];
    }
    if (lpParameters.empty()) sei.lpParameters = nullptr;
    else sei.lpParameters = lpParameters;
    ret TRUE;
}

inline ShellCmdLine::ShellCmdLine() : argc(0), argv(nullptr) {
    if (AnalyzeCmdLine(GetCommandLineW()) == FALSE) ret;
    lpFile.clear();
    lpFile.StoredWSTR.reset(new WSTR[MAX_PATH]());
    DWORD fileLen = GetModuleFileNameW(nullptr, lpFile.StoredWSTR.get(), MAX_PATH);
    if (fileLen == 0) lpFile.clear();
    else if (fileLen == MAX_PATH) lpFile.StoredWSTR.get()[MAX_PATH - 1] = L'\0';
    sei.lpFile = lpFile;
    GetStartupInfoW(&si);
    if (si.dwFlags & STARTF_USESHOWWINDOW) nCmdShow = si.wShowWindow;
    sei.nShow = nCmdShow;
    lpDir.clear();
    lpDir.StoredWSTR.reset(new WSTR[MAX_PATH]());
    DWORD dirLen = GetCurrentDirectoryW(MAX_PATH, lpDir.StoredWSTR.get());
    if (dirLen == 0) lpDir.clear();
    else if (dirLen >= MAX_PATH) lpDir.StoredWSTR.get()[MAX_PATH - 1] = L'\0';
    sei.lpDirectory = lpDir;
}

inline SHELLEXECUTEINFOW ShellCmdLine::GetCmdLine() const {
    ret sei;
}
inline MultiSTR ShellCmdLine::GetCmdLineAsSTR() {
    lpCmdLine = lpFile;
    if (lpParameters.empty());
    else {
        lpCmdLine += L" ";
        lpCmdLine += lpParameters;
    }
    ret lpCmdLine;
}