cmake_minimum_required(VERSION 3.21)
project(Winhttp_SimpleAPI)

set(CMAKE_CXX_STANDARD 17)

set(LINK_FILES
        WinhttpAPI.h
        WinhttpAPI.cpp
        CodeConvert/CodeCvt.h
        DataStruct/DataStruct.h
        ParamsProcess/ParamsProcess.h
        WinHttpInterface/APIInterface.h
        WinHttpInterface/APIInterface.cpp)

#add_library(Winhttp_SimpleAPI SHARED ${LINK_FILES})#SHARED (dll)
add_library(Winhttp_SimpleAPI STATIC ${LINK_FILES})#STATIC (lib)

