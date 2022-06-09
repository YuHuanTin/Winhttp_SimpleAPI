# 简单的Winhttp接口
使用Winhttp_Request连接和发送不同类型的请求

###### ~~目前还不支持发送数据,例如POST等~~

# 使用方法:
### 静态库
file:
> #include "..\\WinhttpAPI.h"

cmakelist:
> link_directories(lib) #the directory where the libs are stored, as defined by you. like ..\\lib
> 
> link_libraries(Winhttp_SimpleAPI.lib)

### 动态库
file:
> #include "..\\WinhttpAPI.h"


