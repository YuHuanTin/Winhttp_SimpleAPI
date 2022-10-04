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
struct stWinHttpOption{
    DWORD dwOption = 0;
    DWORD lpBuffer = 0;
};
struct stHttpRequest{
    std::string Url;
    std::string Model;
    std::string Body;
    std::string Proxy;
    std::string ProxyBypass;
    std::string PathOfDownloadFile;
    stTimeOut TimeOut;
    stWinHttpOption Option;
};
struct stHttpResponse{
    std::string Body;
    std::string Headers;
};
class cWinHttpAPI{
private:
    std::map<std::string,std::string> Headers;
public:
    cWinHttpAPI() = default;
    cWinHttpAPI(stHttpRequest &HttpRequest, stHttpResponse &HttpResponse){
        Request(HttpRequest, HttpResponse);
    }
    int Request(stHttpRequest &Buffer, stHttpResponse &HttpResponse);
    bool SetHeader(const std::string &Key, const std::string &Value);
    std::string GetHeader(const std::string &Key);
};

extern std::wstring CodeCvt_StrToWStr(const std::string &Src, UINT CodePage);
extern std::string CodeCvt_WStrToStr(const std::wstring &Src, UINT CodePage);
extern void CodeCvt_WcharToChar(wchar_t *Src, char *&Dst, UINT CodePage);
extern void CodeCvt_CharToWchar(char *Src, wchar_t *&Dst, UINT CodePage);

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
