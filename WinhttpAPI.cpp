#include "WinhttpAPI.h"
#include "WinhttpInterface/APIInterface.h"
#include "ParamsProcess/ParamsProcess.h"
#include "CodeConvert/CodeCvt.h"

using std::string;
using std::wstring;

int cWinhttpAPI::Request(stHttpRequest &httpRequest, stHttpResponse &httpResponse) {
    //Check
    if (httpRequest.Url.empty() || httpRequest.Model.empty()){
        printf("[error] Exact request method or url\n");
        return -1;
    }

    //Declare
    WinHttpInterface httpInterface;

    //Open
    wstring UA = CodeCvt_StrToWStr(ParamsProcess_GetUA(this->Headers),CP_ACP);
    wstring Proxy = CodeCvt_StrToWStr(httpRequest.Proxy,CP_ACP);
    wstring ProxyBypass = CodeCvt_StrToWStr(httpRequest.ProxyBypass,CP_ACP);
    httpInterface.Open(UA,Proxy,ProxyBypass,0);

    //Crack
    wstring Url = CodeCvt_StrToWStr(httpRequest.Url,CP_ACP);
    URL_COMPONENTS UrlComponents = ParamsProcess_GetUrlComponents(httpRequest.Url);
    httpInterface.CrackUrl(Url, Url.length(),0,UrlComponents);
    wstring UrlHostName (&UrlComponents.lpszHostName[0],&UrlComponents.lpszHostName[UrlComponents.dwHostNameLength]);

    //Connect
    httpInterface.Connect(UrlHostName,UrlComponents.nPort);

    //OpenRequest
    wstring Model = CodeCvt_StrToWStr(httpRequest.Model,CP_ACP);
    httpInterface.OpenRequest(Model,UrlComponents.lpszUrlPath, L"",0);

    //SetTimeOut
    httpInterface.SetTimeOut(httpRequest.TimeOut.ResolveTimeout,httpRequest.TimeOut.ConnectTimeout,httpRequest.TimeOut.SendTimeout,httpRequest.TimeOut.ReceiveTimeout);

    //AddRequestHeaders
    for (const auto &ch : this->Headers) {
        wstring header = (CodeCvt_StrToWStr(ch.first + ": " + ch.second + "\r\n",CP_ACP));
        httpInterface.AddRequestHeaders(header,0);
    }

    //SendRequest
    httpInterface.SendRequest(L"",httpRequest.Body,httpRequest.Body.length(),httpRequest.Body.length());

    //ReceiveResponse
    httpInterface.ReceiveResponse();

    //QueryHeaders
    DWORD headersLen;
    if ((headersLen = httpInterface.QueryHeaders(nullptr,0)) > 0){
        auto *Buffer = (wchar_t *)malloc(headersLen);
        memset(Buffer, 0, headersLen);
        httpInterface.QueryHeaders(Buffer, headersLen);
        char *charBuffer = nullptr;
        CodeCvt_WcharToChar(Buffer, charBuffer, CP_ACP);
        httpResponse.Headers = charBuffer;
        free(charBuffer);
        free(Buffer);
    }

    //QueryDataAvailable & ReadData
    DWORD BufferLen = httpInterface.QueryDataAvailable();
    while (BufferLen > 0){
        auto *Buffer = (char *)malloc(BufferLen);
        memset(Buffer,0,BufferLen);
        httpInterface.ReadData(Buffer, BufferLen);
        httpResponse.Body.append(Buffer);
        free(Buffer);
        BufferLen = httpInterface.QueryDataAvailable();
    }

    //CloseHandle
    httpInterface.CloseAllHandles();
    return 0;
}
bool cWinhttpAPI::SetHeaders(const string &Key, const string &Value) {
    if (Key.empty() || Value.empty())
        return false;
    if (this->Headers.find(Key) != this->Headers.end())
        this->Headers[Key,Value];
    else
        this->Headers.insert({Key, Value});
    return true;
}
string cWinhttpAPI::GetHeaders(const string &Key) {
    if (Key.empty() || this->Headers.find(Key) == this->Headers.end())
        return "";
    return this->Headers[Key];
}
