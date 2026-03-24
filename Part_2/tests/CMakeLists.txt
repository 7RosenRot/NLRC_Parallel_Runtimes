cmake_minimum_required(VERSION 3.20)
set(This Part_2Tests)

project(${This} CXX)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Threads REQUIRED)

set(Sources
  main.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
  Part_2
  GTest::gtest_main
  GTest::gtest
  Threads::Threads
)

include(GoogleTest)
gtest_discover_tests(${This})