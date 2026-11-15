#pragma once

#include <cstddef>
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#include <string>
#include <memory>
#include <vector>
#include <windows.h>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "shell32.lib")

#define ret return

#ifndef CP_UTF16LE
#define CP_UTF16LE 1200
#endif
#ifndef CP_UTF16BE
#define CP_UTF16BE 1201
#endif
#ifndef CP_UTF32LE
#define CP_UTF32LE 12000
#endif
#ifndef CP_UTF32BE
#define CP_UTF32BE 12001
#endif

typedef std::string str;
typedef const std::string cstr;
typedef std::wstring wstr;
typedef const std::wstring cwstr;
typedef char STR;
typedef wchar_t WSTR;
typedef const std::type_info *LPCTYPE;

using namespace std;

inline UINT GetCurrentCodePage();

struct ToSTR {
    struct {
        SIZE_T cbSize = 0;
        SIZE_T StrLen = 0;
        SIZE_T WcsLen = 0;
    } mutable len;
    typedef decltype(len) LEN;

    enum class StrType { LPSTR, LPWSTR };
    enum class Way { FromMultiByte, FromWideChar };

    mutable UINT CP = CP_ACP;

    mutable unique_ptr<STR[]> StoredSTR;
    mutable unique_ptr<WSTR[]> StoredWSTR;
    mutable unique_ptr<STR[]> cachedSTR;
    mutable unique_ptr<WSTR[]> cachedWSTR;

    LPSTR LPWSTR2LPSTR(LPCWSTR lpCWSTR) const;
    LPWSTR LPSTR2LPWSTR(LPCSTR lpCSTR) const;
    LSTATUS StrCopyEx_s(const ToSTR& Src, ToSTR& Target);
    LSTATUS StrCrossCopyEx_s(const ToSTR& Src, ToSTR& Target, Way W);

    LEN& size() const;
    LEN& length() const;
    BOOL empty() const;
    VOID clear();

    BOOL reserve(LPDWORD pcbBuf);
    LSTATUS insertA(SIZE_T pos, const ToSTR& src);
    LSTATUS insertW(SIZE_T pos, const ToSTR& src);
    LSTATUS eraseA(SIZE_T pos, SIZE_T len);
    LSTATUS eraseW(SIZE_T pos, SIZE_T len);
    LSTATUS replaceA(SIZE_T pos, SIZE_T len, const ToSTR& src);
    LSTATUS replaceW(SIZE_T pos, SIZE_T len, const ToSTR& src);

    LPSTR wide2narrow(LPCWSTR wide);
    LPWSTR narrow2wide(LPCSTR narrow);

    template <StrType T> auto GetSTR() const {
        if constexpr (T == StrType::LPSTR) ret StoredSTR.get();
        if constexpr (T == StrType::LPWSTR) ret StoredWSTR.get();
    }

    ToSTR() : StoredSTR(nullptr), StoredWSTR(nullptr) {}
    ToSTR(LPCSTR lpStr) {
        if (lpStr) {
            SIZE_T len = strlen(lpStr) + 1;
            StoredSTR.reset(new STR[len]);
            strcpy_s(StoredSTR.get(), len, lpStr);
        }
    }
    ToSTR(LPCWSTR lpWStr) {
        if (lpWStr) {
            SIZE_T len = wcslen(lpWStr) + 1;
            StoredWSTR.reset(new WSTR[len]);
            wcscpy_s(StoredWSTR.get(), len, lpWStr);
        }
    }
    ToSTR(str Str) {
        if (Str.size()) {
            SIZE_T len = Str.size() + 1;
            StoredSTR.reset(new STR[len]);
            strcpy_s(StoredSTR.get(), len, Str.c_str());
        }
    }
    ToSTR(wstr WStr) {
        if (WStr.size()) {
            SIZE_T len = WStr.size() + 1;
            StoredWSTR.reset(new WSTR[len]);
            wcscpy_s(StoredWSTR.get(), len, WStr.c_str());
        }
    }

    ToSTR(nullptr_t) {
        StoredSTR.reset(nullptr);
        StoredWSTR.reset(nullptr);
        cachedSTR.reset(nullptr);
        cachedWSTR.reset(nullptr);
    }
    /*ToSTR(LPCSTR lpStr = nullptr, LPCWSTR lpWStr = nullptr) {
        if (lpStr) {
            SIZE_T len = strlen(lpStr) + 1;
            StoredSTR.reset(new STR[len]);
            strcpy_s(StoredSTR.get(), len, lpStr);
        }
        if (lpWStr) {
            SIZE_T len = wcslen(lpWStr) + 1;
            StoredWSTR.reset(new WSTR[len]);
            wcscpy_s(StoredWSTR.get(), len, lpWStr);
        }
    }*/
    ToSTR(const ToSTR& other) {
        StrCopyEx_s(other, *this);
    }

