#ifndef WINHTTP_SIMPLEAPI_CODECVT_H
#define WINHTTP_SIMPLEAPI_CODECVT_H

#include <string>
#include "windows.h"

extern std::wstring CodeCvt_StrToWStr(const std::string &Src, UINT CodePage);
extern std::string CodeCvt_WStrToStr(const std::wstring &Src, UINT CodePage);
extern void CodeCvt_WcharToChar(wchar_t *pcwSrc, char *&pcDst, UINT CodePage);
extern void CodeCvt_CharToWchar(char *pcSrc, wchar_t *&pcwDst, UINT CodePage);

#endif //WINHTTP_SIMPLEAPI_CODECVT_H
