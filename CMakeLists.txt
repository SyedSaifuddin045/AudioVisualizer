cmake_minimum_required(VERSION 3.8)

project("AudioVisualizer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")

set(include_dir "${PROJECT_SOURCE_DIR}/include/")
set(src_dir "${PROJECT_SOURCE_DIR}/src/")

file(GLOB_RECURSE source_files "${src_dir}/*.c" "${src_dir}/*.cpp")
file(GLOB_RECURSE header_files "${include_dir}/*.h" "${include_dir}/*.hpp")

include_directories(${include_dir})

add_executable(${PROJECT_NAME} ${source_files} ${header_files})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)