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

int cWinhttpAPI::Request(stHttpRequest &httpRequest, stHttpResponse &httpResponse) {
    DWORD dwErrorCode;
    DWORD Flag = 0;

    //Check
    if (httpRequest.Url.empty() || httpRequest.Model.empty()){
        fnCheckResult("Url == empty or Model == empty");
        return -1;
    }

    //Declare
    WinHttpInterface httpInterface;

    //Open
    wstring UA = CodeCvt_StrToWStr(ParamsProcess_GetUA(this->Headers),CP_ACP);
    wstring Proxy = CodeCvt_StrToWStr(httpRequest.Proxy,CP_ACP);
    wstring ProxyBypass = CodeCvt_StrToWStr(httpRequest.ProxyBypass,CP_ACP);
    if ((dwErrorCode = httpInterface.Open(UA,Proxy,ProxyBypass,0)) != 0)
        fnCheckResult("Open", dwErrorCode);

    //Crack
    wstring Url = CodeCvt_StrToWStr(httpRequest.Url,CP_ACP);
    URL_COMPONENTS UrlComponents = ParamsProcess_InitUrlComponents(httpRequest.Url);
    httpInterface.CrackUrl(Url, Url.length(),0,UrlComponents);

    //Connect
    wstring UrlHostName (&UrlComponents.lpszHostName[0],&UrlComponents.lpszHostName[UrlComponents.dwHostNameLength]);
    if ((dwErrorCode = httpInterface.Connect(UrlHostName,UrlComponents.nPort)) != 0)
        fnCheckResult("Connect", dwErrorCode);

    //OpenRequest
    wstring Model = CodeCvt_StrToWStr(httpRequest.Model,CP_ACP);
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
    httpInterface.SetTimeOut(httpRequest.TimeOut.ResolveTimeout,httpRequest.TimeOut.ConnectTimeout,httpRequest.TimeOut.SendTimeout,httpRequest.TimeOut.ReceiveTimeout);

    //AddRequestHeaders
    for (const auto &ch : this->Headers) {
        wstring header = (CodeCvt_StrToWStr(ch.first + ": " + ch.second + "\r\n",CP_ACP));
        httpInterface.AddRequestHeaders(header,0);
    }

    //SendRequest
    if ((dwErrorCode = httpInterface.SendRequest(L"",httpRequest.Body,httpRequest.Body.length(),httpRequest.Body.length())) != 0)
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
        httpResponse.Headers.append(charBuffer,BufferLen);
        free(charBuffer);
        free(Buffer);
    }

    //QueryDataAvailable & ReadData
    BufferLen = httpInterface.QueryDataAvailable();
    while (BufferLen > 0){
        auto *Buffer = (char *)malloc(BufferLen);
        memset(Buffer,0,BufferLen);
        httpInterface.ReadData(Buffer, BufferLen);
        httpResponse.Body.append(Buffer,BufferLen);
        free(Buffer);
        BufferLen = httpInterface.QueryDataAvailable();
    }

    //CloseHandle
    httpInterface.CloseAllHandles();
    return 0;
}
bool cWinhttpAPI::SetHeader(const string &Key, const string &Value) {
    if (Key.empty() || Value.empty())
        return false;
    if (this->Headers.find(Key) != this->Headers.end())
        this->Headers[Key,Value];
    else
        this->Headers.insert({Key, Value});
    return true;
}
string cWinhttpAPI::GetHeader(const string &Key) {
    if (Key.empty() || this->Headers.find(Key) == this->Headers.end())
        return "";
    return this->Headers[Key];
}
