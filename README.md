# A simple secondary wrapper for WinHttp

## Usage:
```
stHttpResponse httpResponse;
stHttpRequest httpRequest;
httpRequest.Url = "https://www.baidu.com";
httpRequest.Model = "get";
httpRequest.TimeOut = 3000;//ms
cWinhttpAPI::SetHeaders(httpRequest,"Connection","keep-alive");
cWinhttpAPI::SetHeaders(httpRequest,"Context-Type","text/html");
cWinhttpAPI::Request(httpRequest,httpResponse);

//Get the content and headers returned from a web visit.
printf("[+]%s,%s\n",httpResponse.Body.c_str(),httpResponse.allHeaders.c_str());
//Retrieve a specific key value from the returned Headers
printf("[x]%s\n",cWinhttpAPI::GetHeaders(httpResponse,"Date").c_str());
```

## Import Methods:
##### must have
```
#include "..\\WinhttpAPI.h"
```
##### cmakelist:
```
link_directories(lib) #the directory where the libs are stored, as defined by you. like ..\\lib
link_libraries(Winhttp_SimpleAPI.lib)
```
##### or file:
```
#pragma comment(lib,"..\\Winhttp_SimpleAPI.lib")
```
## Thanks:
[![Licenses for Open Source Development](https://img.shields.io/badge/JetBrains-Open%20Source%20License-white?logo=JetBrains&style=plastic)](https://www.jetbrains.com/community/opensource/#support)
