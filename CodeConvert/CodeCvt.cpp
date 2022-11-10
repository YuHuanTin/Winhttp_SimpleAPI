#include "CodeCvt.h"

using std::string;
using std::wstring;

string CodeCvt_WStrToStr(const wstring &Src, UINT CodePage = CP_ACP) {
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
wstring CodeCvt_StrToWStr(const string &Src, UINT CodePage = CP_ACP) {
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
std::unique_ptr<char[]> CodeCvt_WcharToChar_Unique_Ptr(wchar_t *Src, UINT CodePage = CP_ACP){
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    std::unique_ptr<char[]> Dst(new char [len * sizeof(char)]);
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
std::unique_ptr<wchar_t[]> CodeCvt_CharToWchar_Unique_Ptr(char *Src, UINT CodePage = CP_ACP){
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0
    );
    std::unique_ptr<wchar_t[]> Dst(new wchar_t [len * sizeof(wchar_t)]);
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst.get(),
                        len
    );
    return Dst;
}