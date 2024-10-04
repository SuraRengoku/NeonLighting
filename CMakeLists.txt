cmake_minimum_required(VERSION 3.29)
project(NeonLighting)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB_RECURSE SRCS src/*.cpp)
file(GLOB_RECURSE HEADERS include/*.h)

add_executable(NeonLighting main.cpp)