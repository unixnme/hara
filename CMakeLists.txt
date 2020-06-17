cmake_minimum_required(VERSION 3.14)
project(hara)

set(CMAKE_CXX_STANDARD 11)

add_library(hara INTERFACE)
target_include_directories(hara INTERFACE include)
target_sources(hara INTERFACE
        include/hara/Input.h
        include/hara/Output.h
        include/hara/String.h
        include/hara/PriorityQueue.h
        include/hara/Macros.h)