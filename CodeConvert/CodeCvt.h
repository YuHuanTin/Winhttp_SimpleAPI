#ifndef WINHTTP_SIMPLEAPI_CODECVT_H
#define WINHTTP_SIMPLEAPI_CODECVT_H

#include <string>
#include <memory>
#include "Windows.h"

class CodeCvt {
public:
    //used for string
    static std::string WstrToStr(const std::wstring &Src, UINT CodePage = CP_ACP);
    static std::wstring StrToWstr(const std::string &Src, UINT CodePage = CP_ACP);

    //used for char *
    static std::unique_ptr<char[]> WstrToStr(wchar_t *Src, UINT CodePage = CP_ACP);
    static std::unique_ptr<wchar_t[]> StrToWstr(char *Src, UINT CodePage = CP_ACP);
};


#endif //WINHTTP_SIMPLEAPI_CODECVT_H