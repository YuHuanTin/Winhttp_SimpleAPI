#ifndef WINHTTP_SIMPLEAPI_CODECVT_H
#define WINHTTP_SIMPLEAPI_CODECVT_H

#include <string>
#include <memory>
#include "windows.h"

extern std::string CodeCvt_WStrToStr(const std::wstring &Src, UINT CodePage);
extern std::wstring CodeCvt_StrToWStr(const std::string &Src, UINT CodePage);
extern std::unique_ptr<char[]> CodeCvt_WcharToChar_Unique_Ptr(wchar_t *Src, UINT CodePage);
extern std::unique_ptr<wchar_t[]> CodeCvt_CharToWchar_Unique_Ptr(char *Src, UINT CodePage);

#endif //WINHTTP_SIMPLEAPI_CODECVT_H