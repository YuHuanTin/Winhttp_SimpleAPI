#ifndef WINHTTP_SIMPLEAPI_CODECVT_H
#define WINHTTP_SIMPLEAPI_CODECVT_H

#include <string>
#include <memory>
#include "windows.h"

extern std::string CodeCvt_WStrToStr(const std::wstring &Src, UINT CodePage);
extern std::wstring CodeCvt_StrToWStr(const std::string &Src, UINT CodePage);
extern void CodeCvt_WcharToChar_Unique_Ptr(wchar_t *Src, std::unique_ptr<char[]> &Dst, UINT CodePage);
extern void CodeCvt_CharToWchar_Unique_Ptr(char *Src, std::unique_ptr<wchar_t[]> &Dst, UINT CodePage);

#endif //WINHTTP_SIMPLEAPI_CODECVT_H