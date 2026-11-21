cmake_minimum_required(VERSION 4.3)
project(DynamicArray)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(DynamicArray Array.h
        main.cpp)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.18.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(unit_tests
        Array.h
        array_tests.cpp
)

target_link_libraries(unit_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)