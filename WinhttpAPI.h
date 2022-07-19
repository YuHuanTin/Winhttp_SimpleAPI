#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <iostream>
#include "Windows.h"
#include "winhttp.h"
using namespace std;
#pragma comment(lib,"winhttp.lib")
extern char *g_szRetHandles;//optional return data
extern "C" __declspec(dllexport) char *Winhttp_Request(char *inUrl,char *inModel, char *inBody, char *inHandles, char *inCookies, char *inProxy, unsigned uTimeout);
extern "C" __declspec(dllexport) void fn_EncodingSwitch(std::string &cpSrc,std::string &cpDst, unsigned CP_src, unsigned CP_dst);
extern "C" __declspec(dllexport) void fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst);
extern "C" __declspec(dllexport) void fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst);
#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
