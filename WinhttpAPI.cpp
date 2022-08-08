#include "WinhttpAPI.h"

void fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst){
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
void fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst){
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
void fn_EncodingSwitch(std::string &cpSrc,std::string &cpDst, unsigned CP_src, unsigned CP_dst){
    char *cpData;
    wchar_t *wcpData;
    fn_Mbs2Wcs(cpSrc.data(),wcpData,CP_src);
    fn_Wcs2Mbs(wcpData, cpData, CP_dst);
    cpDst.clear();
    cpDst.append(cpData);
    free(cpData);
    free(wcpData);
}

void fn_CharToUp(string &szData){
    for (char &ch:szData) {
        ch = (char)toupper(ch);
    }
}
vector<string> fn_SplitStr(const string &Data, const string &SplitStr){
    char *remainBuf= const_cast<char *>(Data.data());
    vector<string> out;
    while (strlen(remainBuf) > 0){
        char *c = strtok_s(remainBuf, SplitStr.c_str(), &remainBuf);
        if (c != nullptr)
            out.emplace_back(c);
    }
    return out;
}
string fn_Header_GetValue(const string &Key,const string &findKey){
    unsigned pos;
    if ((pos = Key.find(findKey)) != string::npos){
        pos += findKey.length();
        if ((pos = Key.find(':', pos)) != string::npos){
            ++pos;
            unsigned cmp = 0;
            while ((cmp = Key.find(' ', pos + cmp)) != string::npos){
                ++pos;
            }
            return Key.substr(pos);
        }
    }
    return "";
}

void fn_ErrorOutput(const string &Msg,unsigned long errorCode){
    printf("[error]%s , Last error: %#0lX\n",Msg.c_str(),errorCode);
}

class operationRequest{
private:
    wchar_t *wcpData = nullptr;
    void initUA(const string &headers){
        vector<string> vHandles = fn_SplitStr(headers,"\r\n");
        string HeaderValue;
        for (const string &ch:vHandles) {
            if (!(HeaderValue = fn_Header_GetValue(ch, "User-Agent")).empty()){
                fn_Mbs2Wcs(HeaderValue.data(), wcpData, CP_ACP);
                UA.assign(wcpData);
                free(wcpData);
                break;
            }
        }
    }
    void initProxy(const string &proxy){
        char *str = const_cast<char *>(proxy.data());
        fn_Mbs2Wcs(str,wcpData,CP_ACP);
        Proxy.assign(wcpData);
        free(wcpData);
    }
    void initModel(const string &model){
        string str = const_cast<char *>(model.data());
        fn_CharToUp(str);
        fn_Mbs2Wcs(str.data(),wcpData,CP_ACP);
        Model.assign(wcpData);
        free(wcpData);
    }
    void initHeaders(const string &headers){
        char *str = const_cast<char *>(headers.data());
        fn_Mbs2Wcs(str,wcpData,CP_ACP);
        Headers.assign(wcpData);
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
        fn_Mbs2Wcs(str, wcpData, CP_ACP);
        if (!WinHttpCrackUrl(wcpData,wcslen(wcpData),0,&urlComponents)){
            fn_ErrorOutput("WinHttpCrackUrl",GetLastError());
        }
        HostName = urlComponents.lpszHostName;
        HostName = HostName.substr(0,urlComponents.dwHostNameLength);
    }
public:
    wstring UA = L"LogStatistic";
    wstring Proxy;
    wstring Model;
    wstring Headers;
    wstring HostName;
    URL_COMPONENTS urlComponents{};
    explicit operationRequest(stHttpRequest &Request){
        initHeaders(Request.Headers);
        initUA(Request.Headers);
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
    bool Open(const wstring &UA, const wstring &Proxy){
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
    bool Connect(const wstring &HostName,URL_COMPONENTS &urlComponents){
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
    bool OpenRequest(const wstring &Model,URL_COMPONENTS &urlComponents){
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
    bool AddRequestHeaders(const wstring &Headers){
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
    bool SendRequest(const wstring &Headers,const string &Body){
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
            fn_Wcs2Mbs(wcpBuffer,cpData,CP_ACP);
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
int Winhttp_Request(stHttpRequest &httpRequest,stHttpResponse &httpResponse){
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
    winHttp.QueryHeaders(httpResponse.Headers);
    winHttp.QueryDataAvailable();
    winHttp.ReadData(httpResponse.Body);
    winHttp.Clear();
    return 0;
}
