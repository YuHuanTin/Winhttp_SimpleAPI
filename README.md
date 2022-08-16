# A simple secondary wrapper for WinHttp

## Usage:
```
stHttpResponse httpResponse;
stHttpRequest httpRequest;
httpRequest.Url = "https://www.baidu.com";
httpRequest.Model = "get";
httpRequest.TimeOut = 3000;//ms
cWinHttpSimpleAPI::Winhttp_SetHeaders(httpRequest,"Connection","keep-alive");
cWinHttpSimpleAPI::Winhttp_SetHeaders(httpRequest,"Context-Type","text/html");
cWinHttpSimpleAPI::Winhttp_Request(httpRequest,httpResponse);

//Get the content and headers returned from a web visit.
printf("[+]%s,%s\n",httpResponse.Body.c_str(),httpResponse.allHeaders.c_str());
//Retrieve a specific key value from the returned Headers
printf("[x]%s\n",cWinHttpSimpleAPI::Winhttp_GetHeaders(httpResponse,"Date").c_str());
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
