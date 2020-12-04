cmake_minimum_required(VERSION 3.17)
project(untitled2)

include_directories(/usr/local/lib/pkgconfig)
find_library(untitled2_lib untitled2)

set(frameworks ${untitled2_lib})

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(SOURCE_FILES main.cpp)

add_executable(untitled2 ${SOURCE_FILES})
target_link_libraries(untitled2 gtest gtest_main)