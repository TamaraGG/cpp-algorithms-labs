cmake_minimum_required(VERSION 4.3)
project(RulerOfEgypt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_executable(RulerOfEgypt main.cpp
        settings.h
        interface_functions.h
        round_state.h
        round_state.cpp
        interface_functions.cpp
        work_with_files.h
        work_with_files.cpp)
