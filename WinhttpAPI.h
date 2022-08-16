#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#include <vector>
#include <string>
#include <map>
#include "Windows.h"
#include "winhttp.h"
#pragma comment(lib,"winhttp.lib")

struct stHttpRequest{
    std::string Url;
    std::string Model;
    std::string Body;//Post data
    std::string Proxy;
    long TimeOut;
    std::map<std::string,std::string> Headers;
};
struct stHttpResponse{
    std::string Body;
    std::map<std::string,std::string> Headers;
    std::string allHeaders;
};
class cWinHttpSimpleAPI{
public:
    static bool Winhttp_SetHeaders(stHttpRequest &httpRequest,const std::string &key,const std::string &value);
    //return empty str if key can't find
    static std::string Winhttp_GetHeaders(stHttpResponse &httpResponse,const std::string &key);

    static int Winhttp_Request(stHttpRequest &httpRequest, stHttpResponse &httpResponse);
    static void fn_EncodingSwitch(std::string &cpSrc, std::string &cpDst, unsigned CP_src, unsigned CP_dst);
    static void fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst);
    static void fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst);
};
#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
