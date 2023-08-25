cmake_minimum_required(VERSION 3.8)

project("AudioVisualizer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++17")

set(src_dir "${PROJECT_SOURCE_DIR}/src/")
set(libraries "${PROJECT_SOURCE_DIR}/lib/")
set(kissfft "${PROJECT_SOURCE_DIR}/KissFFT/")
set(include_dir "${PROJECT_SOURCE_DIR}/include/" ${kissfft})

file(GLOB_RECURSE source_files "${src_dir}/*.c" "${src_dir}/*.cpp")
file(GLOB_RECURSE header_files "${include_dir}/*.h" "${include_dir}/*.hpp")
file(GLOB kissfft_files "${kissfft}/*.h" "${kissfft}/*.c")

include_directories(${include_dir})

add_executable(${PROJECT_NAME} ${source_files} ${header_files} ${kissfft_files})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/SDL2.lib ${CMAKE_SOURCE_DIR}/lib/SDL2main.lib ${CMAKE_SOURCE_DIR}/lib/SDL2_mixer.lib opengl32)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/SDL2d.lib ${CMAKE_SOURCE_DIR}/lib/SDL2main.lib ${CMAKE_SOURCE_DIR}/lib/SDL2_mixer.lib opengl32)
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/dll ${CMAKE_SOURCE_DIR}/music $<TARGET_FILE_DIR:AudioVisualizer>)