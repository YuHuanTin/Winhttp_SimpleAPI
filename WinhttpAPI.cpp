#include "WinhttpAPI.h"
#include "CodeConvert/CodeCvt.h"
#include "ParamsProcess/ParamsProcess.h"
#include "WinHttpInterface/APIInterface.h"

using std::string;
using std::wstring;
using std::runtime_error;

void printException(std::exception &error, DWORD ec = 0) {
    std::cout << std::hex << "[-]error: " << error.what() << " ec: " << ec << '\n';
}

void WinhttpAPI::Request() {
    try {
        if (pHttpRequest == nullptr || pHttpResponse == nullptr) throw runtime_error("unset httprequest or httpresponse");
        Request(*pHttpRequest, *pHttpResponse);
    } catch (runtime_error &error) {
        printException(error, 0);
    }
}

void WinhttpAPI::Request(HttpRequestT &HttpRequest, HttpResponseT &HttpResponse) {
    DWORD lastError = 0;
    try {
        // Check
        if (HttpRequest.url.empty() || HttpRequest.protocol.empty()) throw runtime_error("unset httprequest.url or httpreques.protocol");

        // ClearLastResponseData
        if (!HttpResponse.Body.empty() || !HttpResponse.Headers.empty()) {
            HttpResponse.Body.clear();
            HttpResponse.Headers.clear();
        }

        //Declare
        DWORD dwFlag = 0;
        WinHttpInterface httpInterface;

        //Open
        wstring wszUA = CodeCvt::StrToWstr(ParamProcess::GetUA(this->headers), CP_ACP);
        wstring wszProxy = CodeCvt::StrToWstr(HttpRequest.proxy, CP_ACP);
        wstring wszProxyBypass = CodeCvt::StrToWstr(HttpRequest.proxyBypass, CP_ACP);
        if ((lastError = httpInterface.Open(wszUA, wszProxy, wszProxyBypass, 0)) != 0)
            throw runtime_error("failed Open");

        //Crack
        wstring wszUrl = CodeCvt::StrToWstr(HttpRequest.url, CP_ACP);
        URL_COMPONENTS UrlComponents = ParamProcess::InitUrlComponents();
        if ((lastError = httpInterface.CrackUrl(wszUrl, wszUrl.length(), 0, UrlComponents)) != 0)
            throw runtime_error("failed CrackUrl");

        //Connect
        wstring wszUrlHostName(&UrlComponents.lpszHostName[0], &UrlComponents.lpszHostName[UrlComponents.dwHostNameLength]);
        if ((lastError = httpInterface.Connect(wszUrlHostName, UrlComponents.nPort)) != 0)
            throw runtime_error("failed Connect");

        //OpenRequest
        wstring wszModel = CodeCvt::StrToWstr(HttpRequest.protocol, CP_ACP);
        switch (UrlComponents.nScheme) {
            case INTERNET_SCHEME_HTTP:
                dwFlag = 0;
                break;
            case INTERNET_SCHEME_HTTPS:
                dwFlag = WINHTTP_FLAG_SECURE;
                break;
        }
        if ((lastError = httpInterface.OpenRequest(wszModel, UrlComponents.lpszUrlPath, L"", dwFlag)) != 0)
            throw runtime_error("failed OpenRequest");

        //SetTimeOut
        httpInterface.SetTimeOut(HttpRequest.timeout.resolveTimeout, HttpRequest.timeout.connectTimeout,
                                 HttpRequest.timeout.sendTimeout, HttpRequest.timeout.receiveTimeout);

        //SetOption
        if (HttpRequest.winhttpOption.dwOption != 0)
            httpInterface.SetOption(HttpRequest.winhttpOption.dwOption, HttpRequest.winhttpOption.lpBuffer);

        //AddRequestHeaders
        for (const auto &ch: this->headers) {
            wstring header = (CodeCvt::StrToWstr(ch.first + ": " + ch.second + "\r\n", CP_ACP));
            httpInterface.AddRequestHeaders(header, 0);
        }

        //SendRequest
        if ((lastError = httpInterface.SendRequest(L"", HttpRequest.body, HttpRequest.body.length(),
                                                   HttpRequest.body.length())) != 0)
            throw runtime_error("failed SendRequest");

        //ReceiveResponse
        if ((lastError = httpInterface.ReceiveResponse()) != 0)
            throw runtime_error("failed ReceiveResponse");

        //QueryHeaders
        DWORD dwBufLen = httpInterface.QueryHeaders(nullptr, 0);
        if (dwBufLen > 0) {
            std::unique_ptr<wchar_t[]> wBuf = std::make_unique<wchar_t[]>(dwBufLen);
            memset(wBuf.get(), 0, dwBufLen);
            httpInterface.QueryHeaders(wBuf.get(), dwBufLen);

            std::unique_ptr<char[]> cBuf = CodeCvt::WstrToStr(wBuf.get(), CP_ACP);
            HttpResponse.Headers.append(cBuf.get(), strlen(cBuf.get()));
        }

        // QueryDataAvailable & ReadData
        if ((lastError = httpInterface.QueryDataAvailable(dwBufLen)) == 0 && dwBufLen > 0) {
            std::FILE *fp = nullptr;
            std::function<void(char *, DWORD)> writeData;
            if (HttpRequest.saveMethod.downloadMethod == HttpRequestT::SaveMethodT::STRING_STREAM) {
                writeData = [&HttpResponse](char *Buf, DWORD BufLen) {
                    HttpResponse.Body.append(Buf, BufLen);
                };
            } else {
                writeData = [&fp](char *Buf, DWORD BufLen) {
                    fwrite(Buf, BufLen, 1, fp);
                };
                if (fopen_s(&fp, HttpRequest.saveMethod.downloadPath.c_str(), "wb+") != 0)
                    throw runtime_error("failed open file");
            }
            do {
                std::unique_ptr<char[]> charBuf = std::make_unique<char[]>(dwBufLen);
                httpInterface.ReadData(charBuf.get(), dwBufLen);
                writeData(charBuf.get(), dwBufLen);
                lastError = httpInterface.QueryDataAvailable(dwBufLen);
            } while (lastError == 0 && dwBufLen > 0);
            if (lastError != 0)
                throw runtime_error("unknow error");
        }
    } catch (runtime_error &error) {
        printException(error, lastError);
    }
}

bool WinhttpAPI::SetHeader(const string &Key, const string &Value) {
    if (Key.empty())
        return false;
    if (Value.empty())
        this->headers.erase(Key);
    else if (this->headers.find(Key) != this->headers.cend())
        this->headers[Key, Value];
    else
        this->headers.insert({Key, Value});
    return true;
}

bool WinhttpAPI::SetHeaders(const std::map<string, string> &KeyValue) {
    for (auto &headerKeyValue: KeyValue)
        if (!this->SetHeader(headerKeyValue.first, headerKeyValue.second))
            return false;
    return true;
}

string WinhttpAPI::GetHeader(const string &Key) {
    if (Key.empty() || this->headers.find(Key) == this->headers.end())
        return {};
    return this->headers[Key];
}