    operator LPSTR() const {
        if (StoredSTR);
        else if (StoredWSTR) {
            LPWSTR2LPSTR(StoredWSTR.get());
            StoredSTR = move(cachedSTR);
        }
        ret this->StoredSTR.get();
    }
    /*operator LPSTR() const {
        ret this->StoredSTR.get();
    }*/
    operator LPWSTR() const {
        if (StoredWSTR);
        else if (StoredSTR) {
            LPSTR2LPWSTR(StoredSTR.get());
            StoredWSTR = move(cachedWSTR);
        }
        ret this->StoredWSTR.get();
    }
    /*operator LPWSTR() const {
        ret this->StoredWSTR.get();
    }*/

    operator str() const {
        if (StoredSTR) ret str(StoredSTR.get());
        else if (StoredWSTR) {
            LPSTR temp = LPWSTR2LPSTR(StoredWSTR.get());
            str res = temp ? str(temp) : str();
            ret res;
        }
        ret str();
    }
    /*operator str() const {
        if (StoredSTR) ret str(StoredSTR.get());
        ret str();
    }*/
    operator wstr() const {
        if (StoredWSTR) ret wstr(StoredWSTR.get());
        else if (StoredSTR) {
            LPWSTR temp = LPSTR2LPWSTR(StoredSTR.get());
            wstr res = temp ? wstr(temp) : wstr();
            ret res;
        }
        ret wstr();
    }
    /*operator wstr() const {
        if (StoredWSTR) ret wstr(StoredWSTR.get());
        ret wstr();
    }*/

    operator vector<STR>() const {
        if (StoredSTR) {
            SIZE_T len = strlen(StoredSTR.get());
            ret vector<STR>(StoredSTR.get(), StoredSTR.get() + len);
        } else if (StoredWSTR) {
            unique_ptr<STR[]> temp;
            LPWSTR2LPSTR(StoredWSTR.get());
            temp = move(cachedSTR);
            if (temp) {
                SIZE_T len = strlen(temp.get());
                ret vector<STR>(temp.get(), temp.get() + len);
            }
        }
        ret vector<STR>();
    }
    /*operator vector<STR>() const {
        if (StoredSTR) {
            SIZE_T len = strlen(StoredSTR.get());
            ret vector<STR>(StoredSTR.get(), StoredSTR.get() + len);
        }
        ret vector<STR>();
    }*/
    operator vector<WSTR>() const {
        if (StoredWSTR) {
            SIZE_T len = wcslen(StoredWSTR.get());
            ret vector<WSTR>(StoredWSTR.get(), StoredWSTR.get() + len);
        } else if (StoredSTR) {
            unique_ptr<WSTR[]> temp;
            LPSTR2LPWSTR(StoredSTR.get());
            temp = move(cachedWSTR);
            if (temp) {
                SIZE_T len = wcslen(temp.get());
                ret vector<WSTR>(temp.get(), temp.get() + len);
            }
        }
        ret vector<WSTR>();
    }
    /*operator vector<WSTR>() const {
        if (StoredWSTR) {
            SIZE_T len = wcslen(StoredWSTR.get());
            ret vector<WSTR>(StoredWSTR.get(), StoredWSTR.get() + len);
        }
        ret vector<WSTR>();
    }*/

