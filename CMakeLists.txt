cmake_minimum_required(VERSION 3.8)

project("AudioVisualizer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")

set(include_dir "${PROJECT_SOURCE_DIR}/include/")
set(src_dir "${PROJECT_SOURCE_DIR}/src/")
set(libraries "${PROJECT_SOURCE_DIR}/lib/")

file(GLOB_RECURSE source_files "${src_dir}/*.c" "${src_dir}/*.cpp")
file(GLOB_RECURSE header_files "${include_dir}/*.h" "${include_dir}/*.hpp")

find_package(SDL2 REQUIRED)

include_directories(${include_dir})

add_executable(${PROJECT_NAME} ${source_files} ${header_files})

target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2 SDL2::SDL2main opengl32)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/dll $<TARGET_FILE_DIR:AudioVisualizer>)