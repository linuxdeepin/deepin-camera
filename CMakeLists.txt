cmake_minimum_required(VERSION 3.0)

project(deepin-camera VERSION 1.0.0)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Enable build hardening.")

    set(CMAKE_VERBOSE_MAKEFILE ON)

    set(HARDENING_FLAGS "-Wdate-time -D_FORTIFY_SOURCE=2 -g -O2 -ffile-prefix-map=${CMAKE_SOURCE_DIR}=. -fstack-protector-strong -fstack-clash-protection -Wformat -Werror=format-security")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HARDENING_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${HARDENING_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")
endif()

add_subdirectory(src)

#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_subdirectory(tests)
#endif()


