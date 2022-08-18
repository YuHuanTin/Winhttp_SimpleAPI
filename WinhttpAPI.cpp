#include "WinhttpAPI.h"

using std::string;
using std::vector;

void fn_ErrorOutput(const string &Msg,unsigned long errorCode){
    printf("[error]%s , Last error: %#0lX\n",Msg.c_str(),errorCode);
}
void fn_CharToUp(string &szData){
    for (char &ch:szData) {
        ch = (char)toupper(ch);
    }
}
vector<string> fn_SplitStr(const string &Data, const string &SplitStr){
    string strRemain = Data + SplitStr;
    vector<string> out;
    if (strRemain.length() > 0){
        size_t pos[2] = {0};
        while ((pos[1] = strRemain.find(SplitStr,pos[0])) != string::npos){
            if (pos[0] == pos[1]){
                pos[0] += SplitStr.length();
                continue;
            }
            out.emplace_back(strRemain.substr(pos[0],pos[1] - pos[0]));
            pos[0] = pos[1] + SplitStr.length();
        }
    }
    return out;
}

class operationRequest{
private:
    wchar_t *wcpData = nullptr;
    void initProxy(const string &proxy){
        char *str = const_cast<char *>(proxy.data());
        cWinHttpSimpleAPI::fn_Mbs2Wcs(str,wcpData,CP_ACP);
        Proxy.assign(wcpData);
        free(wcpData);
    }
    void initModel(const string &model){
        string str = const_cast<char *>(model.data());
        fn_CharToUp(str);
        cWinHttpSimpleAPI::fn_Mbs2Wcs(str.data(),wcpData,CP_ACP);
        Model.assign(wcpData);
        free(wcpData);
    }
    void initUrl(const string &url){
        memset(&urlComponents, 0, sizeof(urlComponents));
        urlComponents.dwStructSize = sizeof(urlComponents);
        urlComponents.dwSchemeLength    = -1;
        urlComponents.dwHostNameLength  = -1;
        urlComponents.dwUrlPathLength   = -1;
        urlComponents.dwExtraInfoLength = -1;

        char *str = const_cast<char *>(url.data());
        cWinHttpSimpleAPI::fn_Mbs2Wcs(str, wcpData, CP_ACP);
        if (!WinHttpCrackUrl(wcpData,wcslen(wcpData),0,&urlComponents)){
            fn_ErrorOutput("WinHttpCrackUrl",GetLastError());
        }
        HostName = urlComponents.lpszHostName;
        HostName = HostName.substr(0,urlComponents.dwHostNameLength);
    }
    void initHeaders(const std::map<string,string> &mHeaders){
        if (mHeaders.empty()){
            return;
        }
        string strHeaders;
        char *str = strHeaders.data();
        if (mHeaders.find("User-Agent") != mHeaders.end()){
            strHeaders = mHeaders.at("User-Agent");
            str = strHeaders.data();
            cWinHttpSimpleAPI::fn_Mbs2Wcs(str,wcpData,CP_ACP);
            UA.assign(wcpData);
            free(wcpData);
        }
        strHeaders.clear();
        for (const auto &keyValue:mHeaders) {
            strHeaders += (keyValue.first + ": " + keyValue.second +"\r\n");
        }
        str = strHeaders.data();
        cWinHttpSimpleAPI::fn_Mbs2Wcs(str,wcpData,CP_ACP);
        Headers.assign(wcpData);
        free(wcpData);
    }
public:
    std::wstring UA = L"LogStatistic";
    std::wstring Proxy;
    std::wstring Model;
    std::wstring Headers;
    std::wstring HostName;
    URL_COMPONENTS urlComponents{};
    explicit operationRequest(stHttpRequest &Request){
        initHeaders(Request.Headers);
        initProxy(Request.Proxy);
        initModel(Request.Model);
        initUrl(Request.Url);
    }
};
class operationWinHttp{
private:
    char *cpData = nullptr;
    DWORD dwNumberOfBytesToRead = 0;
    HINTERNET   hSession = nullptr,
                hConnect = nullptr,
                hRequest = nullptr;
public:
    bool Open(const std::wstring &UA, const std::wstring &Proxy){
        hSession = WinHttpOpen(UA.c_str(),
                               Proxy.empty() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_NAMED_PROXY,
                               Proxy.empty() ? WINHTTP_NO_PROXY_NAME : Proxy.c_str(),
                               Proxy.empty() ? WINHTTP_NO_PROXY_BYPASS : nullptr,
                               0
        );
        if (hSession == nullptr){
            fn_ErrorOutput("WinHttpOpen",GetLastError());
            return false;
        }
        return true;
    }
    bool Connect(const std::wstring &HostName,URL_COMPONENTS &urlComponents){
        hConnect = WinHttpConnect(hSession,
                                  HostName.c_str(),
                                  (urlComponents.nPort==443||urlComponents.nPort==80) ? INTERNET_DEFAULT_PORT : urlComponents.nPort,
                                  0
        );
        if (hConnect == nullptr){
            fn_ErrorOutput("WinHttpConnect",GetLastError());
            return false;
        }
        return true;
    }
    bool OpenRequest(const std::wstring &Model,URL_COMPONENTS &urlComponents){
        hRequest = WinHttpOpenRequest(hConnect,
                                    Model.c_str(),
                                    urlComponents.lpszUrlPath,
                                    nullptr,
                                    nullptr,
                                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                                    WINHTTP_FLAG_SECURE
        );
        if (hRequest == nullptr){
            fn_ErrorOutput("WinHttpOpenRequest",GetLastError());
            return false;
        }
        return true;
    }
    bool SetOption(DWORD dwSetOption){
        if (!WinHttpSetOption(hRequest,
                              WINHTTP_OPTION_DISABLE_FEATURE,
                              &dwSetOption,
                              sizeof(dwSetOption)
        )){
            fn_ErrorOutput("WinHttpSetOption",GetLastError());
            return false;
        }
        return true;
    }
    bool SetTimeOut(int TimeOut){
        if (!WinHttpSetTimeouts(hRequest,
                                TimeOut,
                                TimeOut,
                                TimeOut,
                                TimeOut
        )){
            fn_ErrorOutput("WinHttpSetTimeouts",GetLastError());
            return false;
        }
        return true;
    }
    bool AddRequestHeaders(const std::wstring &Headers){
        if (!WinHttpAddRequestHeaders(hRequest,
                                      Headers.c_str(),
                                      Headers.length(),
                                      WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE
        )){
            fn_ErrorOutput("WinHttpAddRequestHeaders",GetLastError());
            return false;
        }
        return true;
    }
    bool SendRequest(const std::wstring &Headers,const string &Body){
        char *str = const_cast<char *>(Body.data());
        if (!WinHttpSendRequest(hRequest,
                                Headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : Headers.c_str(),
                                -1L,
                                Body.empty() ? WINHTTP_NO_REQUEST_DATA : str,
                                Body.length(),
                                Body.length(),
                                0
        )){
            fn_ErrorOutput("WinHttpSendRequest",GetLastError());
            return false;
        }
        return true;
    }
    bool ReceiveResponse(){
        if (!WinHttpReceiveResponse(hRequest,nullptr)){
            fn_ErrorOutput("WinHttpReceiveResponse",GetLastError());
            return false;
        }
        return true;
    }
    bool QueryHeaders(string &Headers){
        Headers.clear();
        WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_RAW_HEADERS_CRLF,
                            nullptr,
                            nullptr,
                            &dwNumberOfBytesToRead,
                            nullptr
        );
        if (dwNumberOfBytesToRead <= 0){
            return false;
        }
        if (dwNumberOfBytesToRead > 0) {
            auto *wcpBuffer = (wchar_t *)malloc(dwNumberOfBytesToRead);
            WinHttpQueryHeaders(hRequest,
                                WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                nullptr,
                                wcpBuffer,
                                &dwNumberOfBytesToRead,
                                nullptr
            );
            cWinHttpSimpleAPI::fn_Wcs2Mbs(wcpBuffer,cpData,CP_ACP);
            Headers.append(cpData);
            free(wcpBuffer);
            free(cpData);
        }
        return true;
    }
    bool QueryDataAvailable(){
        if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
            fn_ErrorOutput("WinHttpQueryDataAvailable",GetLastError());
            return false;
        }
        return true;
    }
    bool ReadData(string &Body){
        if (dwNumberOfBytesToRead <= 0){
            return false;
        }
        while (dwNumberOfBytesToRead > 0){
            DWORD dwRecv = 0;
            char *cpBuffer = (char *)malloc(dwNumberOfBytesToRead + 1);
            memset(cpBuffer,0,dwNumberOfBytesToRead+1);
            if (!WinHttpReadData(hRequest,cpBuffer, dwNumberOfBytesToRead,&dwRecv)){
                fn_ErrorOutput("WinHttpReadData",GetLastError());
                return false;
            }
            if (!QueryDataAvailable()){
                fn_ErrorOutput("QueryDataAvailable",GetLastError());
                return false;
            }
            Body.append(cpBuffer);
            free(cpBuffer);
        }
        return true;
    }
    void Clear(){
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }
};
int cWinHttpSimpleAPI::Winhttp_Request(stHttpRequest &httpRequest, stHttpResponse &httpResponse) {
    if (httpRequest.Url.empty() || httpRequest.Model.empty()){
        fn_ErrorOutput("Exact request method or url", 0);
        return -1;
    }
    operationRequest request(httpRequest);
    operationWinHttp winHttp;
    winHttp.Open(request.UA,request.Proxy);
    winHttp.Connect(request.HostName,request.urlComponents);
    winHttp.OpenRequest(request.Model,request.urlComponents);
    winHttp.SetOption(WINHTTP_DISABLE_REDIRECTS);
    if (httpRequest.TimeOut > 0){
        winHttp.SetTimeOut(httpRequest.TimeOut);
    }
    if (!request.Headers.empty()){
        winHttp.AddRequestHeaders(request.Headers);
    }
    winHttp.SendRequest(request.Headers,httpRequest.Body);
    winHttp.ReceiveResponse();
    winHttp.QueryHeaders(httpResponse.allHeaders);
    winHttp.QueryDataAvailable();
    winHttp.ReadData(httpResponse.Body);
    winHttp.Clear();

    if (!httpResponse.allHeaders.empty()){
        vector<string> vHeaders = fn_SplitStr(httpResponse.allHeaders,"\r\n");
        for (const string &ch:vHeaders) {
            vector<string> vKeyValue = fn_SplitStr(ch,": ");
            if (vKeyValue.size() == 2){
                if (httpResponse.Headers.find(vKeyValue[0]) != httpResponse.Headers.end()){
                    httpResponse.Headers[vKeyValue[0]] = httpResponse.Headers.at(vKeyValue[0]) + "\r\n" + vKeyValue[1];
                } else{
                    httpResponse.Headers.insert({vKeyValue[0],vKeyValue[1]});
                }
            }
        }
    }
    return 0;
}
bool cWinHttpSimpleAPI::Winhttp_SetHeaders(stHttpRequest &httpRequest,const string &key,const string &value) {
    if (key.empty() || value.empty()){
        return false;
    }
    if (httpRequest.Headers.find(key) != httpRequest.Headers.end()){
        httpRequest.Headers[key,value];
    } else{
        httpRequest.Headers.insert({key,value});
    }
    return true;
}
string cWinHttpSimpleAPI::Winhttp_GetHeaders(stHttpResponse &httpResponse,const string &key) {
    if (key.empty() || httpResponse.Headers.find(key) == httpResponse.Headers.end()){
        return "";
    }
    return httpResponse.Headers[key];
}
void cWinHttpSimpleAPI::fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst){
    cpDst = nullptr;
    int len = WideCharToMultiByte(CP_dst,
                                  NULL,
                                  wcpStr,
                                  -1,
                                  nullptr,
                                  0,
                                  nullptr,
                                  nullptr
    );
    cpDst = (char *)realloc(cpDst,len * sizeof(char));
    WideCharToMultiByte(CP_dst,
                        NULL,
                        wcpStr,
                        -1,
                        cpDst,
                        (int)(len * sizeof(char)),
                        nullptr,
                        nullptr
    );
}
void cWinHttpSimpleAPI::fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst){
    wcpDst = nullptr;
    int len = MultiByteToWideChar(CP_dst,
                                  NULL,
                                  cpStr,
                                  -1,
                                  nullptr,
                                  0
    );
    wcpDst = (wchar_t *)realloc(wcpDst,len * sizeof(wchar_t));
    MultiByteToWideChar(CP_dst,
                        NULL,
                        cpStr,
                        -1,
                        wcpDst,
                        (int)(len * sizeof(wchar_t))
    );
}
void cWinHttpSimpleAPI::fn_EncodingSwitch(string &cpSrc,string &cpDst, unsigned CP_src, unsigned CP_dst){
    char *cpData;
    wchar_t *wcpData;
    fn_Mbs2Wcs(cpSrc.data(),wcpData,CP_src);
    fn_Wcs2Mbs(wcpData, cpData, CP_dst);
    cpDst.clear();
    cpDst.append(cpData);
    free(cpData);
    free(wcpData);
}