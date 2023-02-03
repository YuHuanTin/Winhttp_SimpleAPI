#ifndef WINHTTP_SIMPLEAPI_WINHTTPAPI_H
#define WINHTTP_SIMPLEAPI_WINHTTPAPI_H

#include <map>
#include <string>
#include <Windows.h>

struct TimeoutT {
    int resolveTimeout = 3000;
    int connectTimeout = 3000;
    int sendTimeout = 3000;
    int receiveTimeout = 3000;
};
struct WinhttpOptionT {
    DWORD dwOption = 0;
    DWORD lpBuffer = 0;
};
struct HttpRequestT {
    std::string url;
    std::string protocol;
    std::string body;
    std::string proxy;
    std::string proxyBypass;

    struct SaveMethodT {
        enum MethodE {
            STRING_STREAM, FILE_STREAM
        };
        MethodE downloadMethod;
        std::string downloadPath;

        SaveMethodT(MethodE DownloadMethod, std::string DownloadPath = {}) : downloadMethod(DownloadMethod), downloadPath(std::move(DownloadPath)) {}
    };

    SaveMethodT saveMethod;
    TimeoutT timeout;
    WinhttpOptionT winhttpOption;

    HttpRequestT() : saveMethod(SaveMethodT::MethodE::STRING_STREAM) {}

    HttpRequestT(std::string Url, std::string Protocol, SaveMethodT SaveMethod = {SaveMethodT::STRING_STREAM})
            : url(std::move(Url)), protocol(std::move(Protocol)), saveMethod(std::move(SaveMethod)) {
    }
};
struct HttpResponseT {
    std::string Body;
    std::string Headers;
};

class WinhttpAPI {
private:
    std::map<std::string, std::string> headers;
    HttpRequestT *pHttpRequest = nullptr;
    HttpResponseT *pHttpResponse = nullptr;
public:
    WinhttpAPI() = default;

    WinhttpAPI(HttpRequestT &HttpRequest, HttpResponseT &HttpResponse) : pHttpRequest(&HttpRequest), pHttpResponse(&HttpResponse) {}

    void Request();

    void Request(HttpRequestT &HttpRequest, HttpResponseT &HttpResponse);

    bool SetHeader(const std::string &Key, const std::string &Value);

    bool SetHeaders(const std::map<std::string, std::string> &KeyValue);

    std::string GetHeader(const std::string &Key);
};

#endif //WINHTTP_SIMPLEAPI_WINHTTPAPI_H
