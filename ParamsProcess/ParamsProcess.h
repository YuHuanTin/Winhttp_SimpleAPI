#ifndef WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
#define WINHTTP_SIMPLEAPI_PARAMSPROCESS_H

#include "../DataStruct/DataStruct.h"

namespace ParamProcess {
    std::string GetUA(std::map<std::string, std::string> &Headers) {
        std::string     szUA = "LogStatistic";
        for (const auto &ch: Headers) {
            if (ch.first == "User-Agent")
                return ch.second;
        }
        return szUA;
    }
    URL_COMPONENTS InitUrlComponents() {
        URL_COMPONENTS UrlComponents;
        memset(&UrlComponents, 0, sizeof(URL_COMPONENTS));
        UrlComponents.dwStructSize      = sizeof(URL_COMPONENTS);
        UrlComponents.dwSchemeLength    = -1;
        UrlComponents.dwHostNameLength  = -1;
        UrlComponents.dwUrlPathLength   = -1;
        UrlComponents.dwExtraInfoLength = -1;
        return UrlComponents;
    }
}

#endif //WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
