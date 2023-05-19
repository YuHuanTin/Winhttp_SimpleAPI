# A simple secondary wrapper for WinHttp

![image](https://learn.microsoft.com/en-us/windows/win32/winhttp/images/art-winhttp3.png)
## Import Methods:
1. ### cmake + source
   ##### CMakeLists.txt
    ```cmake
    include_directories(${PROJECT_SOURCE_DIR}/include)
    add_executable(${PROJECT_NAME} main.cpp)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/Winhttp_SimpleAPI.lib)
    ```
    ##### main.cpp
    ```c++
    #include <WinhttpAPI.h>
    ```
2. ### source only
   ##### main.cpp
    ```c++
    #include "..\\WinhttpAPI.h"
    #pragma comment(lib,"..\\Winhttp_SimpleAPI.lib")
    ```
3. ### cmake install + source
   enter this cmakelist.txt and use cmake install

   if you want set install path, please set 'CMAKE_INSTALL_PREFIX'
   #### your project cmakelist.txt
   ```cmake
    if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(CMAKE_INSTALL_PREFIX "D:/debug")
    elseif (${CMAKE_BUILD_TYPE} STREQUAL "Release")
        set(CMAKE_INSTALL_PREFIX "D:/")
    endif ()

    include_directories(${CMAKE_INSTALL_PREFIX}/include)
    add_executable(${PROJECT_NAME} main.cpp)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_INSTALL_PREFIX}/lib/Winhttp_SimpleAPI.lib)
       
   ```
   ##### main.cpp
   ```c++
   #include <WinhttpAPI.h>
   ```


## Usage:
##### First step, import namespace
```c++
using namespace Winhttp;
```
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
##### Other
- use encode/decode function
    ```c++
    // transform string to wstring
    std::wstring wStr = CodeCvt::StrToWstr("hello world", CP_ACP);
    
    // transform wstring to string
    std::string str = CodeCvt::WstrToStr(L"hello world", CP_ACP);
    ```

## Thanks:
[![Licenses for Open Source Development](https://img.shields.io/badge/JetBrains-Open%20Source%20License-white?logo=JetBrains&style=plastic)](https://www.jetbrains.com/community/opensource/#support)
