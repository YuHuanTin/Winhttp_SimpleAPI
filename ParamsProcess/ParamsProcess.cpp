#include "ParamsProcess.h"

using std::string;
using std::map;

string ParamProcess::GetUA(map<string, string> &Headers) {
    string szUA = "LogStatistic";
    for (const auto &ch: Headers) {
        if (ch.first == "User-Agent")
            return ch.second;
    }
    return szUA;
}
URL_COMPONENTS ParamProcess::InitUrlComponents(const string &Url) {
    URL_COMPONENTS UrlComponents;
    memset(&UrlComponents,0,sizeof(URL_COMPONENTS));
    UrlComponents.dwStructSize = sizeof(URL_COMPONENTS);
    UrlComponents.dwSchemeLength    = -1;
    UrlComponents.dwHostNameLength  = -1;
    UrlComponents.dwUrlPathLength   = -1;
    UrlComponents.dwExtraInfoLength = -1;
    return UrlComponents;
}