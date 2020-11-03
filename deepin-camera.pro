QT += core gui  printsupport svg dbus concurrent dtkwidget dtkgui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

  qtHaveModule(opengl): QT += opengl


CONFIG += c++11 link_pkgconfig
TEMPLATE = app
TARGET = deepin-camera

isEmpty(PREFIX){
    PREFIX = /usr
}

QMAKE_CFLAGS_ISYSTEM = -I

DEFINES += LIBCHEESE_LIBRARY
#DEFINES +=QT_NO_WARNING_OUTPUT
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#SUBDIRS += src
include(basepub/basepub.pri)
include(libcam/libcam.pri)
include(src/qtsingleapplication/qtsingleapplication.pri)

SOURCES += \
    $$PWD/src/src/main.cpp \
    $$PWD/src/src/mainwindow.cpp \
    $$PWD/src/src/videowidget.cpp \
    $$PWD/src/src/thumbnailsbar.cpp \
    $$PWD/src/src/majorimageprocessingthread.cpp \
    $$PWD/src/src/LPF_V4L2.c \
    $$PWD/src/src/devnummonitor.cpp \
    $$PWD/src/src/Settings.cpp\
    $$PWD/src/src/imageitem.cpp\
    $$PWD/src/src/closedialog.cpp \
    $$PWD/src/src/settings_translation.cpp \
    $$PWD/src/src/capplication.cpp

HEADERS += \
    $$PWD/src/src/mainwindow.h \
    $$PWD/src/src/thumbnailsbar.h \##
    $$PWD/src/src/majorimageprocessingthread.h \
    $$PWD/src/src/LPF_V4L2.h \
    $$PWD/src/src/videowidget.h \
    $$PWD/src/src/devnummonitor.h \
    $$PWD/src/src/imageitem.h \
    $$PWD/src/src/Settings.h \
    $$PWD/src/src/closedialog.h \
    $$PWD/src/src/capplication.h

INCLUDEPATH +=/usr/include/libusb-1.0\
                /usr/include/SDL2\
                /usr/include \
                ./libcam/libcam_v4l2core\
                ./libcam/libcam_render\
                ./libcam/libcam_encoder \
                ./libcam/libcam_audio \
                ./libcam/libcam \
                ./basepub

LIBS += -L/lib/x86_64-linux-gnu\
        -lasound\
        -ludev\
        -lusb-1.0\
        -lv4l2\
        -lavformat\
        -lavutil\
        -lswscale\
        -lpng\
        -lz\
        -lSDL2\
#        -lavcodec\
        -lportaudio\
        -lswresample \
        -ldl
        #-lpulse

LIBS += -LSYSTEMLIBDIR -lffmpegthumbnailer

RESOURCES += \
    $$PWD/src/resource/resources.qrc

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-camera
isEmpty(PREFIX){
    PREFIX = /usr
}


target.path = $$INSTROOT$$BINDIR
icon_files.path = $$INSTROOT$$PREFIX/share/icons/hicolor/scalable/apps/
icon_files.files = $$PWD/resource/deepin-camera.svg

desktop.path = $$INSTROOT$$APPDIR
desktop.files = deepin-camera.desktop

manual.path = /usr/share/dman/
manual.files = $$PWD/dman/*

dbus_service.files = $$PWD/com.deepin.Camera.service
dbus_service.path = $$PREFIX/share/dbus-1/services
unix {
    target.path = /usr/bin
#    INSTALLS += target
}

CONFIG(release, debug|release) {
    TRANSLATIONS = $$files($$PWD/translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
}

translations.path = $$PREFIX/share/deepin-camera/translations
translations.files = $$PWD/translations/*.qm

INSTALLS = target desktop dbus_service icon_files manual translations
QMAKE_CXXFLAGS += -Wl,-as-need -fPIE
QMAKE_LFLAGS+=-pie

host_sw_64: {
# 在 sw_64 平台上添加此参数，否则会在旋转图片时崩溃
    QMAKE_CFLAGS += -mieee
    QMAKE_CXXFLAGS += -mieee
}

host_mips64:{
   QMAKE_CXX += -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi
   QMAKE_CXXFLAGS += -O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi -Wl,as-need -fPIE
   QMAKE_LFLAGS+=-pie
}

