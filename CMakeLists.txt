cmake_minimum_required(VERSION 3.8)
project(NoCProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES Main.cpp Setting.h Structs.h)
add_executable(NoCProject ${SOURCE_FILES})