cmake_minimum_required(VERSION 3.21)
project(Winhttp_SimpleAPI)

set(CMAKE_CXX_STANDARD 17)

add_library(Winhttp_SimpleAPI SHARED WinhttpAPI.cpp WinhttpAPI.h)