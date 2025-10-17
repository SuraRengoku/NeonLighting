cmake_minimum_required(VERSION 3.10)
project(NeonLighting)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
include_directories(include/BVH)
include_directories(include/Common)      
include_directories(include/Shading)
include_directories(include/Entity)
include_directories(include/external)

file(GLOB_RECURSE SRCS 
    src/*.cpp
    src/BVH/*.cpp
    src/Common/*.cpp
    src/Shading/*.cpp
    src/Entity/*.cpp
    src/external/*.cpp
)

file(GLOB_RECURSE HEADERS 
    include/*.h 
    include/*.hpp 
    include/BVH/*.h 
    include/BVH/*.hpp
    include/Common/*.h      
    include/Common/*.hpp    
    include/Shading/*.h 
    include/Shading/*.hpp
    include/Entity/*.h 
    include/Entity/*.hpp
    include/external/*.h 
    include/external/*.hpp
)

add_executable(NeonLighting main.cpp ${SRCS} ${HEADERS})

find_package(Threads REQUIRED)
target_link_libraries(NeonLighting Threads::Threads)

target_compile_options(NeonLighting PRIVATE
    $<$<CONFIG:Release>:-O3 -DNDEBUG>
    $<$<CONFIG:Debug>:-g>
)