#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#include <string>
#include <map>
#include "Windows.h"

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
    std::map<std::string, std::string> Headers;
public:
    cWinHttpAPI() = default;

    int Request(stHttpRequest &Buffer, stHttpResponse &HttpResponse);
    bool SetHeader(const std::string &Key, const std::string &Value);
    bool SetHeaders(const std::map<std::string, std::string> &KeyValue);
    std::string GetHeader(const std::string &Key);
};

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