    ToSTR& operator=(LPCSTR lpCStr) {
        if (lpCStr) {
            SIZE_T len = strlen(lpCStr) + 1;
            StoredSTR.reset(new STR[len]);
            strcpy_s(StoredSTR.get(), len, lpCStr);
        }
        else StoredSTR.reset();
        cachedSTR.reset();
        return *this;
    }
    ToSTR& operator=(LPCWSTR lpCWStr) {
        if (lpCWStr) {
            SIZE_T len = wcslen(lpCWStr) + 1;
            StoredWSTR.reset(new WSTR[len]);
            wcscpy_s(StoredWSTR.get(), len, lpCWStr);
        }
        else StoredWSTR.reset();
        cachedWSTR.reset();
        return *this;
    }
    ToSTR& operator=(const ToSTR &other) {
        if (this == &other);
        else {
            StoredSTR.reset();
            StoredWSTR.reset();
            if (other.StoredSTR) {
                SIZE_T len = strlen(other.StoredSTR.get()) + 1;
                StoredSTR.reset(new STR[len]);
                strcpy_s(StoredSTR.get(), len, other.StoredSTR.get());
            }
            if (other.StoredWSTR) {
                SIZE_T len = wcslen(other.StoredWSTR.get()) + 1;
                StoredWSTR.reset(new WSTR[len]);
                wcscpy_s(StoredWSTR.get(), len, other.StoredWSTR.get());
            }
        }
        return *this;
    }
    ToSTR operator+(const ToSTR &other) const {
        ToSTR result;
        if (this->StoredSTR && other.StoredSTR) {
            SIZE_T len1 = strlen(this->StoredSTR.get());
            SIZE_T len2 = strlen(other.StoredSTR.get());
            SIZE_T totalLen = len1 + len2 + 1;
            result.StoredSTR.reset(new STR[totalLen]);
            strcpy_s(result.StoredSTR.get(), totalLen, this->StoredSTR.get());
            strcat_s(result.StoredSTR.get(), totalLen, other.StoredSTR.get());
        }
        if (this->StoredWSTR && other.StoredWSTR) {
            SIZE_T len1 = wcslen(this->StoredWSTR.get());
            SIZE_T len2 = wcslen(other.StoredWSTR.get());
            SIZE_T totalLen = len1 + len2 + 1;
            result.StoredWSTR.reset(new WSTR[totalLen]);
            wcscpy_s(result.StoredWSTR.get(), totalLen, this->StoredWSTR.get());
            wcscat_s(result.StoredWSTR.get(), totalLen, other.StoredWSTR.get());
        }
        if ((this->StoredSTR && other.StoredWSTR) || (this->StoredWSTR && other.StoredSTR)) {
            std::unique_ptr<STR[]> tempStr1, tempStr2;
            std::unique_ptr<WSTR[]> tempWStr1, tempWStr2;
            if (this->StoredSTR && other.StoredWSTR) {
                if ((ToSTR::LPSTR2LPWSTR(this->StoredSTR.get())) &&
                ToSTR::LPWSTR2LPSTR(other.StoredWSTR.get()));
                else ret result;
                tempStr1 = move(this->cachedSTR);
                tempWStr1 = move(other.cachedWSTR);
                SIZE_T len1 = strlen(this->StoredSTR.get());
                SIZE_T len2 = strlen(tempStr1.get());
                SIZE_T totalLen = len1 + len2 + 1;
                result.StoredSTR.reset(new STR[totalLen]);
                strcpy_s(result.StoredSTR.get(), totalLen, this->StoredSTR.get());
                strcat_s(result.StoredSTR.get(), totalLen, tempStr1.get());
                SIZE_T len3 = wcslen(tempWStr1.get());
                SIZE_T len4 = wcslen(other.StoredWSTR.get());
                SIZE_T totalWLen = len3 + len4 + 1;
                result.StoredWSTR.reset(new WSTR[totalWLen]);
                wcscpy_s(result.StoredWSTR.get(), totalWLen, tempWStr1.get());
                wcscat_s(result.StoredWSTR.get(), totalWLen, other.StoredWSTR.get());
            }
            if (this->StoredWSTR && other.StoredSTR) {
                if ((ToSTR::LPWSTR2LPSTR(this->StoredWSTR.get())) && 
                ToSTR::LPSTR2LPWSTR(other.StoredSTR.get()));
                else ret result;
                tempStr2 = move(other.cachedSTR);
                tempWStr2 = move(this->cachedWSTR);
                SIZE_T len1 = strlen(tempStr2.get());
                SIZE_T len2 = strlen(other.StoredSTR.get());
                SIZE_T totalLen = len1 + len2 + 1;
                result.StoredSTR.reset(new STR[totalLen]);
                strcpy_s(result.StoredSTR.get(), totalLen, tempStr2.get());
                strcat_s(result.StoredSTR.get(), totalLen, other.StoredSTR.get());
                SIZE_T len3 = wcslen(this->StoredWSTR.get());
                SIZE_T len4 = wcslen(tempWStr2.get());
                SIZE_T totalWLen = len3 + len4 + 1;
                result.StoredWSTR.reset(new WSTR[totalWLen]);
                wcscpy_s(result.StoredWSTR.get(), totalWLen, this->StoredWSTR.get());
                wcscat_s(result.StoredWSTR.get(), totalWLen, tempWStr2.get());
            }
        }
        ret result;
    }
    ToSTR& operator+=(const ToSTR other) {
        if (other.empty()) ret *this;
        *this = *this + other;
        ret *this;
    }

    BOOL operator==(const ToSTR& other) const {
        if (this->StoredSTR && other.StoredSTR) return strcmp(this->StoredSTR.get(), other.StoredSTR.get()) == 0;
        if (this->StoredWSTR && other.StoredWSTR) return wcscmp(this->StoredWSTR.get(), other.StoredWSTR.get()) == 0;
        ret FALSE;
    }
    BOOL operator!=(const ToSTR& other) const {
         return !(*this == other);
    }
    ToSTR(ToSTR&& other) noexcept = default;
    ToSTR& operator=(ToSTR&& other) noexcept = default;
};

typedef class ToSTR MultiSTR, DualSTR_t, STRExt, WSTRExt;
typedef const class ToSTR MultiCSTR, DualCSTR_t, CSTRExt, CWSTRExt;

