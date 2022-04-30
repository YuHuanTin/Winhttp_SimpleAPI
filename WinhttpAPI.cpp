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
char *fn_wcstombs(wchar_t *wcpStr){
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
    memset(mHost,0x0,uAllocHostLen+1);
    memcpy_s(mHost,uAllocHostLen,&strUrl[uPosFirst+2],uAllocHostLen);
    char *mPath=new char[uAllocPathLen];
    memset(mPath, 0x0, uAllocPathLen+1);
    memcpy_s(mPath, uAllocPathLen, &strUrl[uPosNext+1], uAllocPathLen);

    Url.szHost=mHost;
    Url.szUrlPath=mPath;
    return Url;
}


char *Winhttp_Request(char *szUrl,char *szModel,vector<char *>szHandles,char *szCookies,char *szProxy,unsigned uTimeout){
    //UA part
    char *pszUA;
    pszUA="Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/86.0.4240.198 Safari/537.36";
    wchar_t *pwszUA=fn_mbstowcs(pszUA);

    //URL part
    stUrl Url=fn_initURL(szUrl);


    //Handle Part
    HINTERNET   hSession,
                hConnect;

    hSession=WinHttpOpen(pwszUA, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0);
    if (hSession == NULL){
        printf("WinHttpOpen Error,LastError%lx",GetLastError());
    }
//    hConnect=WinHttpConnect(hSession,);





    //clean all var at last
    delete pszUA;
    delete pwszUA;
    return nullptr;
    //printf("%s\r\n%s\r\n%d\r\n-------------------------------------------------------------\r\n",Url.szHost,Url.szUrlPath,Url.uPort);
}

