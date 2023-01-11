#include "CodeCvt.h"

using std::string;
using std::wstring;
using std::unique_ptr;

string CodeCvt::WstrToStr(const wstring &Src, UINT CodePage) {
    if (Src.empty())
        return "";
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  Src.c_str(),
                                  (int) Src.length(),
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    if (len <= 0)
        return "";
    string szResult(len, 0);
    WideCharToMultiByte(CodePage,
                        NULL,
                        Src.c_str(),
                        (int) Src.length(),
                        &szResult.at(0),
                        len,
                        nullptr,
                        nullptr
    );
    return szResult;
}
wstring CodeCvt::StrToWstr(const string &Src, UINT CodePage) {
    if (Src.empty())
        return L"";
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  Src.c_str(),
                                  (int) Src.length(),
                                  nullptr,
                                  0
    );
    if (len <= 0)
        return L"";
    wstring wszResult(len, 0);
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src.c_str(),
                        (int) Src.length(),
                        &wszResult.at(0),
                        len
    );
    return wszResult;
}
unique_ptr<char[]> CodeCvt::WstrToStr(wchar_t *Src, UINT CodePage) {
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    if (len <= 0)
        return {};
    unique_ptr<char[]> Dst = std::make_unique<char[]>(len * sizeof(char));
    WideCharToMultiByte(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst.get(),
                        len,
                        nullptr,
                        nullptr
    );
    return Dst;
}
unique_ptr<wchar_t[]> CodeCvt::StrToWstr(char *Src, UINT CodePage) {
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0
    );
    if (len <= 0)
        return {};
    unique_ptr<wchar_t[]> Dst = std::make_unique<wchar_t[]>(len * sizeof(wchar_t));
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst.get(),
                        len
    );
    return Dst;
}