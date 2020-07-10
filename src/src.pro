QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-camera
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
QT += dtkwidget dtkgui
#PKGCONFIG += dframeworkdbus
# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES +=QT_DEPRECATED_WARNINGS

QMAKE_CFLAGS_ISYSTEM = -I

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/videowidget.cpp \
    src/thumbnailsbar.cpp \
    src/majorimageprocessingthread.cpp \
    src/LPF_V4L2.c \
    src/devnummonitor.cpp \
    src/Settings.cpp\
    src/imageitem.cpp\
    src/closedialog.cpp \
    src/settings_translation.cpp

HEADERS += \
    src/mainwindow.h \
    src/thumbnailsbar.h \
    src/majorimageprocessingthread.h \
    src/LPF_V4L2.h \
    src/videowidget.h \
    src/devnummonitor.h \
    src/imageitem.h\
    src/Settings.h\
    src/closedialog.h


INCLUDEPATH += ../libcam/libcam_v4l2core\
                ../libcam/libcam_render\
                ../libcam/libcam_encoder \
                ../libcam/libcam_audio \
                ../libcam/libcam

LIBS += -L./libs \
        -ldepcam



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
        -lavcodec\
        -lportaudio



LIBS += -LSYSTEMLIBDIR -lffmpegthumbnailer

#LIBS +=/usr/lib/x86_64-linux-gnu/libv4l2.a\
# /usr/lib/x86_64-linux-gnu/libusb-1.0.a\
# /usr/lib/x86_64-linux-gnu/libavformat.a\
# /usr/lib/x86_64-linux-gnu/libavutil.a\
# /usr/lib/x86_64-linux-gnu/libswscale.a\
# /usr/lib/x86_64-linux-gnu/libpng.a\
# /usr/lib/x86_64-linux-gnu/libz.a\
# /usr/lib/x86_64-linux-gnu/libSDL2.a\
# /usr/lib/x86_64-linux-gnu/libavcodec.a\
# /usr/lib/x86_64-linux-gnu/libportaudio.a

#        libv4l2.a\
#        libusb-1.0.a\
#        libavcodec.a\
#        libavutil.a\
#        libavformat.a\
#        libswscale.a\
#        libpng.a\
#        libz.a\
#        libSDL2.a\
#        libportaudio.a



RESOURCES += \
    resource/resources.qrc

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-camera
isEmpty(PREFIX){
    PREFIX = /usr
}

#!system($$PWD/translate_generation.sh): error("Failed to generate translation")

#DESTDIR = ./app

target.path = $$INSTROOT$$BINDIR
icon_files.path = $$PREFIX/share/icons/hicolor/scalable/apps/
icon_files.files = $$PWD/resource/deepin-camera.svg

desktop.path = $$INSTROOT$$APPDIR
desktop.files = deepin-camera.desktop

manual.path = /usr/share/dman/
manual.files = $$PWD/dman/*

#translations.path = $$INSTROOT$$DSRDIR/translations
#translations.files = translations/*.qm

dbus_service.files = $$PWD/com.deepin.Camera.service
dbus_service.path = $$PREFIX/share/dbus-1/services

unix {
    target.path = /usr/bin
#    INSTALLS += target
}

INSTALLS += target desktop icon_files manual dbus_service

#isEmpty(TRANSLATIONS) {
#     include(translations.pri)
#}

#TRANSLATIONS_COMPILED = $$TRANSLATIONS
#TRANSLATIONS_COMPILED ~= s/\.ts/.qm/g

#translations.files = $$TRANSLATIONS_COMPILED
#INSTALLS += translations
#CONFIG *= update_translations release_translations

#CONFIG(update_translations) {
#    isEmpty(lupdate):lupdate=lupdate
#    system($$lupdate -no-obsolete -locations none $$_PRO_FILE_)
#}
#CONFIG(release_translations) {
#    isEmpty(lrelease):lrelease=lrelease
#    system($$lrelease $$_PRO_FILE_)
#}
CONFIG(release, debug|release) {
    TRANSLATIONS = $$files($$PWD/translations/*.ts)
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    for(tsfile, TRANSLATIONS) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
}

translations.path = $$APPSHAREDIR/translations
translations.files = $$PWD/translations/*.qm

INSTALLS = target desktop dbus_service icons manual manual_icon app_icon translations

#DSR_LANG_PATH += $$DSRDIR/translations
#DEFINES += "DSR_LANG_PATH=\\\"$$DSR_LANG_PATH\\\""

#DISTFILES += \
#    image/newUI/focus/close-focus.svg

#DISTFILES += \
    #translations.pri


#ClEAR_OBJ_CHEESE = rm -rf ./*.o
#QMAKE_POST_LINK += $$quote($$ClEAR_OBJ_CHEESE)

#ClEAR_LIB_OBJ = rm -f ./*.o \
#            ../libcam/*.o \
#            ../libcam/libcam_v4l2core/*.o \
#            ../libcam/libcam_render/*.o \
#            ../libcam/libcam_encoder/*.o \
#            ../libcam/libcam_audio/*.o
#QMAKE_POST_LINK += $$quote($$ClEAR_LIB_OBJ)

#DISTFILES += \
#    ../deepin-camera.desktop \
#    deepin-camera.desktop \
#    com.deepin.Camera.service

#DISTFILES +=


