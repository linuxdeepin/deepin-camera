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
                /usr/include


LIBS += -L./libs \
        -ldepcam

LIBS += -L/lib/x86_64-linux-gnu\
        -lv4l2\
        -ludev\
        -lusb-1.0\
        -lavcodec\
        -lavutil\
        -lavformat\
        -lswscale\
        -lpng\
        -lz\
        -lSDL2\
        -lportaudio\
         -lasound
LIBS += -LSYSTEMLIBDIR -lffmpegthumbnailer


RESOURCES += \
    resource/resources.qrc

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/dde-camera
isEmpty(PREFIX){
    PREFIX = /usr
}

#!system($$PWD/translate_generation.sh): error("Failed to generate translation")

#DESTDIR = ./app

target.path = $$INSTROOT$$BINDIR
icon_files.path = $$PREFIX/share/icons/hicolor/scalable/apps/
icon_files.files = $$PWD/src/resource/deepin-camera.svg

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


