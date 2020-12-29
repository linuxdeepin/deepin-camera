cmake_minimum_required(VERSION 3.0)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
project(deepin-camera VERSION 1.0.0)
set(CMD_NAME deepin-camera)
set(TARGET_NAME deepin-camera)
#option (__mips__ "Use UNITTEST" ON)
#add_definitions(-D__mips__)

# 加速编译优化参数
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "mips64")
    SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -Wl,--as-need -fPIE")
    SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -Wl,--as-need -fPIE")
    SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
else()
    SET(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O3 -Wl,--as-need -fPIE")
    SET(CMAKE_C_FLAGS "$ENV{CFLAGS} -O3  -Wl,--as-need -fPIE")
    SET(CMAKE_EXE_LINKER_FLAGS  "-pie")
endif()

include_directories(${PROJECT_BINARY_DIR})
include_directories(${PROJECT_SOURCE_DIR})

configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h @ONLY)

set(PROJECT_INCLUDE
    ./src/libcam/libcam_v4l2core
    ./src/libcam/libcam_render
    ./src/libcam/libcam_encoder
    ./src/libcam/libcam_audio
    ./src/libcam/libcam
    ./src/basepub
    ./src/qtsingleapplication
    ./src/accessibility
    ./src/accessible
    ../
    /usr/include
    /usr/include/libusb-1.0
    )
    
# 设置Qt模块
set(QtModule Core Gui Widgets DBus Concurrent LinguistTools Multimedia PrintSupport)

# 查找Qt相关的库链接
find_package(Qt5 REQUIRED ${QtModule})
find_package(PkgConfig REQUIRED)

file (GLOB_RECURSE CXXSOURCES
    ./src/closedialog.cpp
    ./src/devnummonitor.cpp
    ./src/mainwindow.cpp
    ./src/majorimageprocessingthread.cpp
    ./src/previewopenglwidget.cpp
    ./src/shortcut.cpp
    ./src/thumbnailsbar.cpp
    ./src/videowidget.cpp
    ./src/imageitem.cpp
    ./src/Settings.cpp
    ./src/capplication.cpp
    ./src/dbus_adpator.cpp
    ./src/settings_translation.cpp
    ./src/basepub/*.cpp
    ./src/qtsingleapplication/qtsingleapplication.cpp
    ./src/qtsingleapplication/qtlocalpeer.cpp
    ./src/main.cpp
    )

file (GLOB_RECURSE CSOURCES
    ./src/LPF_V4L2.c
    ./src/libcam/*.c
    ./src/basepub/*.c
    )

file (GLOB_RECURSE HEADERS
    ./src/*.h
    ./src/libcam/*.h
    ./src/basepub/*.h
    ./src/qtsingleapplication/*.h
    ./src/accessibility/*.h
    ./src/accessible/*.h
    )

file (GLOB_RECURSE RESOURCES  ${PROJECT_SOURCE_DIR}/assets/assets.qrc)

pkg_check_modules(3rd_lib REQUIRED
        dtkwidget dtkcore
#        libv4l2
#        libudev
#        libusb-1.0
#        libavcodec
#        libavutil
#        libavformat
#        libswscale
#        libpng
#        sdl2
#        libpulse
#        libswresample
        )

# 添加第三方库的所有文件夹路径到工程中来(注意 *_INCLUDE_DIRS)

file (GLOB TS LIST_DIRECTORIES false ${PROJECT_SOURCE_DIR}/translations/${TARGET_NAME}*.ts)
#message(STATUS "qm")
#message(STATUS ${PROJECT_SOURCE_DIR})

#file(GLOB files LIST_DIRECTORIES false ${PROJECT_SOURCE_DIR}/translations/${CMD_NAME}*.ts)
#foreach(file IN LISTS files)
#    message(STATUS ${cur})
#endforeach(file)

set_source_files_properties(${TS} PROPERTIES OUTPUT_LOCATION ${PROJECT_SOURCE_DIR}/translations)

#message(STATUS "ts")
#message(STATUS ${TS})

qt5_create_translation(QM ${CXXSOURCES} ${TS})

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    qt5_create_translation(QM ${CXXSOURCES} ${TS})
endif ()

# 设置不删除生成的文件夹内容文件
set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)


add_executable( ${TARGET_NAME} ${CXXSOURCES} ${CSOURCES} ${RESOURCES} ${QM} )

target_include_directories(${TARGET_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS} ${PROJECT_INCLUDE} ${GLOB_RECURSE})
# 将第三方库链接进来(注意 *_LIBRARIES)
target_link_libraries(${TARGET_NAME} ${3rd_lib_LIBRARIES}
    pthread
#    portaudio
    dl
    )

# 将工程与Qt模块链接起来
qt5_use_modules(${TARGET_NAME} ${QtModule})


#application
install(TARGETS ${TARGET_NAME} DESTINATION /usr/bin)

#desktop
install(FILES ${PROJECT_SOURCE_DIR}/deepin-camera.desktop
    DESTINATION /usr/share/applications)

#icons
install(FILES ${PROJECT_SOURCE_DIR}/assets/deepin-camera.svg
    DESTINATION /usr/share/icons/hicolor/scalable/apps)

#manual
#install(DIRECTORY ${PROJECT_SOURCE_DIR}/dman/* DESTINATION /usr/share/dman)

#debus_service
install(FILES ${PROJECT_SOURCE_DIR}/com.deepin.Camera.service DESTINATION /usr/share/dbus-1/services)

##translations
#install(FILES ${PROJECT_SOURCE_DIR}/translations/*.qm DESTINATION /usr/share/${PROJECT_SOURCE_DIR}/translations)

set(APPSHAREDIR  /usr/share/${TARGET_NAME})

#translations
install(DIRECTORY ${PROJECT_SOURCE_DIR}/translations
    DESTINATION ${APPSHAREDIR}
    FILES_MATCHING PATTERN "*.qm")
