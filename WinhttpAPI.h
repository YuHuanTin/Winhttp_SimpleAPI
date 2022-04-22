//
// Created by 23945 on 2022/4/22.
//

#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <iostream>
#include <vector>
#include "Windows.h"
#include "winhttp.h"
using namespace std;
#pragma comment(lib,"winhttp.lib")

extern "C" __declspec(dllexport) char *Winhttp_Get(char *szUrl,vector<char *>szHandles,char *szCookies,char *szProxy,unsigned uTimeout);
extern "C" __declspec(dllexport) char *Winhttp_Post();


#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
