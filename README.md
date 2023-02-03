# A simple secondary wrapper for WinHttp

![image](https://learn.microsoft.com/en-us/windows/win32/winhttp/images/art-winhttp3.png)
## Import Methods:
##### CMakeLists.txt:
```c++
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/Winhttp_SimpleAPI.lib)
```
##### main.cpp
```c++
#include <WinhttpAPI.h>
```
##### Or main.cpp:
```c++
#include "..\\WinhttpAPI.h"
#pragma comment(lib,"..\\Winhttp_SimpleAPI.lib")
```
## Usage:
##### Request a website information
```c++
HttpRequestT httpRequest;
HttpResponseT httpResponse;
httpRequest.url = "https://www.baidu.com";
httpRequest.protocol = "get";

WinhttpAPI httpAPI;
httpAPI.SetHeader("Connection","keep-alive");
httpAPI.SetHeader("Context-Type","text/html");
httpAPI.Request(httpRequest,httpResponse);

//Get the content and headers returned from a web visit.
std::cout << "[+]" << httpResponse.body << ", " << httpResponse.headers << '\n';
//Retrieve a specific key value from the returned headers
std::cout << "[+]" << httpAPI.GetHeader("Date") << '\n';
```
##### Download a file
```c++
HttpRequestT httpRequest = {"https://example.com/1GB.bin", "get", {HttpRequestT::SaveMethodT::FILE_STREAM, "C:\\test.bin"}};
HttpResponseT httpResponse;

WinhttpAPI httpAPI(httpRequest,httpResponse);
httpAPI.request();
//At the same time,the headers exist and the body has empty
```

## Thanks:
[![Licenses for Open Source Development](https://img.shields.io/badge/JetBrains-Open%20Source%20License-white?logo=JetBrains&style=plastic)](https://www.jetbrains.com/community/opensource/#support)
