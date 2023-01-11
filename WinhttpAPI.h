#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#include <string>
#include <map>
#include "Windows.h"
#include "CodeConvert/CodeCvt.h"

struct stTimeOut {
    int ResolveTimeout = 3000;
    int ConnectTimeout = 3000;
    int SendTimeout = 3000;
    int ReceiveTimeout = 3000;
};
struct stWinHttpOption {
    DWORD dwOption = 0;
    DWORD lpBuffer = 0;
};
struct stHttpRequest {
    std::string Url;
    std::string Model;
    std::string Body;
    std::string Proxy;
    std::string ProxyBypass;
    struct stSaveMethod {
        enum eMethod {
            STRING_STREAM, FILE_STREAM
        } Method;
        std::string SavePath;
        stSaveMethod(eMethod DownloadMethod, const std::string &Buffer = std::string()) : Method(DownloadMethod), SavePath(Buffer) {}
    } SaveMethod{stSaveMethod::eMethod::STRING_STREAM};
    stTimeOut TimeOut;
    stWinHttpOption WinhttpOption;
};
struct stHttpResponse {
    std::string Body;
    std::string Headers;
};

class cWinHttpAPI {
private:
    std::map<std::string, std::string> Headers;
    stHttpRequest *pHttpRequest = nullptr;
    stHttpResponse *pHttpResponse = nullptr;
public:
    cWinHttpAPI() = default;

    cWinHttpAPI(stHttpRequest &httpRequest, stHttpResponse &httpResponse)
            : pHttpRequest(&httpRequest), pHttpResponse(&httpResponse) {}

    //use with ctor(httpRequest,httpResponse)
    int Request();

    //use with ctor(default)
    int Request(stHttpRequest &Buffer, stHttpResponse &HttpResponse);
    bool SetHeader(const std::string &Key, const std::string &Value);
    bool SetHeaders(const std::map<std::string, std::string> &KeyValue);
    std::string GetHeader(const std::string &Key);
};

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
