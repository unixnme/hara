cmake_minimum_required(VERSION 3.14)
project(io)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/hara)

add_executable(cat cat.cc)
target_link_libraries(cat hara)
target_compile_definitions(cat PUBLIC HARA_VERBOSE) # optional