QT += core gui printsupport svg dbus concurrent dtkwidget multimedia
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
include(camera/src/basepub/basepub.pri)
include(camera/src/libcam/libcam.pri)
include(camera/src/qtsingleapplication/qtsingleapplication.pri)

SOURCES += \
    $$PWD/camera/src/capplication.cpp \
    $$PWD/camera/src/main.cpp \
    $$PWD/camera/src/mainwindow.cpp \
    $$PWD/camera/src/videowidget.cpp \
    $$PWD/camera/src/thumbnailsbar.cpp \
    $$PWD/camera/src/majorimageprocessingthread.cpp \
    $$PWD/camera/src/LPF_V4L2.c \
    $$PWD/camera/src/devnummonitor.cpp \
    $$PWD/camera/src/Settings.cpp\
    $$PWD/camera/src/imageitem.cpp\
    $$PWD/camera/src/closedialog.cpp \
    $$PWD/camera/src/settings_translation.cpp \
    $$PWD/camera/src/dbus_adpator.cpp \
    $$PWD/camera/src/previewopenglwidget.cpp \
    $$PWD/camera/src/shortcut.cpp

HEADERS += \
    $$PWD/camera/src/mainwindow.h \
    $$PWD/camera/src/thumbnailsbar.h \
    $$PWD/camera/src/majorimageprocessingthread.h \
    $$PWD/camera/src/LPF_V4L2.h \
    $$PWD/camera/src/videowidget.h \
    $$PWD/camera/src/devnummonitor.h \
    $$PWD/camera/src/imageitem.h \
    $$PWD/camera/src/Settings.h \
    $$PWD/camera/src/closedialog.h \
    $$PWD/camera/src/capplication.h \
    $$PWD/camera/src/dbus_adpator.h \
    $$PWD/camera/src/previewopenglwidget.h \
    $$PWD/camera/src/shortcut.h \
    $$PWD/camera/config.h


INCLUDEPATH +=/usr/include/libusb-1.0\
                /usr/include \
                $$PWD/camera/src/libcam/libcam_v4l2core\
                $$PWD/camera/src/libcam/libcam_render\
                $$PWD/camera/src/libcam/libcam_encoder \
                $$PWD/camera/src/libcam/libcam_audio \
                $$PWD/camera/src/libcam/libcam \
                $$PWD/camera/src/basepub \
                $$PWD/camera \
                $$PWD/camera/src/accessibility \
                $$PWD/camera/src/accessible \
                $$PWD/camera/src/qtsingleapplication

LIBS += -L/lib/x86_64-linux-gnu\
#        -ludev\
#        -lusb-1.0\
#        -lv4l2\
#        -lavutil\
#        -lswscale\
#        -lpng\
#        -lSDL2\
#        -lportaudio\
#        -lswresample \
        -ldl

LIBS += -LSYSTEMLIBDIR

RESOURCES += \
    $$PWD/camera/assets/assets.qrc

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-camera
isEmpty(PREFIX){
    PREFIX = /usr
}


target.path = $$INSTROOT$$BINDIR
icon_files.path = $$INSTROOT$$PREFIX/share/icons/hicolor/scalable/apps/
icon_files.files = $$PWD/camera/assets/deepin-camera.svg

desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/camera/deepin-camera.desktop

manual_dir.files = $$PWD/camera/assets/deepin-camera
manual_dir.path=/usr/share/deepin-manual/manual-assets/application/

dbus_service.files = $$PWD/camera/com.deepin.Camera.service
dbus_service.path = $$PREFIX/share/dbus-1/services
unix {
    target.path = /usr/bin
#    INSTALLS += target
}

CONFIG(release, debug|release) {
    TRANSLATIONS = $$files($$PWD/camera/translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
}

translations.path = $$PREFIX/share/deepin-camera/translations
translations.files = $$PWD/camera/translations/*.qm

INSTALLS = target desktop dbus_service icon_files translations manual_dir
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

