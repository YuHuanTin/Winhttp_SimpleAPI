#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <iostream>
#include "Windows.h"
#include "winhttp.h"
using namespace std;
#pragma comment(lib,"winhttp.lib")
extern "C" __declspec(dllexport) char *Winhttp_Request(char *inUrl,char *inModel, char *inBody, char *inHandles, char *inCookies, char *inProxy, unsigned uTimeout);
extern "C" __declspec(dllexport) char *fn_encoding(char *cpData,unsigned CodePage_before,unsigned CodePage_after);
extern "C" __declspec(dllexport) wchar_t *fn_mbstowcs(char *cpStr);
extern "C" __declspec(dllexport) char *fn_wcstombs(wchar_t *wcpStr);
#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
