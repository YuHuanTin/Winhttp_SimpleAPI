#include "WinhttpAPI.h"

struct WinhttpSetting{

}stWinhttpSettings;

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
    string str="你好世界";
    char *b= new char[str.length()];
    memcpy(b,&str,NULL);
    wchar_t *a=fn_mbstowcs(b);
    cout<<endl;
    return true;

}
char *Winhttp_Get(char *szUrl,vector<char *>szHandles,char *szCookies,char *szProxy,unsigned uTimeout){
    initWinhttp();
    return nullptr;
}
char *Winhttp_Post(){

    return nullptr;
}

