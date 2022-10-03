#include "ParamsProcess.h"

using std::string;
using std::map;

string ParamsProcess_GetUA(map<string,string> &Headers){
    string UA = "LogStatistic";
    for (const auto& ch: Headers) {
        if (ch.first == "User-Agent")
            return ch.second;
    }
    return UA;
}
URL_COMPONENTS ParamsProcess_InitUrlComponents(const string &Url){
    URL_COMPONENTS UrlComponents;
    memset(&UrlComponents,0,sizeof(URL_COMPONENTS));
    UrlComponents.dwStructSize = sizeof(URL_COMPONENTS);
    UrlComponents.dwSchemeLength    = -1;
    UrlComponents.dwHostNameLength  = -1;
    UrlComponents.dwUrlPathLength   = -1;
    UrlComponents.dwExtraInfoLength = -1;
    return UrlComponents;
}