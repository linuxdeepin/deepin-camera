
# 定义需要的cmake版本
cmake_minimum_required(VERSION 3.16)

# 设置工程名字
project(deepin-camera-test VERSION 1.0.0)
set(CMD_NAME deepin-camera-test)
set(TARGET_NAME deepin-camera-test)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
option (UNITTEST "Use UNITTEST" ON)
add_definitions(-DUNITTEST)

execute_process(COMMAND uname -m OUTPUT_VARIABLE MACH
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)



include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_INCLUDE_CURRENT_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(PROJECT_INCLUDE
    ../libcam/libcam_v4l2core
    ../libcam/libcam_render
    ../libcam/libcam_encoder
    ../libcam/libcam_audio
    ../libcam/libcam
    ../src/src/basepub
    ../src/src/accessibility
    ../src/src/accessible
    ../src/src/qtsingleapplication
    ../src/
    ./stub
    ${CMAKE_INSTALL_PREFIX}/include
    ${CMAKE_INSTALL_PREFIX}/include/libusb-1.0
    /usr/include/libusb-1.0
    ${CMAKE_INSTALL_PREFIX}/include/SDL2
    ${PROJECT_SOURCE_DIR}/../deepin-camera
    ${PROJECT_SOURCE_DIR}/googletest/include
    )
# 设置Qt6模块
set(QtModule Core Gui Widgets DBus Concurrent LinguistTools Multimedia PrintSupport Svg SvgWidgets OpenGLWidgets Xml Test)

# 查找Qt6相关的库链接
find_package(Qt6 REQUIRED ${QtModule})
find_package(PkgConfig REQUIRED)

file (GLOB_RECURSE CXXSOURCES
    ./*.cpp
    ../src/src/basepub/*.cpp
    ../src/src/qtsingleapplication/*.cpp
    ./stub/*.cpp
    )

# 收集 src/src/*.cpp (排除 videosurface.cpp)
file(GLOB_RECURSE SRC_CXXSOURCES ../src/src/*.cpp)

# 将 src/src/*.cpp (排除 videosurface.cpp) 加入编译
foreach(src_file ${SRC_CXXSOURCES})
    get_filename_component(src_name ${src_file} NAME)
    if(NOT src_name STREQUAL "videosurface.cpp")
        list(APPEND CXXSOURCES ${src_file})
    endif()
endforeach()

file (GLOB_RECURSE CSOURCES
    ../src/src/*.c
    ../libcam/*.c
    ../src/src/basepub/*.c
    )

file (GLOB_RECURSE HEADERS
    ../src/src/basepub/*.h
    ../libcam/*.h
    ../src/src/qtsingleapplication/*.h
    ../src/src/*.h
    ../src/src/accessibility/*.h
    ../src/src/accessible/*.h
    ../tests/*.h
    ./stub/*.h
    ./stub/*.hpp
    )
file (GLOB_RECURSE RESOURCES  ../src/assets/assets.qrc)

configure_file(../src/config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)

add_executable( ${TARGET_NAME} ${CXXSOURCES} ${CSOURCES} ${RESOURCES}
    )


pkg_check_modules(3rd_lib REQUIRED
        dtk6gui
        dtk6widget
        gstreamer-1.0
        gstreamer-app-1.0
        pciaccess
        libva
        libavcodec
        libavutil
        libavformat
        libswscale
        libswresample
        libudev
        )
# 添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)

target_include_directories(${TARGET_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS} ${PROJECT_INCLUDE} ${GLOB_RECURSE})
# 将第三方库链接进来(注意 *_LIBRARIES)
target_link_libraries(${TARGET_NAME} PRIVATE
    ${3rd_lib_LIBRARIES}
    gtest
    pthread
    Qt6::Test
    dl
    va
    va-x11

    imagevisualresult6
    )

# 将工程与Qt6模块链接起来
target_link_libraries(${TARGET_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
    Qt6::Concurrent
    Qt6::Multimedia
    Qt6::PrintSupport
    Qt6::Svg
    Qt6::SvgWidgets
    Qt6::OpenGLWidgets
)

# 链接DTK6库
target_link_libraries(${TARGET_NAME} PRIVATE
    dtk6core
    dtk6gui
    dtk6widget
)

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

#configure_file(${PROJECT_SOURCE_DIR}/config.h.in  @ONLY)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# 添加gocv覆盖率文件的输出
#set(CMAKE_CXX_FLAGS "-g -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer -g -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
set(CMAKE_Ｌ_FLAGS "-fsanitize=undefined,address,leak -fno-omit-frame-pointer")
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE ON)

set(OBJECT_DIR ${CMAKE_BINARY_DIR}/CMakeFiles/objects)

#add_custom_target(test
#    COMMAND mkdir -p report
#    COMMAND mkdir -p html
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

#add_custom_command(TARGET test
#    COMMAND echo " ===================CREAT LCOV REPROT==================== "
#    COMMAND lcov --directory ./tests/CMakeFiles/deepin-camera-test.dir --zerocounters
#    COMMAND ./tests/${PROJECT_NAME} --gtest_output=xml:./report/report.xml
#    COMMAND lcov --directory . --capture --output-file ./coverageResult/${PROJECT_NAME}_Coverage.info

#    COMMAND echo " =================== do filter begin ==================== "
#    COMMAND lcov --remove ./coverageResult/${PROJECT_NAME}_Coverage.info 'CMakeFiles/${PROJECT_NAME}.dir/${PROJECT_NAME}_autogen/*/*'
#    '${PROJECT_NAME}_autogen/*/*'
#    'googletest/*/*'
#    '*${CMAKE_INSTALL_PREFIX}/include/*'
#    '*/tests/*'
#    '*/src/qtsingleapplication/*'
#    '*/basepub/printoptionspage.cpp'
#    '*/src/src/dbus_adpator.cpp'
#    '*/src/src/settings_translation.cpp'
#    '${CMAKE_INSTALL_PREFIX}/local/*'
#    -o ./coverageResult/${PROJECT_NAME}_Coverage_fileter.info
#    COMMAND echo " =================== do filter end ==================== "

#    COMMAND genhtml -o ./coverageResult/report ./coverageResult/${PROJECT_NAME}_Coverage_fileter.info
#    COMMAND echo " -- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverageResult/report "
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    )
#add_dependencies(test ${PROJECT_NAME})
