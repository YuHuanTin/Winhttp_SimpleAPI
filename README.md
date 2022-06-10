# 简单的Winhttp接口
使用Winhttp_Request连接和发送不同类型的请求

## 使用方法:
### 静态库
##### file:
```
#include "..\\\WinhttpAPI.h"
```

##### cmakelist:
```
link_directories(lib) #the directory where the libs are stored, as defined by you. like ..\\\lib
link_libraries(Winhttp_SimpleAPI.lib)
```
#### or
##### file:
```
#include "..\\\WinhttpAPI.h"
#pragma comment(lib,"..\\\Winhttp_SimpleAPI.lib")
```
### 动态库
##### file:
```
#include "..\\\WinhttpAPI.h" //ignore now

typedef char *(*Winhttp_Request)(char *inUrl,char *inModel, char *inBody, char *inHandles, char *inCookies, char *inProxy, unsigned uTimeout);
HMODULE lib=LoadLibraryA("Winhttp_SimpleAPI.dll"); //the absolute location of the dll or the current directory(just "Winhttp_SimpleAPI.dll")
Winhttp_Request proc=(Winhttp_Request)GetProcAddress(lib,"Winhttp_Request");
```
