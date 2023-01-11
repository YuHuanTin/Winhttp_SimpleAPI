# A simple secondary wrapper for WinHttp

![image](https://learn.microsoft.com/en-us/windows/win32/winhttp/images/art-winhttp3.png)
## Import Methods:
##### main.cpp
```c++
#include "..\\WinhttpAPI.h"
```
##### CMakeLists.txt:
```c++
link_directories(lib) #the directory where the libs are stored, as defined by you. like ..\\lib
link_libraries(Winhttp_SimpleAPI.lib)
```
##### Or main.cpp:
```c++
#include "..\\WinhttpAPI.h"
#pragma comment(lib,"..\\Winhttp_SimpleAPI.lib")
```
## Usage:
##### Request a website information
```c++
stHttpRequest httpRequest;
stHttpResponse httpResponse;
httpRequest.Url = "https://www.baidu.com";
httpRequest.Model = "get";

cWinHttpAPI httpAPI;
httpAPI.SetHeader(httpRequest,"Connection","keep-alive");
httpAPI.SetHeader(httpRequest,"Context-Type","text/html");
httpAPI.Request(httpRequest,httpResponse);

//Get the content and headers returned from a web visit.
printf("[+]%s,%s\n",httpResponse.Body.c_str(),httpResponse.Headers.c_str());
//Retrieve a specific key value from the returned Headers
printf("[x]%s\n",httpAPI.GetHeader("Date").c_str());
```
##### Request a large file
```c++
stHttpRequest httpRequest;
stHttpResponse httpResponse;
httpRequest.Url = "http://test.com/5Gb.bin";//example file url
httpRequest.Model = "get";
httpRequest.SaveMethod = {stHttpRequest::stSaveMethod::FILE_STREAM, "C:\\5.bin"};

cWinHttpAPI httpAPI(httpRequest,httpResponse);
httpAPI.request();
//At the same time,the headers exist and the body has empty
```

## Thanks:
[![Licenses for Open Source Development](https://img.shields.io/badge/JetBrains-Open%20Source%20License-white?logo=JetBrains&style=plastic)](https://www.jetbrains.com/community/opensource/#support)
