#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <iostream>
#include <vector>
#include "Windows.h"
#include "winhttp.h"
using namespace std;
#pragma comment(lib,"winhttp.lib")

extern "C" __declspec(dllexport) char *Winhttp_Request(char *szUrl,char *szModel,char *szBody,vector<char *>szvHandles,char *szCookies,char *szProxy,unsigned uTimeout);

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
