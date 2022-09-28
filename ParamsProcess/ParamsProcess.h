#ifndef WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
#define WINHTTP_SIMPLEAPI_PARAMSPROCESS_H

#include <string>
#include <map>
#include "windows.h"
#include "winhttp.h"

extern std::string ParamsProcess_GetUA(std::map<std::string,std::string> &Headers);
extern URL_COMPONENTS ParamsProcess_GetUrlComponents(const std::string &Url);

#endif //WINHTTP_SIMPLEAPI_PARAMSPROCESS_H
