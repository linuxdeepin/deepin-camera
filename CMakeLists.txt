cmake_minimum_required(VERSION 3.0)

project(deepin-camera VERSION 1.0.0)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
endif()

add_subdirectory(src)
