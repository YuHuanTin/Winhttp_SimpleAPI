cmake_minimum_required(VERSION 3.21)
project(Winhttp_SimpleAPI)

set(CMAKE_CXX_STANDARD 17)

set(LINK_FILES
        WinhttpAPI.h
        WinhttpAPI.cpp
        WinHttpInterface/APIInterface.h
        WinHttpInterface/APIinterface.cpp
        CodeConvert/CodeCvt.cpp
        CodeConvert/CodeCvt.h ParamsProcess/ParamsProcess.cpp ParamsProcess/ParamsProcess.h)

#add_library(Winhttp_SimpleAPI SHARED ${LINK_FILES})#SHARED (dll)
add_library(Winhttp_SimpleAPI STATIC ${LINK_FILES})#STATIC (lib)
