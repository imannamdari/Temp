cmake_minimum_required(VERSION 3.8)
project(NoCProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/Main.cpp src/Setting.h src/Structs.h src/Handler.cpp src/Handler.h src/Container.cpp src/Container.h src/Transmitter.cpp src/Transmitter.h)
add_executable(NoCProject ${SOURCE_FILES})