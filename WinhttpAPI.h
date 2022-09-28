#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#include <string>
#include <map>
#include "windows.h"

struct stTimeOut{
    int ResolveTimeout = 3000;
    int ConnectTimeout = 3000;
    int SendTimeout = 3000;
    int ReceiveTimeout = 3000;
};
struct stHttpRequest{
    std::string Url;
    std::string Model;
    std::string Body;
    std::string Proxy;
    std::string ProxyBypass;
    stTimeOut TimeOut;
};
struct stHttpResponse{
    std::string Body;
    std::string Headers;
};
class cWinhttpAPI{
private:
    std::map<std::string,std::string> Headers;
public:
    cWinhttpAPI() = default;
    cWinhttpAPI(stHttpRequest &httpRequest, stHttpResponse &httpResponse){
        Request(httpRequest, httpResponse);
    }
    int Request(stHttpRequest &httpRequest, stHttpResponse &httpResponse);
    bool SetHeaders(const std::string &Key, const std::string &Value);
    std::string GetHeaders(const std::string &Key);
};

extern std::wstring CodeCvt_StrToWStr(const std::string &Src, UINT CodePage);
extern std::string CodeCvt_WStrToStr(const std::wstring &Src, UINT CodePage);
extern void CodeCvt_WcharToChar(wchar_t *pcwSrc, char *&pcDst, UINT CodePage);
extern void CodeCvt_CharToWchar(char *pcSrc, wchar_t *&pcwDst, UINT CodePage);

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
