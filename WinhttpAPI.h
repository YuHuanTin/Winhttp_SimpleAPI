#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <iostream>
#include <vector>
#include <string>

#include "Windows.h"
#include "winhttp.h"
using namespace std;
#pragma comment(lib,"winhttp.lib")

struct stHttpRequest{
    string Url;
    string Model;
    string Body;//Post��ʽ��
    string Headers;//�Ի���(\r\n)�ָ�
    string Proxy;
};
struct stHttpResponse{
    string Body;
    string Headers;
};

extern __declspec(dllexport) stHttpResponse Winhttp_Request(stHttpRequest *httpRequest);
extern "C" __declspec(dllexport) void fn_EncodingSwitch(std::string &cpSrc,std::string &cpDst, unsigned CP_src, unsigned CP_dst);
extern "C" __declspec(dllexport) void fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst);
extern "C" __declspec(dllexport) void fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst);

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
