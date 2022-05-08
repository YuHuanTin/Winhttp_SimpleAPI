#include "WinhttpAPI.h"

struct stWinhttpSetting{

}_WinhttpSettings;

//函数部分
wchar_t *fn_mbstowcs(char *cpStr){
    size_t len=0;
    mbstowcs_s(&len, nullptr,NULL,cpStr,_TRUNCATE);
    wchar_t *retStr=new wchar_t [len];
    mbstowcs_s(nullptr,retStr,len,cpStr,_TRUNCATE);
    return retStr;
}
char *fn_wcstombs(wchar_t *wcpStr)  {
    size_t len=NULL;
    len=WideCharToMultiByte(CP_ACP,NULL,wcpStr, -1, nullptr, 0,nullptr,nullptr);
    char *retStr=new char[len];
    WideCharToMultiByte(CP_ACP,NULL,wcpStr,-1,retStr,(int)len,nullptr,nullptr);
    return retStr;
}
char *fn_encoding(char *cpData){//to CN
    DWORD dwLen=MultiByteToWideChar(CP_ACP, NULL, cpData, -1, nullptr, 0);
    wchar_t *wcpData=new wchar_t [dwLen];
    MultiByteToWideChar(CP_UTF8, 0, cpData, -1, wcpData, (int)dwLen);
    return fn_wcstombs(wcpData);
}
void fn_char_up(char * &cprData){
    char *tmpConvert=new char[strlen(cprData) + 1];
    memset(tmpConvert, 0, strlen(cprData) + 1);
    memcpy(tmpConvert, cprData, strlen(cprData));
    for (size_t i = 0; i < strlen(tmpConvert); ++i) {
        tmpConvert[i]=toupper(tmpConvert[i]);
    }
    cprData=tmpConvert;
}
bool initWinhttp(){

    return true;
}
//  分割URL结构
void fn_initURL(char *szUrl,URL_COMPONENTS &url){
    memset(&url,0, sizeof(url));
    url.dwStructSize = sizeof(url);

    url.dwSchemeLength    = (DWORD)-1;
    url.dwHostNameLength  = (DWORD)-1;
    url.dwUrlPathLength   = (DWORD)-1;
    url.dwExtraInfoLength = (DWORD)-1;

    wchar_t *wszUrl= nullptr;
    wszUrl=fn_mbstowcs(szUrl);
    WinHttpCrackUrl(wszUrl, wcslen(wszUrl),0,&url);
    printf("WinHttpCrackUrl Error,LastError %lx\r\n",GetLastError());
}

char *Winhttp_Request(char *szUrl,char *szModel,char *szBody,vector<char *>szHandles,char *szCookies,char *szProxy,unsigned uTimeout){
    //prepare
    fn_char_up(szModel);//转大写
    URL_COMPONENTS url;
    DWORD dwNumberOfBytesToRead=0;
    //url处理
    fn_initURL(szUrl, url);
    //协议头处理
    string szSumHandles;
    char *pszUA;
    pszUA="LogStatistic";
    for (auto ch:szHandles) {
        string tmpStr;
        tmpStr=ch;
        if (tmpStr.find("User-Agent")!=string::npos) {
            pszUA = ch + tmpStr.find(':') + 1;
        } else{
            szSumHandles+=tmpStr+"\r\n";
        }
    }
    wchar_t *pwszUA=fn_mbstowcs(pszUA);

    //Handle Part
    HINTERNET   hSession,
                hConnect,
                hRequest;

    hSession=WinHttpOpen(pwszUA, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0);
    if (hSession == nullptr){
        printf("WinHttpOpen Error,LastError %lx\r\n",GetLastError());
    }
    hConnect=WinHttpConnect(hSession, url.lpszHostName,url.nPort,NULL);
    if (hConnect == nullptr){
        printf("WinHttpConnect Error,LastError %lx\r\n",GetLastError());
    }
    hRequest=WinHttpOpenRequest(hConnect, fn_mbstowcs(szModel), url.lpszUrlPath,nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,NULL);
    if (hRequest == nullptr){
        printf("WinHttpOpenRequest Error,LastError %lx\r\n",GetLastError());
    }
    if (!WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0)){
        printf("WinHttpSendRequest Error,LastError %lx\r\n",GetLastError());
    }
    if (!WinHttpReceiveResponse(hRequest,nullptr)){
        printf("WinHttpReceiveResponse Error,LastError %lx\r\n",GetLastError());
    }
    if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
        printf("WinHttpQueryDataAvailable Error,LastError %lx\r\n",GetLastError());
    }

    string szRecvData;
    while (dwNumberOfBytesToRead > 0){
        DWORD dwRecv= 0;
        char *cpBuffer=new char[dwNumberOfBytesToRead+1];
        memset(cpBuffer, 0, dwNumberOfBytesToRead+1);
        if (!WinHttpReadData(hRequest, cpBuffer, dwNumberOfBytesToRead, &dwRecv)){
            printf("WinHttpReadData Error,LastError %lx\r\n",GetLastError());
        }
        if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
            printf("WinHttpQueryDataAvailable Error,LastError %lx\r\n",GetLastError());
        }
        szRecvData+=cpBuffer;
        delete []cpBuffer;
    }
    char *cpRecvData=fn_encoding(szRecvData.data());


    //clean all var at last
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return cpRecvData;
}

