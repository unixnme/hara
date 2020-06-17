cmake_minimum_required(VERSION 3.14)
project(examples)

set(CMAKE_CXX_STANDARD 11)

# optional
add_definitions(-DHARA_VERBOSE)

add_subdirectory(../ ${CMAKE_CURRENT_BINARY_DIR}/hara)

add_executable(cat cat.cc)
target_link_libraries(cat hara)

add_executable(tokenize tokenizer.cc)
target_link_libraries(tokenize hara)

add_executable(everyline everyline.cc)
target_link_libraries(everyline hara)

add_executable(count count.cc)
target_link_libraries(count hara)

add_executable(pqueue_performance pqueue_performance.cc)
target_link_libraries(pqueue_performance hara)