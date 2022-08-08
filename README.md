# A simple secondary wrapper for WinHttp

## Usage:
```
stHttpResponse httpResponse;
stHttpRequest httpRequest;
httpRequest.Url = "https://www.baidu.com";
httpRequest.Model = "get";
httpRequest.TimeOut = 3000;//ms
Winhttp_Request(httpRequest,httpResponse);
printf("%s\n",httpResponse.Body.c_str());
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
