#include <functional>
#include "WinhttpAPI.h"
#include "WinhttpInterface/APIInterface.h"
#include "ParamsProcess/ParamsProcess.h"
#include "CodeConvert/CodeCvt.h"

using std::string;
using std::wstring;

extern void fnCheckResult(const std::string &ErrorFrom, DWORD ErrorCode);
void fnCheckResult(const string &ErrorFrom, DWORD ErrorCode = 0){
    printf("[error]%s, %#0lx\n",ErrorFrom.c_str(),ErrorCode);
}

int cWinHttpAPI::Request(stHttpRequest &HttpRequest, stHttpResponse &HttpResponse) {
    //Check
    if (HttpRequest.Url.empty() || HttpRequest.Model.empty()){
        fnCheckResult("Url == empty or Model == empty");
        return -1;
    }

    //Declare
    DWORD dwErrorCode;
    DWORD Flag = 0;
    WinHttpInterface httpInterface;

    //Open
    wstring UA = CodeCvt_StrToWStr(ParamsProcess_GetUA(this->Headers),CP_ACP);
    wstring Proxy = CodeCvt_StrToWStr(HttpRequest.Proxy, CP_ACP);
    wstring ProxyBypass = CodeCvt_StrToWStr(HttpRequest.ProxyBypass, CP_ACP);
    if ((dwErrorCode = httpInterface.Open(UA,Proxy,ProxyBypass,0)) != 0)
        fnCheckResult("Open", dwErrorCode);

    //Crack
    wstring Url = CodeCvt_StrToWStr(HttpRequest.Url, CP_ACP);
    URL_COMPONENTS UrlComponents = ParamsProcess_InitUrlComponents(HttpRequest.Url);
    httpInterface.CrackUrl(Url, Url.length(),0,UrlComponents);

    //Connect
    wstring UrlHostName (&UrlComponents.lpszHostName[0],&UrlComponents.lpszHostName[UrlComponents.dwHostNameLength]);
    if ((dwErrorCode = httpInterface.Connect(UrlHostName,UrlComponents.nPort)) != 0)
        fnCheckResult("Connect", dwErrorCode);

    //OpenRequest
    wstring Model = CodeCvt_StrToWStr(HttpRequest.Model, CP_ACP);
    switch (UrlComponents.nScheme) {
        case INTERNET_SCHEME_HTTP:
            Flag = 0;
            break;
        case INTERNET_SCHEME_HTTPS:
            Flag = WINHTTP_FLAG_SECURE;
            break;
    }
    if ((dwErrorCode = httpInterface.OpenRequest(Model,UrlComponents.lpszUrlPath, L"",Flag)) != 0)
        fnCheckResult("OpenRequest", dwErrorCode);

    //SetTimeOut
    httpInterface.SetTimeOut(HttpRequest.TimeOut.ResolveTimeout, HttpRequest.TimeOut.ConnectTimeout, HttpRequest.TimeOut.SendTimeout, HttpRequest.TimeOut.ReceiveTimeout);

    //SetOption
    if (HttpRequest.Option.dwOption != 0)
        httpInterface.SetOption(HttpRequest.Option.dwOption,HttpRequest.Option.lpBuffer);

    //AddRequestHeaders
    for (const auto &ch : this->Headers) {
        wstring header = (CodeCvt_StrToWStr(ch.first + ": " + ch.second + "\r\n",CP_ACP));
        httpInterface.AddRequestHeaders(header,0);
    }

    //SendRequest
    if ((dwErrorCode = httpInterface.SendRequest(L"", HttpRequest.Body, HttpRequest.Body.length(), HttpRequest.Body.length())) != 0)
        fnCheckResult("SendRequest", dwErrorCode);

    //ReceiveResponse
    if ((dwErrorCode = httpInterface.ReceiveResponse()) != 0)
        fnCheckResult("ReceiveResponse", dwErrorCode);

    //QueryHeaders
    DWORD BufferLen = httpInterface.QueryHeaders(nullptr,0);
    if (BufferLen > 0){
        auto *Buffer = (wchar_t *)malloc(BufferLen);
        memset(Buffer, 0, BufferLen);
        httpInterface.QueryHeaders(Buffer, BufferLen);
        char *charBuffer = nullptr;
        CodeCvt_WcharToChar(Buffer, charBuffer, CP_ACP);
        HttpResponse.Headers.append(charBuffer, BufferLen);
        free(charBuffer);
        free(Buffer);
    }

    //QueryDataAvailable & ReadData
    BufferLen = httpInterface.QueryDataAvailable();
    if (BufferLen > 0){
        FILE *fp = nullptr;
        std::function<void(char *,DWORD,FILE *,stHttpResponse &)> WriteData;
        if (HttpRequest.PathOfDownloadFile.empty())
            WriteData = [](char *Buf, DWORD BufLen, FILE *fp, stHttpResponse &HttpResponse)->void {HttpResponse.Body.append(Buf, BufLen);};
        else{
            WriteData = [](char *Buf, DWORD BufLen, FILE *fp, stHttpResponse &HttpResponse)->void {fwrite(Buf, BufLen, 1, fp);};
            fopen_s(&fp, HttpRequest.PathOfDownloadFile.c_str(), "wb+");
        }
        while (BufferLen > 0){
            char *Buffer = (char *)malloc(BufferLen);
            httpInterface.ReadData(Buffer, BufferLen);
            WriteData(Buffer, BufferLen, fp, HttpResponse);
            free(Buffer);
            BufferLen = httpInterface.QueryDataAvailable();
        }
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
