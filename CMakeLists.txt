cmake_minimum_required(VERSION 3.9)
enable_testing()
project(jitome)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD          17) # use c++17
set(CMAKE_CXX_EXTENSIONS       OFF) # not gnu17, c++17

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/extlib/ut/include)
include_directories(${PROJECT_SOURCE_DIR}/extlib/xbyak/xbyak)

execute_process(COMMAND git submodule update --init --recursive
                WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

add_definitions("-Wfatal-errors")

add_subdirectory(src)
add_subdirectory(tests)
