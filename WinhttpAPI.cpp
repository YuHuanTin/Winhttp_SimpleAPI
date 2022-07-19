#include "WinhttpAPI.h"

void fn_Wcs2Mbs(wchar_t *wcpStr,char *&cpDst,unsigned CP_dst){
    cpDst= nullptr;
    size_t len=WideCharToMultiByte(CP_dst,NULL,wcpStr, -1, nullptr, 0,nullptr,nullptr);
    cpDst=(char *)realloc(cpDst,len * sizeof(char));
    WideCharToMultiByte(CP_dst,NULL,wcpStr,-1,cpDst,(int)(len * sizeof(char)),nullptr,nullptr);
}
void fn_Mbs2Wcs(char *cpStr,wchar_t *&wcpDst,unsigned CP_dst){
    wcpDst= nullptr;
    size_t len=MultiByteToWideChar(CP_dst,NULL,cpStr,-1,nullptr,0);
    wcpDst=(wchar_t *)realloc(wcpDst,len * sizeof(wchar_t));
    MultiByteToWideChar(CP_dst,NULL,cpStr,-1,wcpDst,(int)(len * sizeof(wchar_t)));
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
void fn_Assign_WString(wstring &wStr, wchar_t *wcpStr){//赋值,并销毁内存
    wStr=wcpStr;
    free(wcpStr);
}
void fn_Assign_String(string &Str, char *cpStr){//赋值,并销毁内存
    Str=cpStr;
    free(cpStr);
}
void fn_CharToUp(string &szData){
    for (auto &ch:szData) {
        ch= (char)toupper(ch);
    }
}

vector<string> fn_SplitStr(string &szData,const string &szSplitChar){
    char *remainBuf= szData.data();
    vector<string> out;
    while (strlen(remainBuf)>0){
        char *c=strtok_s(remainBuf,szSplitChar.c_str(),&remainBuf);
        if (c != nullptr)
            out.emplace_back(c);
    }
    return out;
}
string fn_Header_GetData(const string& Header, const string& HeaderName){
    size_t pos;
    if ((pos = Header.find(HeaderName)) != string::npos){
        pos += HeaderName.size();
        if ((pos = Header.find(':',pos)) != string::npos){
            ++pos;
            size_t cmp=0;
            while ((cmp = Header.find(' ',pos + cmp)) != string::npos){
                ++pos;
            }
            return Header.substr(pos);
        }
    }
    return "";
}
wstring fn_Header_GetUA(string headers){
    vector<string> vHandles = fn_SplitStr(headers,"\r\n");
    string HeaderData;
    wchar_t *wcpData;
    for (const string &ch:vHandles) {
        if (!(HeaderData = fn_Header_GetData(ch, "User-Agent")).empty()){
            fn_Mbs2Wcs(HeaderData.data(),wcpData,CP_ACP);
            return wcpData;
        }
    }
    char UA[]{"LogStatistic"};
    fn_Mbs2Wcs(UA,wcpData,CP_ACP);
    return wcpData;
}
void fn_URL_Init(string Url, URL_COMPONENTS &stUrl){
    memset(&stUrl, 0, sizeof(stUrl));
    stUrl.dwStructSize = sizeof(stUrl);

    stUrl.dwSchemeLength    = (DWORD)-1;
    stUrl.dwHostNameLength  = (DWORD)-1;
    stUrl.dwUrlPathLength   = (DWORD)-1;
    stUrl.dwExtraInfoLength = (DWORD)-1;

    wchar_t *wcpUrl;
    fn_Mbs2Wcs(Url.data(), wcpUrl, CP_ACP);
    if (!WinHttpCrackUrl(wcpUrl, wcslen(wcpUrl), 0, &stUrl)){
        printf("WinHttpCrackUrl Error,LastError %lX\r\n",GetLastError());
    }

    auto *wszHostName= new wchar_t [stUrl.dwHostNameLength + 1];
    wmemset(wszHostName, 0, stUrl.dwHostNameLength + 1);
    wmemcpy(wszHostName, stUrl.lpszHostName, stUrl.dwHostNameLength);
    stUrl.lpszHostName=wszHostName;
}

stHttpResponse Winhttp_Request(stHttpRequest *httpRequest){
    stHttpResponse httpResponse;
    if (httpRequest->Url.empty() | httpRequest->Model.empty()){
        printf("[error]Exact request method or url\n");
        return httpResponse;
    }
    struct stProgressVar{
        wstring UA;
        wstring Proxy;
        wstring Model;
        wstring Headers;
    }progressVar;
    wchar_t *wcpData;//Must use fn_Mbs2Wcs or fn_Wcs2Mbs first
    char *cpData;//Must use fn_Mbs2Wcs or fn_Wcs2Mbs first
    //UA
    progressVar.UA = fn_Header_GetUA(httpRequest->Headers);
    //Proxy
    fn_Mbs2Wcs(httpRequest->Proxy.data(),wcpData,CP_ACP);
    fn_Assign_WString(progressVar.Proxy,wcpData);
    //Model
    fn_CharToUp(httpRequest->Model);
    fn_Mbs2Wcs(httpRequest->Model.data(),wcpData,CP_ACP);
    fn_Assign_WString(progressVar.Model,wcpData);
    //Url
    URL_COMPONENTS stUrl;
    fn_URL_Init(httpRequest->Url, stUrl);
    //Headers
    fn_Mbs2Wcs(httpRequest->Headers.data(),wcpData,CP_ACP);
    fn_Assign_WString(progressVar.Headers,wcpData);

    //ProcessVars
    DWORD dwNumberOfBytesToRead=0;


    //Winhttp
    HINTERNET   hSession,hConnect,hRequest;

    hSession=WinHttpOpen(progressVar.UA.c_str(),
                         progressVar.Proxy.empty() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_NAMED_PROXY,
                         progressVar.Proxy.empty() ? WINHTTP_NO_PROXY_NAME : progressVar.Proxy.c_str(),
                         progressVar.Proxy.empty() ? WINHTTP_NO_PROXY_BYPASS : nullptr,
                         0
    );
    if (hSession == nullptr){
        printf("WinHttpOpen Error,LastError %lX\r\n",GetLastError());
    }
    hConnect=WinHttpConnect(hSession,
                            stUrl.lpszHostName,
                            (stUrl.nPort==443||stUrl.nPort==80) ? INTERNET_DEFAULT_PORT : stUrl.nPort,
                            0
    );
    if (hConnect == nullptr){
        printf("WinHttpConnect Error,LastError %lX\r\n",GetLastError());
    }
    hRequest=WinHttpOpenRequest(hConnect,
                                progressVar.Model.c_str(),
                                stUrl.lpszUrlPath,
                                nullptr,
                                nullptr,
                                WINHTTP_DEFAULT_ACCEPT_TYPES,
                                WINHTTP_FLAG_SECURE
    );
    if (hRequest == nullptr){
        printf("WinHttpOpenRequest Error,LastError %lX\r\n",GetLastError());
    }
    DWORD dwSetOption=WINHTTP_DISABLE_REDIRECTS;
    if (!WinHttpSetOption(hRequest,
                          WINHTTP_OPTION_DISABLE_FEATURE,
                          &dwSetOption,
                          sizeof(dwSetOption)
    )){
        printf("WinHttpSetOption Error,LastError %lX\r\n",GetLastError());
    }
    if (!progressVar.Headers.empty()){
        if (!WinHttpAddRequestHeaders(hRequest,
                                      progressVar.Headers.c_str(),
                                      progressVar.Headers.length(),
                                      WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE
        )){
            printf("WinHttpAddRequestHeaders Error,LastError %lX\r\n",GetLastError());
        }
    }
    if (!WinHttpSendRequest(hRequest,
                            progressVar.Headers.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : progressVar.Headers.c_str(),
                            progressVar.Headers.length(),
                            httpRequest->Body.empty() ? WINHTTP_NO_REQUEST_DATA : httpRequest->Body.data(),
                            httpRequest->Body.length(),
                            httpRequest->Body.length(),
                            0
    )){
        printf("WinHttpSendRequest Error,LastError %lX\r\n",GetLastError());
    }
    if (!WinHttpReceiveResponse(hRequest,nullptr)){
        printf("WinHttpReceiveResponse Error,LastError %lX\r\n",GetLastError());
    }

    wchar_t *wcpBuffer = nullptr;
    DWORD bytesRead = 0;
    WinHttpQueryHeaders(hRequest,
                        WINHTTP_QUERY_RAW_HEADERS_CRLF,
                        nullptr,
                        nullptr,
                        &bytesRead,
                        nullptr
    );
    if (bytesRead > 0) {
        wcpBuffer = new wchar_t[bytesRead / sizeof(wchar_t)];
        WinHttpQueryHeaders(hRequest,
                            WINHTTP_QUERY_RAW_HEADERS_CRLF,
                            nullptr,
                            wcpBuffer,
                            &bytesRead,
                            nullptr
        );
    }
    //Get response headers
    fn_Wcs2Mbs(wcpBuffer,cpData,CP_ACP);
    fn_Assign_String(httpResponse.Headers,cpData);

    if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
        printf("WinHttpQueryDataAvailable Error,LastError %lX\r\n",GetLastError());
    }


    while (dwNumberOfBytesToRead > 0){
        DWORD dwRecv= 0;
        char *cpBuffer=new char[dwNumberOfBytesToRead+1];
        memset(cpBuffer, 0, dwNumberOfBytesToRead+1);
        if (!WinHttpReadData(hRequest, cpBuffer, dwNumberOfBytesToRead, &dwRecv)){
            printf("WinHttpReadData Error,LastError %lX\r\n",GetLastError());
        }
        if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
            printf("WinHttpQueryDataAvailable Error,LastError %lX\r\n",GetLastError());
        }
        httpResponse.Body.append(cpBuffer);
        delete []cpBuffer;
    }
    //closeHandles
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    free(stUrl.lpszHostName);
    return httpResponse;
}
