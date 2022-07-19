#include "WinhttpAPI.h"
char *g_szRetHandles;

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

void fn_CharToUp(string &szData){
    for (auto &ch:szData) {
        ch= (char)toupper(ch);
    }
}
void fn_initURL(string szUrl,URL_COMPONENTS &url){
    memset(&url,0, sizeof(url));
    url.dwStructSize = sizeof(url);

    url.dwSchemeLength    = (DWORD)-1;
    url.dwHostNameLength  = (DWORD)-1;
    url.dwUrlPathLength   = (DWORD)-1;
    url.dwExtraInfoLength = (DWORD)-1;

    wchar_t *wcpUrl;
    fn_Mbs2Wcs(szUrl.data(), wcpUrl, CP_ACP);
    if (!WinHttpCrackUrl(wcpUrl, wcslen(wcpUrl), 0, &url)){
        printf("WinHttpCrackUrl Error,LastError %lX\r\n",GetLastError());
    }
    auto *wszHostName= new wchar_t [url.dwHostNameLength+1];
    wmemset(wszHostName,0,url.dwHostNameLength+1);
    wmemcpy(wszHostName,url.lpszHostName,url.dwHostNameLength);
    url.lpszHostName=wszHostName;
    free(wcpUrl);
}
string fn_GetHandleUA(string retUA){
    if (retUA.find("User-Agent") != string::npos) {
        char *buf;
        string ch = strtok_s(retUA.data(), "\r\n", &buf);
        while (strlen(buf)>=0){
            if (ch.find("User-Agent")!=string::npos){
                retUA=ch.substr(ch.find(':')+1, string::npos);
                break;
            }
            ch = strtok_s(nullptr, "\r\n",&buf);
        }
    } else{
        retUA="LogStatistic";
    }
    return retUA;
}
char *Winhttp_Request(const char *inUrl,const char *inModel,const char *inBody,const char *inHandles,const char *inCookies,const char *inProxy, unsigned uTimeout){
    //reInitVar
    string  szUrl{inUrl== nullptr?"":inUrl},
            szModel{inModel== nullptr?"":inModel},
            szBody{inBody== nullptr?"":inBody},
            szHandles{inHandles== nullptr?"":inHandles},
            szCookie{inCookies== nullptr?"":inCookies},
            szProxy{inProxy== nullptr?"":inProxy},
            szUA{fn_GetHandleUA(szHandles)};

    //prepare
    wchar_t *pwszUA=fn_mbstowcs(szUA.data());
    fn_CharToUp(szModel);
    URL_COMPONENTS url;
    DWORD dwNumberOfBytesToRead=0;

    //url´¦Àí
    fn_initURL(szUrl, url);

    //Winhttp
    HINTERNET   hSession,
                hConnect,
                hRequest;

    hSession=WinHttpOpen(pwszUA, szProxy.empty() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_NAMED_PROXY, szProxy.empty() ? WINHTTP_NO_PROXY_NAME : fn_mbstowcs(szProxy.data()), szProxy.empty() ? WINHTTP_NO_PROXY_BYPASS : nullptr, 0);
    if (hSession == nullptr){
        printf("WinHttpOpen Error,LastError %lX\r\n",GetLastError());
    }
    hConnect=WinHttpConnect(hSession, url.lpszHostName,(url.nPort==443||url.nPort==80) ? INTERNET_DEFAULT_PORT : url.nPort,0);
    if (hConnect == nullptr){
        printf("WinHttpConnect Error,LastError %lX\r\n",GetLastError());
    }
    hRequest=WinHttpOpenRequest(hConnect, fn_mbstowcs(szModel.data()), url.lpszUrlPath, nullptr, nullptr, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (hRequest == nullptr){
        printf("WinHttpOpenRequest Error,LastError %lX\r\n",GetLastError());
    }
    DWORD dwSetOption=WINHTTP_DISABLE_REDIRECTS;
    if (!WinHttpSetOption(hRequest,WINHTTP_OPTION_DISABLE_FEATURE,&dwSetOption, sizeof(dwSetOption))){
        printf("WinHttpSetOption Error,LastError %lX\r\n",GetLastError());

    }
    if (!szHandles.empty()){
        if (!WinHttpAddRequestHeaders(hRequest, fn_mbstowcs(szHandles.data()),szHandles.length(),WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE)){
            printf("WinHttpAddRequestHeaders Error,LastError %lX\r\n",GetLastError());
        }
    }
    if (!WinHttpSendRequest(hRequest,szHandles.empty() ? WINHTTP_NO_ADDITIONAL_HEADERS : fn_mbstowcs(szHandles.data()),szHandles.length(),szBody.empty() ? WINHTTP_NO_REQUEST_DATA : szBody.data(),szBody.length(),szBody.length(),0)){
        printf("WinHttpSendRequest Error,LastError %lX\r\n",GetLastError());
    }
    if (!WinHttpReceiveResponse(hRequest,nullptr)){
        printf("WinHttpReceiveResponse Error,LastError %lX\r\n",GetLastError());
    }

    wchar_t *wcpBuffer = nullptr;
    DWORD bytesRead = 0;
    WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, nullptr, nullptr, &bytesRead, nullptr);
    if (bytesRead > 0) {
        wcpBuffer = new wchar_t[bytesRead / sizeof(wchar_t)];
        WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF, nullptr, wcpBuffer, &bytesRead, nullptr);
    }
    g_szRetHandles=fn_wcstombs(wcpBuffer);

    if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
        printf("WinHttpQueryDataAvailable Error,LastError %lX\r\n",GetLastError());
    }

    string szRecvData;
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
        szRecvData+=cpBuffer;
        delete []cpBuffer;
    }

    char *retData=new char[szRecvData.length() + 1];
    memset(retData,0,szRecvData.length() + 1);
    memcpy(retData,szRecvData.data(), szRecvData.length()+1);

    //clean all var at last
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    szUrl.clear();
    szModel.clear();
    szBody.clear();
    szHandles.clear();
    szCookie.clear();
    szProxy.clear();
    szUA.clear();
    szRecvData.clear();
    delete[] wcpBuffer;
    delete[] pwszUA;
    return retData;
}
