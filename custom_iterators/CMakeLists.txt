cmake_minimum_required(VERSION 3.12)

project(AMSTeL_design_tests_custom_iterators)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories("${PROJECT_SOURCE_DIR}/..")

add_executable(test_custom_iterators ${PROJECT_SOURCE_DIR}/test_custom_iterators.cpp)
target_compile_features(test_custom_iterators PUBLIC cxx_std_20)
