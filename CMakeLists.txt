cmake_minimum_required(VERSION 3.21)
project(Winhttp_SimpleAPI)

# 设置 Cmake 包安装目录
foreach (INSTALL_PATH ${CMAKE_PREFIX_PATH})
    string(TOLOWER ${INSTALL_PATH} INSTALL_PATH_TOLOWER)
    string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_TOLOWER)
    
    string(FIND ${INSTALL_PATH_TOLOWER} "debug" INSTALL_PATH_IS_DEBUG)
    string(FIND ${BUILD_TYPE_TOLOWER} "debug" BUILD_TYPE_IS_DEBUG)

    if (NOT INSTALL_PATH_IS_DEBUG EQUAL -1 AND NOT BUILD_TYPE_IS_DEBUG EQUAL -1)
        set(CMAKE_INSTALL_PREFIX ${INSTALL_PATH})
        message("install_path is debug & build_type is debug\n${INSTALL_PATH_TOLOWER}\n${BUILD_TYPE_TOLOWER}")
        message("set install_path up ${CMAKE_INSTALL_PREFIX}")
    elseif (INSTALL_PATH_IS_DEBUG EQUAL -1 AND BUILD_TYPE_IS_DEBUG EQUAL -1)
        set(CMAKE_INSTALL_PREFIX ${INSTALL_PATH})
        message("install_path is release & build_type is release\n${INSTALL_PATH_TOLOWER}\n${BUILD_TYPE_TOLOWER}")
        message("set install_path up ${CMAKE_INSTALL_PREFIX}")
    endif ()
endforeach ()

set(CMAKE_CXX_STANDARD 17)

# 设置 install include 的文件
set(Winhttp_SimpleAPI_INCLUDES ${CMAKE_SOURCE_DIR}/WinhttpAPI.h)

# 设置编译所需头文件
set(Winhttp_SimpleAPI_HEADERS
        WinhttpAPI.h
        CodeConvert/CodeCvt.h
        DataStruct/DataStruct.h
        ParamsProcess/ParamsProcess.h
        WinHttpInterface/APIInterface.h
        )
# 设置编译所需源文件
set(Winhttp_SimpleAPI_SOURCES
        WinhttpAPI.cpp
        WinHttpInterface/APIInterface.cpp
        )

add_library(Winhttp_SimpleAPI STATIC ${Winhttp_SimpleAPI_HEADERS} ${Winhttp_SimpleAPI_SOURCES})

# 默认安装位置为 ${CMAKE_INSTALL_PREFIX}/<DESTINATION 定义的路径>
install(TARGETS Winhttp_SimpleAPI EXPORT FindWinhttp_SimpleAPI
        RUNTIME DESTINATION bin # 二进制文件存放位置
        LIBRARY DESTINATION bin # 动态库存放位置
        ARCHIVE DESTINATION lib # 静态库存放位置
        )

install(FILES ${Winhttp_SimpleAPI_INCLUDES}
        DESTINATION include
        )

#FIND_PATH(Winhttp_SimpleAPI_INCLUDE_DIR
#        include/WinhttpAPI.h
#        )
#FIND_LIBRARY(Winhttp_SimpleAPI_LIBRARY
#        lib/Winhttp_SimpleAPI.lib
#        )
#IF (Winhttp_SimpleAPI_INCLUDE_DIR AND Winhttp_SimpleAPI_LIBRARY)
#    SET(Winhttp_SimpleAPI_FOUND TRUE)
#ENDIF (Winhttp_SimpleAPI_INCLUDE_DIR AND Winhttp_SimpleAPI_LIBRARY)
#message(${Winhttp_SimpleAPI_INCLUDE_DIR})
#message(${Winhttp_SimpleAPI_LIBRARY})
# 生成 findxxx.cmake
#install(EXPORT FindWinhttp_SimpleAPI
#        DESTINATION ${CMAKE_ROOT}/Modules)