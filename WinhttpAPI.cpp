#include "WinhttpAPI.h"
char *g_szRetHandles;
wchar_t *fn_mbstowcs(char *cpStr){
    size_t len=MultiByteToWideChar(CP_ACP,NULL,cpStr,-1,nullptr,0);
    auto *retStr=new wchar_t [len];
    MultiByteToWideChar(CP_ACP,NULL,cpStr,-1,retStr,(int)len);
    return retStr;
}
char *fn_wcstombs(wchar_t *wcpStr){
    size_t len=WideCharToMultiByte(CP_ACP,NULL,wcpStr, -1, nullptr, 0,nullptr,nullptr);
    char *retStr=new char[len];
    WideCharToMultiByte(CP_ACP,NULL,wcpStr,-1,retStr,(int)len,nullptr,nullptr);
    return retStr;
}
char *fn_encoding(char *cpData,unsigned CodePage_before,unsigned CodePage_after){//to CN
    DWORD dwLen=MultiByteToWideChar(CodePage_before, NULL, cpData, -1, nullptr, 0);
    auto *wcpData=new wchar_t [dwLen];
    MultiByteToWideChar(CodePage_after, NULL, cpData, -1, wcpData, (int)dwLen);
    return fn_wcstombs(wcpData);
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

    wchar_t *wszUrl=fn_mbstowcs(szUrl.data());
    if (!WinHttpCrackUrl(wszUrl, wcslen(wszUrl),0,&url)){
        printf("WinHttpCrackUrl Error,LastError %lX\r\n",GetLastError());
    }
    auto *wszHostName= new wchar_t [url.dwHostNameLength+1];
    wmemset(wszHostName,0,url.dwHostNameLength+1);
    wmemcpy(wszHostName,url.lpszHostName,url.dwHostNameLength);
    url.lpszHostName=wszHostName;
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
char *Winhttp_Request(char *inUrl,char *inModel, char *inBody, char *inHandles, char *inCookies, char *inProxy, unsigned uTimeout){
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

    hSession=WinHttpOpen(pwszUA, szProxy.empty() ? WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY : WINHTTP_ACCESS_TYPE_NAMED_PROXY, szProxy.empty() ? nullptr : fn_mbstowcs(szProxy.data()), nullptr, 0);
    if (hSession == nullptr){
        printf("WinHttpOpen Error,LastError %lX\r\n",GetLastError());
    }
    hConnect=WinHttpConnect(hSession, url.lpszHostName,(url.nPort==443||url.nPort==80)?INTERNET_DEFAULT_PORT:url.nPort,NULL);
    if (hConnect == nullptr){
        printf("WinHttpConnect Error,LastError %lX\r\n",GetLastError());
    }
    hRequest=WinHttpOpenRequest(hConnect, fn_mbstowcs(szModel.data()), url.lpszUrlPath, nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, NULL);
    if (hRequest == nullptr){
        printf("WinHttpOpenRequest Error,LastError %lX\r\n",GetLastError());
    }
    if (!szHandles.empty()){
        if (!WinHttpAddRequestHeaders(hRequest, fn_mbstowcs(szHandles.data()),szHandles.length(),WINHTTP_ADDREQ_FLAG_ADD|WINHTTP_ADDREQ_FLAG_REPLACE)){
            printf("WinHttpAddRequestHeaders Error,LastError %lX\r\n",GetLastError());
        }
    }
    if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, szBody.data(), szBody.empty() ? NULL : (DWORD)szBody.length(), szBody.length(), 0)){
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
