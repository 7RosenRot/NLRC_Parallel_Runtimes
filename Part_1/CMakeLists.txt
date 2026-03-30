cmake_minimum_required(VERSION 3.20)
set(This Part_1)

project(${This} CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Headers
  workspace/include/counter.hpp
)

add_library(${This} INTERFACE)

target_include_directories(${This} INTERFACE
  workspace/include
)

add_subdirectory(tests)