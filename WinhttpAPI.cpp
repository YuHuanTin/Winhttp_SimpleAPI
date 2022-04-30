#include "WinhttpAPI.h"

struct stWinhttpSetting{

}_WinhttpSettings;

//szUrl结构体,将此文本拆分
struct stUrl{
    char *szHost= nullptr;
    unsigned uPort=80;
    char *szUrlPath= nullptr;
};
//函数部分
wchar_t *fn_mbstowcs(char *cpStr){
    size_t len=0;
    mbstowcs_s(&len, nullptr,NULL,cpStr,_TRUNCATE);
    wchar_t *retStr=new wchar_t [len];
    mbstowcs_s(nullptr,retStr,len,cpStr,_TRUNCATE);
    return retStr;
}
char *fn_wcstombs(wchar_t *wcpStr){//may have some problem
    size_t len=0;
    wcstombs_s(&len, nullptr,NULL,wcpStr,_TRUNCATE);
    char *retStr=new char[len];
    wcstombs_s(nullptr,retStr,len,wcpStr,_TRUNCATE);
    return retStr;
}
bool initWinhttp(){

    return true;
}
//  分割URL结构
stUrl fn_initURL(char *szUrl){
    stUrl   Url;
    string  strUrl=szUrl;
    size_t  uPosFirst=strUrl.find("//",0),
            uPosNext=strUrl.find('/',uPosFirst+2),
            uPosPort=strUrl.find(':', uPosFirst + 2),
            uAllocHostLen=0,
            uAllocPathLen=0;

    if (uPosPort != string::npos){
        Url.uPort= strtoul(strUrl.substr(uPosPort+1,uPosNext).data(), nullptr,0);
        uAllocHostLen=uPosPort-uPosFirst-2;
    } else{
        uAllocHostLen=uPosNext-uPosFirst-2;
    }
    if (uPosNext != string::npos){
        uAllocPathLen= strlen(szUrl)-uPosNext-1;
    } else{
        uAllocPathLen=0;
        uPosNext=0;
    }
    char *mHost=new char[uAllocHostLen];
    memset(mHost,0,uAllocHostLen+1);
    memcpy_s(mHost,uAllocHostLen,&strUrl[uPosFirst+2],uAllocHostLen);
    char *mPath=new char[uAllocPathLen];
    memset(mPath, 0, uAllocPathLen+1);
    memcpy_s(mPath, uAllocPathLen, &strUrl[uPosNext+1], uAllocPathLen);

    Url.szHost=mHost;
    Url.szUrlPath=mPath;
    return Url;
}


char *Winhttp_Request(char *szUrl,char *szModel,vector<char *>szHandles,char *szCookies,char *szProxy,unsigned uTimeout){
    //prepare

    //_strupr_s(szModel, strlen(szModel));//到大写
    DWORD dwNumberOfBytesToRead=0;


    //UA part
    char *pszUA;
    pszUA="Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36";
    wchar_t *pwszUA=fn_mbstowcs(pszUA);

    //URL part
    stUrl Url=fn_initURL(szUrl);


    //Handle Part
    HINTERNET   hSession,
                hConnect,
                hRequest;

    hSession=WinHttpOpen(pwszUA, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0);
    if (hSession == nullptr){
        printf("WinHttpOpen Error,LastError%lx",GetLastError());
    }
    hConnect=WinHttpConnect(hSession, fn_mbstowcs(Url.szHost),(INTERNET_PORT)Url.uPort,NULL);
    if (hConnect == nullptr){
        printf("WinHttpConnect Error,LastError%lx",GetLastError());
    }
    hRequest=WinHttpOpenRequest(hConnect, fn_mbstowcs(szModel), fn_mbstowcs(Url.szUrlPath),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,NULL);
    if (hRequest == nullptr){
        printf("WinHttpOpenRequest Error,LastError%lx",GetLastError());
    }
    if (!WinHttpSendRequest(hRequest,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0)){
        printf("WinHttpSendRequest Error,LastError%lx",GetLastError());
    }
    if (!WinHttpReceiveResponse(hRequest,nullptr)){
        printf("WinHttpReceiveResponse Error,LastError%lx",GetLastError());
    }
    if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
        printf("WinHttpQueryDataAvailable Error,LastError%lx",GetLastError());
    }
    char *cpRecvData= new char[dwNumberOfBytesToRead+1];
    memset(cpRecvData,0,dwNumberOfBytesToRead+1);
    while (dwNumberOfBytesToRead > 0){
        DWORD dwRecv= 0;
        char *cpBuffer=new char[dwNumberOfBytesToRead+1];
        memset(cpBuffer, 0, dwNumberOfBytesToRead+1);

        if (!WinHttpReadData(hRequest, cpBuffer, dwNumberOfBytesToRead, &dwRecv)){
            printf("WinHttpReadData Error,LastError%lx",GetLastError());
        }
        if (!WinHttpQueryDataAvailable(hRequest,&dwNumberOfBytesToRead)){
            printf("WinHttpQueryDataAvailable Error,LastError%lx",GetLastError());
        }
        //返回原文长度
        DWORD dwRawData=MultiByteToWideChar(CP_ACP, NULL, cpBuffer, -1, nullptr, 0);
        wchar_t *wcpConvertData=new wchar_t [dwRawData];
        MultiByteToWideChar(CP_UTF8, 0, cpBuffer, -1, wcpConvertData, dwRawData);
        cpBuffer= fn_wcstombs(wcpConvertData);
        strcat_s(cpRecvData,dwRawData,cpBuffer);
        delete[] cpBuffer;
        delete[] wcpConvertData;
    }

    //clean all var at last
    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return cpRecvData;
}

