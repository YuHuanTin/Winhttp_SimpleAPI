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
void CodeCvt_CharToWchar(char *pcSrc, wchar_t *&pcwDst, UINT CodePage = CP_ACP){
    pcwDst = nullptr;
    int len = MultiByteToWideChar(CodePage,
                                  NULL,
                                  pcSrc,
                                  -1,
                                  nullptr,
                                  0
    );
    pcwDst = (wchar_t *)realloc(pcwDst, len * sizeof(wchar_t));
    MultiByteToWideChar(CodePage,
                        NULL,
                        pcSrc,
                        -1,
                        pcwDst,
                        len
    );
}
void CodeCvt_WcharToChar(wchar_t *pcwSrc, char *&pcDst, UINT CodePage = CP_ACP){
    pcDst = nullptr;
    int len = WideCharToMultiByte(CodePage,
                                  NULL,
                                  pcwSrc,
                                  -1,
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    pcDst = (char *)realloc(pcDst, len * sizeof(char));
    WideCharToMultiByte(CodePage,
                        NULL,
                        pcwSrc,
                        -1,
                        pcDst,
                        len,
                        nullptr,
                        nullptr
    );
}
