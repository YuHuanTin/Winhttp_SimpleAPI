#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <iostream>
#include <vector>
#include "Windows.h"
#include "winhttp.h"
using namespace std;
#pragma comment(lib,"winhttp.lib")

extern "C" __declspec(dllexport) char *Winhttp_Request(char *inUrl,char *inModel, char *inBody, char *inHandles, char *inCookies, char *inProxy, unsigned uTimeout);

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
