cmake_minimum_required(VERSION 3.12)

project(AMSTeL_design_tests_map_tuple_keys)

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)

include_directories("${PROJECT_SOURCE_DIR}/..")

add_executable(test_map_NxN ${PROJECT_SOURCE_DIR}/test_map_NxN.cpp)
target_compile_features(test_map_NxN PUBLIC cxx_std_20)
add_executable(test_map_NxNxN ${PROJECT_SOURCE_DIR}/test_map_NxNxN.cpp)
target_compile_features(test_map_NxNxN PUBLIC cxx_std_20)