inline BOOL ReadFileToBuf(LPCSTR path, std::unique_ptr<BYTE[]>& buf, DWORD& size) {
    buf.reset();
    size = 0;
    LPCSTR p = path;
    if (p); else ret FALSE;
    HANDLE h = CreateFileA(
        p, GENERIC_READ, FILE_SHARE_READ,
        nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if (h == INVALID_HANDLE_VALUE) ret FALSE;
    DWORD sz = GetFileSize(h, nullptr);
    if (sz == INVALID_FILE_SIZE || sz == 0) {
        CloseHandle(h);
        ret FALSE;
    }
    buf.reset(new BYTE[sz + 4]());
    ReadFile(h, buf.get(), sz, &size, nullptr);
    CloseHandle(h);
    buf[size] = 0;
    ret TRUE;
}

class encoding;
struct encoding {
    mutable str path;
    encoding(cstr& p = "") : path(p) {} ;
    encoding(LPCSTR p = "") : path(p ? p : "") {} ;
    BOOL VerifyUTF8(const BYTE* buf, DWORD size) const;
    BOOL VerifyUTF8BOM(const BYTE* buf, DWORD size) const;
    BOOL VerifyUTF16LE(const BYTE* buf, DWORD size) const;
    BOOL VerifyUTF16BE(const BYTE* buf, DWORD size) const;
    BOOL VerifyUTF32LE(const BYTE* buf, DWORD size) const;
    BOOL VerifyUTF32BE(const BYTE* buf, DWORD size) const;
    BOOL VerifyANSI(const BYTE* buf, DWORD size) const;
    UINT DetectCodePage() const;
    wstr ReadText() const;
};
inline UINT GetCurrentCodePage(LPCSTR lpFile /* 该参数应为__FILE__ */) {
    static UINT cached = [lpFile] {
        encoding enc = lpFile;
        return enc.DetectCodePage();
    } ();
    ret cached;
}

typedef ToSTR::LEN STRLEN;
inline LPWSTR ToSTR::LPSTR2LPWSTR(LPCSTR lpCSTR) const {
    if (lpCSTR); else ret nullptr;
    int len = MultiByteToWideChar(CP, 0, lpCSTR, -1, nullptr, 0);
    if (len <= 0) ret nullptr;
    this->cachedWSTR.reset(new WSTR[len]);
    // LPWSTR result = new WSTR[len];
    MultiByteToWideChar(CP, 0, lpCSTR, -1,cachedWSTR.get(), len);
    ret cachedWSTR.get();
}
inline LPSTR ToSTR::LPWSTR2LPSTR(LPCWSTR lpCWSTR) const {
    if (lpCWSTR); else ret nullptr;
    int len = WideCharToMultiByte(CP, 0, lpCWSTR, -1, nullptr, 0, nullptr, nullptr);
    if (len <= 0) ret nullptr;
    this->cachedSTR.reset(new STR[len]);
    // LPSTR result = new STR[len];
    WideCharToMultiByte(CP, 0, lpCWSTR, -1, cachedSTR.get(), len, nullptr, nullptr);
    ret cachedSTR.get();
}
inline LSTATUS ToSTR::StrCopyEx_s(DualCSTR_t& Src, DualSTR_t& Target) {
    if (&Src == &Target) ret ERROR_SUCCESS;
    if (Src.StoredSTR) {
        SIZE_T len = strlen(Src.StoredSTR.get()) + 1;
        Target.StoredSTR.reset(new STR[len]);
        strcpy_s(Target.StoredSTR.get(), len, Src.StoredSTR.get());
    } else Target.StoredSTR.reset();
    if (Src.StoredWSTR) {
        SIZE_T len = wcslen(Src.StoredWSTR.get()) + 1;
        Target.StoredWSTR.reset(new WSTR[len]);
        wcscpy_s(Target.StoredWSTR.get(), len, Src.StoredWSTR.get());
    } else Target.StoredWSTR.reset();
    Target.cachedSTR.reset();
    Target.cachedWSTR.reset();
    ret ERROR_SUCCESS;
}
inline LSTATUS ToSTR::StrCrossCopyEx_s(DualCSTR_t& Src, DualSTR_t& Target, Way W) {
    if (W == Way::FromMultiByte && Src.StoredSTR) {
        SIZE_T len = MultiByteToWideChar(Src.CP, 0, Src.StoredSTR.get(), -1, nullptr, 0);
        if (len <= 0) ret ERROR_INVALID_PARAMETER;
        Target.cachedWSTR.reset(new WSTR[len]());
        MultiByteToWideChar(Src.CP, 0, Src.StoredSTR.get(), -1, Target.cachedWSTR.get(), len);
        Target.StoredWSTR = move(Target.cachedWSTR);
        if (&Src != &Target) Target.StoredSTR.reset();
        Target.cachedSTR.reset();
        ret ERROR_SUCCESS;
    } else
    if (W == Way::FromWideChar && Src.StoredWSTR) {
        SIZE_T len = WideCharToMultiByte(Src.CP, 0, Src.StoredWSTR.get(), -1, nullptr, 0, nullptr, nullptr);
        if (len <= 0) ret ERROR_INVALID_PARAMETER;
        Target.cachedSTR.reset(new STR[len]());
        WideCharToMultiByte(Src.CP, 0, Src.StoredWSTR.get(), -1, Target.cachedSTR.get(), len, nullptr, nullptr);
        Target.StoredSTR = move(Target.cachedSTR);
        if (&Src != &Target) Target.StoredWSTR.reset();
        Target.cachedWSTR.reset();
        ret ERROR_SUCCESS;
    } else ret ERROR_INVALID_PARAMETER;
}
inline STRLEN& ToSTR::size() const {
    this->len.StrLen = this->StoredSTR ? strlen(this->StoredSTR.get()) : 0;
    this->len.WcsLen = this->StoredWSTR ? wcslen(this->StoredWSTR.get()) : 0;
    ret this->len;
}
inline STRLEN& ToSTR::length() const {
    ret this->size();
}
inline BOOL ToSTR::empty() const {
    STRLEN len = this->size();
    ret len.StrLen == 0 && len.WcsLen == 0;
}
inline VOID ToSTR::clear() {
    this->StoredSTR.reset(nullptr);
    this->StoredWSTR.reset(nullptr);
}
inline BOOL ToSTR::reserve(LPDWORD size) {
    if (size == nullptr) ret FALSE;
    this->StoredSTR.reset(new STR[*size + 1]());
    this->StoredWSTR.reset(new WSTR[*size + 1]());
    ret TRUE;
}
inline LSTATUS ToSTR::insertA(SIZE_T pos, const ToSTR& src) {
    if (this->StoredSTR.get() == nullptr) {
        if (this->StoredWSTR.get() == nullptr) ret ERROR_INVALID_PARAMETER;
        else this->StrCrossCopyEx_s(*this, *this, Way::FromWideChar);
    };
    LPCSTR Src = (LPCSTR)src;
    if (Src == nullptr) ret ERROR_SUCCESS; // Do nothing, This pointer is original.
    SIZE_T lenThis = strlen(this->StoredSTR.get());
    SIZE_T lenSrc = strlen(Src);
    if (pos > lenThis) ret ERROR_INVALID_PARAMETER;
    vector<STR> temp;
    temp.reserve(lenThis + lenSrc + 1);
    for (int idx = 0; idx < pos; ++idx) temp.push_back(*(this->StoredSTR.get() + idx));
    for (int idx = 0; idx < lenSrc; ++idx) temp.push_back(*(src.StoredSTR.get() + idx));
    for (int idx = pos; idx < lenThis; ++idx) temp.push_back(*(this->StoredSTR.get() + idx));
    temp.push_back('\0');
    this->StoredSTR.reset(new STR[temp.size()]);
    strcpy_s(this->StoredSTR.get(), temp.size(), temp.data());
    this->StoredWSTR.reset(nullptr);
    ret ERROR_SUCCESS;
}
inline LSTATUS ToSTR::insertW(SIZE_T pos, const ToSTR& src) {
    if (this->StoredWSTR.get() == nullptr) {
        if (this->StoredSTR.get() == nullptr) ret ERROR_INVALID_PARAMETER;
        else this->StrCrossCopyEx_s(*this, *this, Way::FromMultiByte);
    };
    LPCWSTR Src = (LPCWSTR)src;
    if (Src == nullptr) ret ERROR_SUCCESS; // Do nothing, This pointer is original.
    SIZE_T lenThis = wcslen(this->StoredWSTR.get());
    SIZE_T lenSrc = wcslen(Src);
    if (pos > lenThis) ret ERROR_INVALID_PARAMETER;
    unique_ptr<WSTR[]> temp(new WSTR[lenThis + lenSrc + 1]);
    for (int idx = 0; idx < pos; ++idx) *(temp.get() + idx) = *(this->StoredWSTR.get() + idx);
    for (int idx = 0; idx < lenSrc; ++idx) *(temp.get() + idx + pos) = *(src.StoredWSTR.get() + idx);
    for (int idx = pos; idx < lenThis; ++idx) *(temp.get() + lenSrc + idx) = *(this->StoredWSTR.get() + idx);
    *(temp.get() + lenThis + lenSrc) = L'\0';
    this->StoredWSTR.reset(temp.release());
    this->StoredSTR.reset(nullptr);
    ret ERROR_SUCCESS;
}
inline LSTATUS ToSTR::eraseA(SIZE_T pos, SIZE_T len) {
    if (this->StoredSTR.get() == nullptr) {
        if (this->StoredWSTR.get() == nullptr) return ERROR_INVALID_PARAMETER;
        else this->StrCrossCopyEx_s(*this, *this, Way::FromWideChar);
    }
    LPCSTR cur = this->StoredSTR.get();
    SIZE_T curLen = strlen(cur);
    if (pos > curLen) return ERROR_INVALID_PARAMETER;
    if (pos + len > curLen) len = curLen - pos;
    if (len == 0) return ERROR_SUCCESS;
    std::vector<STR> temp;
    temp.reserve(curLen - len + 1);
    for (SIZE_T i = 0; i < pos; ++i) temp.push_back(cur[i]);
    for (SIZE_T i = pos + len; i < curLen; ++i) temp.push_back(cur[i]);
    temp.push_back('\0');
    this->StoredSTR.reset(new STR[temp.size()]);
    strcpy_s(this->StoredSTR.get(), temp.size(), temp.data());
    this->StoredWSTR.reset(nullptr);
    return ERROR_SUCCESS;
}
inline LSTATUS ToSTR::eraseW(SIZE_T pos, SIZE_T len) {
    if (this->StoredWSTR.get() == nullptr) {
        if (this->StoredSTR.get() == nullptr) return ERROR_INVALID_PARAMETER;
        else this->StrCrossCopyEx_s(*this, *this, Way::FromMultiByte);
    }
    LPCWSTR cur = this->StoredWSTR.get();
    SIZE_T curLen = wcslen(cur);
    if (pos > curLen) return ERROR_INVALID_PARAMETER;
    if (pos + len > curLen) len = curLen - pos;
    if (len == 0) return ERROR_SUCCESS;
    std::unique_ptr<WSTR[]> temp(new WSTR[curLen - len + 1]);
    for (SIZE_T i = 0; i < pos; ++i) temp.get()[i] = cur[i];
    for (SIZE_T i = pos + len; i < curLen; ++i) temp.get()[i - len] = cur[i];
    temp.get()[curLen - len] = L'\0';
    this->StoredWSTR.reset(temp.release());
    this->StoredSTR.reset(nullptr);
    return ERROR_SUCCESS;
}

inline LSTATUS ToSTR::replaceA(SIZE_T pos, SIZE_T len, const ToSTR& src) {
    if (this->StoredSTR.get() == nullptr) {
        if (this->StoredWSTR.get() == nullptr) return ERROR_INVALID_PARAMETER;
        else this->StrCrossCopyEx_s(*this, *this, Way::FromWideChar);
    }
    LPCSTR cur = this->StoredSTR.get();
    LPCSTR Src = (LPCSTR)src;
    if (Src == nullptr) return ERROR_INVALID_PARAMETER;
    SIZE_T curLen = strlen(cur);
    SIZE_T srcLen = strlen(Src);
    if (pos > curLen) return ERROR_INVALID_PARAMETER;
    if (pos + len > curLen) len = curLen - pos;
    std::vector<STR> temp;
    temp.reserve(curLen - len + srcLen + 1);
    for (SIZE_T i = 0; i < pos; ++i) temp.push_back(cur[i]);
    for (SIZE_T i = 0; i < srcLen; ++i) temp.push_back(Src[i]);
    for (SIZE_T i = pos + len; i < curLen; ++i) temp.push_back(cur[i]);
    temp.push_back('\0');
    this->StoredSTR.reset(new STR[temp.size()]);
    strcpy_s(this->StoredSTR.get(), temp.size(), temp.data());
    this->StoredWSTR.reset(nullptr);
    return ERROR_SUCCESS;
}

inline LSTATUS ToSTR::replaceW(SIZE_T pos, SIZE_T len, const ToSTR& src) {
    if (this->StoredWSTR.get() == nullptr) {
        if (this->StoredSTR.get() == nullptr) return ERROR_INVALID_PARAMETER;
        else this->StrCrossCopyEx_s(*this, *this, Way::FromMultiByte);
    }
    LPCWSTR cur = this->StoredWSTR.get();
    LPCWSTR Src = (LPCWSTR)src;
    if (Src == nullptr) return ERROR_INVALID_PARAMETER;
    SIZE_T curLen = wcslen(cur);
    SIZE_T srcLen = wcslen(Src);
    if (pos > curLen) return ERROR_INVALID_PARAMETER;
    if (pos + len > curLen) len = curLen - pos;
    std::unique_ptr<WSTR[]> temp(new WSTR[curLen - len + srcLen + 1]);
    for (SIZE_T i = 0; i < pos; ++i) temp.get()[i] = cur[i];
    for (SIZE_T i = 0; i < srcLen; ++i) temp.get()[pos + i] = Src[i];
    for (SIZE_T i = pos + len; i < curLen; ++i) temp.get()[i - len + srcLen] = cur[i];
    temp.get()[curLen - len + srcLen] = L'\0';
    this->StoredWSTR.reset(temp.release());
    this->StoredSTR.reset(nullptr);
    return ERROR_SUCCESS;
}

inline BOOL operator==(DualCSTR_t& lhs, LPCSTR rhs) {
    if (rhs == nullptr) return lhs.StoredSTR == nullptr && lhs.StoredWSTR == nullptr;
    if (lhs.StoredSTR == nullptr) return FALSE;
    ret strcmp(lhs.StoredSTR.get(), rhs) == 0;
}
inline BOOL operator==(LPCSTR lhs, DualCSTR_t& rhs) {
    ret rhs == lhs;
}
inline BOOL operator==(DualCSTR_t& lhs, LPCWSTR rhs) {
    if (rhs == nullptr) return lhs.StoredSTR == nullptr && lhs.StoredWSTR == nullptr;
    if (lhs.StoredWSTR == nullptr) return FALSE;
    ret wcscmp(lhs.StoredWSTR.get(), rhs) == 0;
}
inline BOOL operator==(LPCWSTR lhs, DualCSTR_t& rhs) {
    ret rhs == lhs;
}
inline BOOL operator==(str lhs, DualCSTR_t& rhs) {
    ret rhs == lhs.c_str();
}
inline BOOL operator==(DualCSTR_t& lhs, str rhs) {
    ret lhs == rhs.c_str();
}
inline BOOL operator==(wstr lhs, DualCSTR_t& rhs) {
    ret rhs == lhs.c_str();
}
inline BOOL operator==(DualCSTR_t& lhs, wstr rhs) {
    ret lhs == rhs.c_str();
}
inline BOOL operator!=(DualCSTR_t& lhs, LPCSTR rhs) {
    ret (lhs == rhs) == FALSE;
}
inline BOOL operator!=(LPCSTR lhs, DualCSTR_t& rhs) {
    ret (rhs == lhs) == FALSE;
}
inline BOOL operator!=(DualCSTR_t& lhs, LPCWSTR rhs) {
    ret (lhs == rhs) == FALSE;
}
inline BOOL operator!=(LPCWSTR lhs, DualCSTR_t& rhs) {
    ret (rhs == lhs) == FALSE;
}
inline BOOL operator!=(str lhs, DualCSTR_t& rhs) {
    ret (rhs == lhs.c_str()) == FALSE;
}
inline BOOL operator!=(DualCSTR_t& lhs, str rhs) {
    ret (lhs == rhs.c_str()) == FALSE;
}
inline BOOL operator!=(wstr lhs, DualCSTR_t& rhs) {
    ret (rhs == lhs.c_str()) == FALSE;
}
inline BOOL operator!=(DualCSTR_t& lhs, wstr rhs) {
    ret (lhs == rhs.c_str()) == FALSE;
}

inline BOOL encoding::VerifyUTF8(const BYTE* buf, DWORD size) const {
    DWORD i = 0;
    while (i < size) {
        BYTE b = buf[i];
        DWORD n = 0;
        if (b < 0x80) n = 0;
        else if ((b & 0xE0) == 0xC0) n = 1;
        else if ((b & 0xF0) == 0xE0) n = 2;
        else if ((b & 0xF8) == 0xF0) n = 3;
        else ret FALSE;
        if (i + n >= size) ret FALSE;
        for (DWORD j = 1; j <= n; ++j) {
            if ((buf[i + j] & 0xC0) != 0x80) ret FALSE;
        }
        i += n + 1;
    }
    ret TRUE;
}
inline BOOL encoding::VerifyUTF8BOM(const BYTE* buf, DWORD size) const {
    if (size >= 3 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) ret TRUE;
    ret FALSE;
}
inline BOOL encoding::VerifyUTF16LE(const BYTE* buf, DWORD size) const {
    if (size >= 4 && buf[0] == 0xFF && buf[1] == 0xFE && buf[2] == 0x00 && buf[3] == 0x00) ret FALSE;
    if (size >= 2 && buf[0] == 0xFF && buf[1] == 0xFE) ret TRUE;
    ret FALSE;
}
inline BOOL encoding::VerifyUTF16BE(const BYTE* buf, DWORD size) const {
    if (size >= 2 && buf[0] == 0xFE && buf[1] == 0xFF) ret TRUE;
    ret FALSE;
}
inline BOOL encoding::VerifyUTF32LE(const BYTE* buf, DWORD size) const {
    if (size >= 4 && buf[0] == 0xFF && buf[1] == 0xFE && buf[2] == 0x00 && buf[3] == 0x00) ret TRUE;
    ret FALSE;
}
inline BOOL encoding::VerifyUTF32BE(const BYTE* buf, DWORD size) const {
    if (size >= 4 && buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0xFE && buf[3] == 0xFF) ret TRUE;
    ret FALSE;
}
inline BOOL encoding::VerifyANSI(const BYTE* buf, DWORD size) const {
    if (VerifyUTF32LE(buf, size)) ret FALSE;
    if (VerifyUTF32BE(buf, size)) ret FALSE;
    if (VerifyUTF16LE(buf, size)) ret FALSE;
    if (VerifyUTF16BE(buf, size)) ret FALSE;
    if (VerifyUTF8BOM(buf, size)) ret FALSE;
    if (VerifyUTF8(buf, size)) ret FALSE;
    ret TRUE;
}
inline UINT encoding::DetectCodePage() const {
    std::unique_ptr<BYTE[]> buf;
    DWORD size = 0;
    if (ReadFileToBuf(path.c_str(), buf, size)); else ret CP_ACP;
    if (VerifyUTF32LE(buf.get(), size)) ret CP_UTF32LE;
    if (VerifyUTF32BE(buf.get(), size)) ret CP_UTF32BE;
    if (VerifyUTF16LE(buf.get(), size)) ret CP_UTF16LE;
    if (VerifyUTF16BE(buf.get(), size)) ret CP_UTF16BE;
    if (VerifyUTF8BOM(buf.get(), size)) ret CP_UTF8;
    if (VerifyUTF8(buf.get(), size)) ret CP_UTF8;
    if (VerifyANSI(buf.get(), size)) ret CP_ACP;
    ret CP_ACP;
}
inline MultiSTR CodePageName(UINT cp) {
    switch (cp) {
        case CP_ACP:     ret "CP_ACP";
        case CP_UTF8:    ret "CP_UTF8";
        case CP_UTF16LE: ret "CP_UTF16LE";
        case CP_UTF16BE: ret "CP_UTF16BE";
        case CP_UTF32LE: ret "CP_UTF32LE";
        case CP_UTF32BE: ret "CP_UTF32BE";
        default:         ret "Unknown";
    }
}
inline wstr encoding::ReadText() const {
    wstr result;
    std::unique_ptr<BYTE[]> buf;
    DWORD size = 0;
    if (ReadFileToBuf(path.c_str(), buf, size)); else ret result;
    UINT cp = this->DetectCodePage();
    BYTE* data = buf.get();
    DWORD dataLen = size;
    if (VerifyUTF32LE(data, dataLen) || VerifyUTF32BE(data, dataLen)) {
        data += 4;
        dataLen -= 4;
    }
    else if (VerifyUTF16LE(data, dataLen) || VerifyUTF16BE(data, dataLen)) {
        data += 2;
        dataLen -= 2;
    }
    else if (VerifyUTF8BOM(data, dataLen)) {
        data += 3;
        dataLen -= 3;
    }
    if (cp == CP_UTF16LE) {
        DWORD wcount = dataLen / sizeof(WSTR);
        std::unique_ptr<WSTR[]> wbuf(new WSTR[wcount + 1]);
        memcpy(wbuf.get(), data, wcount * sizeof(WSTR));
        wbuf[wcount] = 0;
        result = wbuf.get();
    }
    else if (cp == CP_UTF16BE) {
        DWORD wcount = dataLen / sizeof(WSTR);
        std::unique_ptr<WSTR[]> wbuf(new WSTR[wcount + 1]);
        for (DWORD i = 0; i < wcount; ++i) {
            wbuf[i] = (WSTR)((data[i * 2] << 8) | data[i * 2 + 1]);
        }
        wbuf[wcount] = 0;
        result = wbuf.get();
    }
    else if (cp == CP_UTF32LE || cp == CP_UTF32BE) {
        DWORD count = dataLen / 4;
        std::unique_ptr<WSTR[]> wbuf(new WSTR[count + 1]);
        for (DWORD i = 0; i < count; ++i) {
            DWORD cp32;
            if (cp == CP_UTF32LE) cp32 = data[i*4] | (data[i*4+1]<<8) | (data[i*4+2]<<16) | (data[i*4+3]<<24);
            else cp32 = (data[i*4]<<24) | (data[i*4+1]<<16) | (data[i*4+2]<<8) | data[i*4+3];
            if (cp32 <= 0xFFFF) wbuf[i] = (WSTR)cp32;
            else wbuf[i] = L'?';
        }
        wbuf[count] = 0;
        result = wbuf.get();
    }
    else {
        UINT useCp = (cp == CP_ACP) ? CP_ACP : cp;
        int wlen = MultiByteToWideChar(useCp, 0, (LPCSTR)data, (int)dataLen, nullptr, 0);
        if (wlen > 0) {
            std::unique_ptr<WSTR[]> wbuf(new WSTR[wlen + 1]);
            MultiByteToWideChar(useCp, 0, (LPCSTR)data, (int)dataLen, wbuf.get(), wlen);
            wbuf[wlen] = 0;
            result = wbuf.get();
        }
    }
    ret result;
}