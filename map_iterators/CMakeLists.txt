cmake_minimum_required(VERSION 3.0)

project(AMSTeL_design_tests_map_iterators)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories("${PROJECT_SOURCE_DIR}/..")

add_executable(test_map_iterators ${PROJECT_SOURCE_DIR}/test_map_iterators.cpp)
