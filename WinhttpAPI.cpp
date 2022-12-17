#include <functional>
#include "WinhttpAPI.h"
#include "WinhttpInterface/APIInterface.h"
#include "ParamsProcess/ParamsProcess.h"
#include "CodeConvert/CodeCvt.h"

using std::string;
using std::wstring;

void fnCheckResult(const string &ErrorFrom, DWORD ErrorCode = 0){
    printf("[error]%s, %#0lx\n",ErrorFrom.c_str(),ErrorCode);
}

int cWinHttpAPI::Request() {
    if (pHttpRequest != nullptr && pHttpResponse != nullptr)
        return Request(*pHttpRequest,*pHttpResponse);
    return -1;
}
int cWinHttpAPI::Request(stHttpRequest &HttpRequest, stHttpResponse &HttpResponse) {
    //Check
    if (HttpRequest.Url.empty() || HttpRequest.Model.empty()) {
        fnCheckResult("wszUrl == empty or wszModel == empty");
        return -1;
    }

    //ClearData
    if (!HttpResponse.Body.empty() || !HttpResponse.Headers.empty()) {
        HttpResponse.Body.clear();
        HttpResponse.Headers.clear();
    }

    //Declare
    DWORD dwErrorCode;
    DWORD dwFlag = 0;
    WinHttpInterface httpInterface;

    //Open
    wstring wszUA = CodeCvt::StrToWstr(ParamProcess::GetUA(this->Headers), CP_ACP);
    wstring wszProxy = CodeCvt::StrToWstr(HttpRequest.Proxy, CP_ACP);
    wstring wszProxyBypass = CodeCvt::StrToWstr(HttpRequest.ProxyBypass, CP_ACP);
    if ((dwErrorCode = httpInterface.Open(wszUA, wszProxy, wszProxyBypass, 0)) != 0)
        fnCheckResult("Open", dwErrorCode);

    //Crack
    wstring wszUrl = CodeCvt::StrToWstr(HttpRequest.Url, CP_ACP);
    URL_COMPONENTS UrlComponents = ParamProcess::InitUrlComponents(HttpRequest.Url);
    httpInterface.CrackUrl(wszUrl, wszUrl.length(), 0, UrlComponents);

    //Connect
    wstring wszUrlHostName (&UrlComponents.lpszHostName[0], &UrlComponents.lpszHostName[UrlComponents.dwHostNameLength]);
    if ((dwErrorCode = httpInterface.Connect(wszUrlHostName, UrlComponents.nPort)) != 0)
        fnCheckResult("Connect", dwErrorCode);

    //OpenRequest
    wstring wszModel = CodeCvt::StrToWstr(HttpRequest.Model, CP_ACP);
    switch (UrlComponents.nScheme) {
        case INTERNET_SCHEME_HTTP:
            dwFlag = 0;
            break;
        case INTERNET_SCHEME_HTTPS:
            dwFlag = WINHTTP_FLAG_SECURE;
            break;
    }
    if ((dwErrorCode = httpInterface.OpenRequest(wszModel, UrlComponents.lpszUrlPath, L"", dwFlag)) != 0)
        fnCheckResult("OpenRequest", dwErrorCode);

    //SetTimeOut
    httpInterface.SetTimeOut(HttpRequest.TimeOut.ResolveTimeout, HttpRequest.TimeOut.ConnectTimeout, HttpRequest.TimeOut.SendTimeout, HttpRequest.TimeOut.ReceiveTimeout);

    //SetOption
    if (HttpRequest.Option.dwOption != 0)
        httpInterface.SetOption(HttpRequest.Option.dwOption,HttpRequest.Option.lpBuffer);

    //AddRequestHeaders
    for (const auto &ch : this->Headers) {
        wstring header = (CodeCvt::StrToWstr(ch.first + ": " + ch.second + "\r\n",CP_ACP));
        httpInterface.AddRequestHeaders(header,0);
    }

    //SendRequest
    if ((dwErrorCode = httpInterface.SendRequest(L"", HttpRequest.Body, HttpRequest.Body.length(), HttpRequest.Body.length())) != 0)
        fnCheckResult("SendRequest", dwErrorCode);

    //ReceiveResponse
    if ((dwErrorCode = httpInterface.ReceiveResponse()) != 0)
        fnCheckResult("ReceiveResponse", dwErrorCode);

    //QueryHeaders
    DWORD dwBufLen = httpInterface.QueryHeaders(nullptr, 0);
    if (dwBufLen > 0){
        std::unique_ptr<wchar_t []> wcharBuf(new wchar_t [dwBufLen]);
        memset(wcharBuf.get(), 0, dwBufLen);
        httpInterface.QueryHeaders(wcharBuf.get(), dwBufLen);

        std::unique_ptr<char[]> charBuf = CodeCvt::WstrToStr(wcharBuf.get(), CP_ACP);
        HttpResponse.Headers.append(charBuf.get(), strlen(charBuf.get()));
    }

    //QueryDataAvailable & ReadData
    if ((dwErrorCode = httpInterface.QueryDataAvailable(dwBufLen)) == 0 && dwBufLen > 0) {
        FILE *fp = nullptr;
        std::function<void(char *, DWORD, FILE *, stHttpResponse &)> fnWriteData;
        if (HttpRequest.PathOfDownloadFile.empty())
            fnWriteData = [](char *Buf, DWORD BufLen, FILE *fp, stHttpResponse &HttpResponse) -> void {
                HttpResponse.Body.append(Buf, BufLen);
            };
        else {
            fnWriteData = [](char *Buf, DWORD BufLen, FILE *fp, stHttpResponse &HttpResponse) -> void {
                fwrite(Buf, BufLen, 1, fp);
            };
            fopen_s(&fp, HttpRequest.PathOfDownloadFile.c_str(), "wb+");
        }
        do {
            std::unique_ptr<char[]> charBuf(new char[dwBufLen]);
            httpInterface.ReadData(charBuf.get(), dwBufLen);
            fnWriteData(charBuf.get(), dwBufLen, fp, HttpResponse);
            dwErrorCode = httpInterface.QueryDataAvailable(dwBufLen);
        } while (dwErrorCode == 0 && dwBufLen > 0);

        if (fp != nullptr) fclose(fp);
    }

    //CloseHandle
    httpInterface.CloseAllHandles();
    return 0;
}
bool cWinHttpAPI::SetHeader(const string &Key, const string &Value) {
    if (Key.empty())
        return false;
    if (Value.empty())
        this->Headers.erase(Key);
    else if (this->Headers.find(Key) != this->Headers.cend())
        this->Headers[Key, Value];
    else
        this->Headers.insert({Key,Value});
    return true;
}
bool cWinHttpAPI::SetHeaders(const std::map<string, string> &KeyValue) {
    for (auto &headerKeyValue: KeyValue)
        if (!this->SetHeader(headerKeyValue.first, headerKeyValue.second))
            return false;
    return true;
}
string cWinHttpAPI::GetHeader(const string &Key) {
    if (Key.empty() || this->Headers.find(Key) == this->Headers.end())
        return "";
    return this->Headers[Key];
}
