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
    string result(len, 0);
    WideCharToMultiByte(CodePage,
                        NULL,
                        Src.c_str(),
                        (int) Src.length(),
                        &result.at(0),
                        len,
                        nullptr,
                        nullptr
    );
    return result;
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
    wstring result(len, 0);
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src.c_str(),
                        (int) Src.length(),
                        &result.at(0),
                        len
    );
    return result;
}
void CodeCvt_WcharToChar_Unique_Ptr(wchar_t *Src, std::unique_ptr<char[]> &Dst, UINT CodePage = CP_ACP) {
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    Dst.reset((char *) malloc(len * sizeof(char)));
    WideCharToMultiByte(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst.get(),
                        len,
                        nullptr,
                        nullptr
    );
}
void CodeCvt_CharToWchar_Unique_Ptr(char *Src, std::unique_ptr<wchar_t[]> &Dst, UINT CodePage = CP_ACP) {
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0
    );
    Dst.reset((wchar_t *) malloc(len * sizeof(wchar_t)));
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst.get(),
                        len
    );
}