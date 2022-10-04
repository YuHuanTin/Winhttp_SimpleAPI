#include "CodeCvt.h"

using std::string;
using std::wstring;

wstring CodeCvt_StrToWStr(const string &Src, UINT CodePage = CP_ACP){
    if (Src.empty())
        return L"";
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  Src.c_str(),
                                  (int)Src.length(),
                                  nullptr,
                                  0
    );
    if (len <= 0)
        return L"";
    wstring result(len,0);
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src.c_str(),
                        (int)Src.length(),
                        &result.at(0),
                        len
    );
    return result;
}
string CodeCvt_WStrToStr(const wstring &Src, UINT CodePage = CP_ACP){
    if (Src.empty())
        return "";
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  Src.c_str(),
                                  (int)Src.length(),
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    if (len <= 0)
        return "";
    string result(len,0);
    WideCharToMultiByte(CodePage,
                        NULL,
                        Src.c_str(),
                        (int)Src.length(),
                        &result.at(0),
                        len,
                        nullptr,
                        nullptr
    );
    return result;
}
void CodeCvt_CharToWchar(char *Src, wchar_t *&Dst, UINT CodePage = CP_ACP){
    Dst = nullptr;
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0
    );
    Dst = (wchar_t *)realloc(Dst, len * sizeof(wchar_t));
    MultiByteToWideChar(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst,
                        len
    );
}
void CodeCvt_WcharToChar(wchar_t *Src, char *&Dst, UINT CodePage = CP_ACP){
    Dst = nullptr;
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  Src,
                                  -1,
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    Dst = (char *)realloc(Dst, len * sizeof(char));
    WideCharToMultiByte(CodePage,
                        NULL,
                        Src,
                        -1,
                        Dst,
                        len,
                        nullptr,
                        nullptr
    );
}
