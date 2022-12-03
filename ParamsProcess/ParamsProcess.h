#ifndef WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
#define WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
#include <string>
#include <map>
#include "windows.h"
#include "winhttp.h"

class ParamProcess {
public:
    static std::string GetUA(std::map<std::string, std::string> &Headers);
    static URL_COMPONENTS InitUrlComponents(const std::string &Url);
};


#endif //WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
