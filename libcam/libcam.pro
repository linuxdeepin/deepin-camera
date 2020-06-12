#-------------------------------------------------
#
# Project created by QtCreator 2020-04-27T14:08:06
#
#-------------------------------------------------

TARGET = depcam
TEMPLATE = lib
CONFIG   += c++11 link_pkgconfig

DEFINES += LIBCHEESE_LIBRARY QT_NO_WARNING_OUTPUT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=libcam_v4l2core/colorspaces.c \
    libcam_v4l2core/control_profile.c \
    libcam_v4l2core/core_time.c \
    libcam_v4l2core/dct.c \
    libcam_v4l2core/frame_decoder.c \
    libcam_v4l2core/jpeg_decoder.c \
    libcam_v4l2core/save_image.c \
    libcam_v4l2core/save_image_bmp.c \
    libcam_v4l2core/save_image_jpeg.c \
    libcam_v4l2core/save_image_png.c \
    libcam_v4l2core/soft_autofocus.c \
    libcam_v4l2core/uvc_h264.c \
    libcam_v4l2core/v4l2_controls.c \
    libcam_v4l2core/v4l2_core.c \
    libcam_v4l2core/v4l2_devices.c \
    libcam_v4l2core/v4l2_formats.c \
    libcam_v4l2core/v4l2_xu_ctrls.c \
    libcam_v4l2core/core_io.c \
    libcam_v4l2core/gui.c\
    libcam_encoder/audio_codecs.c \
    libcam_encoder/avi.c \
    libcam_encoder/encoder.c \
    libcam_encoder/file_io.c \
    libcam_encoder/libav_encoder.c \
    libcam_encoder/matroska.c \
    libcam_encoder/muxer.c \
    libcam_encoder/stream_io.c \
    libcam_encoder/video_codecs.c \
    libcam_render/render.c \
    libcam_render/render_fx.c \
    libcam_render/render_osd_crosshair.c \
    libcam_render/render_osd_vu_meter.c \
    libcam_render/render_sdl2.c \
    libcam_audio/audio.c \
    libcam_audio/audio_fx.c \
    libcam_audio/audio_portaudio.c \
    libcam_audio/audio_pulseaudio.c \
    libcam/config.c \
    libcam/options.c \
    libcam/camview.c

HEADERS +=libcam_v4l2core/colorspaces.h \
    libcam_v4l2core/control_profile.h \
    libcam_v4l2core/core_time.h \
    libcam_v4l2core/dct.h \
    libcam_v4l2core/frame_decoder.h \
    libcam_v4l2core/gviewv4l2core.h \
    libcam_v4l2core/jpeg_decoder.h \
    libcam_v4l2core/save_image.h \
    libcam_v4l2core/soft_autofocus.h \
    libcam_v4l2core/uvc_h264.h \
    libcam_v4l2core/v4l2_controls.h \
    libcam_v4l2core/v4l2_core.h \
    libcam_v4l2core/v4l2_devices.h \
    libcam_v4l2core/v4l2_formats.h \
    libcam_v4l2core/v4l2_xu_ctrls.h \
    libcam_v4l2core/gui.h\
    libcam_v4l2core/gview.h \
    libcam_v4l2core/core_io.h \
    libcam_encoder/avi.h \
    libcam_encoder/encoder.h \
    libcam_encoder/file_io.h \
    libcam_encoder/gview.h \
    libcam_encoder/gviewencoder.h \
    libcam_encoder/matroska.h \
    libcam_encoder/stream_io.h \
    libcam_render/gview.h \
    libcam_render/gviewrender.h \
    libcam_render/render.h \
    libcam_render/render_sdl2.h \
    libcam_audio/audio.h \
    libcam_audio/audio_portaudio.h \
    libcam_audio/audio_pulseaudio.h \
    libcam_audio/gview.h \
    libcam_audio/gviewaudio.h \
    libcam/config.h \
    libcam/options.h \
    libcam/camview.h

INCLUDEPATH +=/usr/include/libusb-1.0\
                /usr/include/SDL2\
                /usr/include\
                ./libcam_v4l2core \
                ./libcam_render \
                ./libcam_encoder \
                ./libcam_audio \
                ./libcam

isEmpty(PREFIX){
    PREFIX = /usr
}

BUILD_DIST = ../src/libs
ARCH = $$QMAKE_HOST.arch

DESTDIR = $$BUILD_DIST

unix {
    target.path = /usr/lib
    INSTALLS += target

    isEmpty(PREFIX){
        target.path = $$[QT_INSTALL_LIBS]
    } else {
        target.path = $${PREFIX}/lib
    }
ClEAR_LIB_OBJ = rm -f ./*.o \
            ../libcam/*.o \
            ../libcam/libcam_v4l2core/*.o \
            ../libcam/libcam_render/*.o \
            ../libcam/libcam_encoder/*.o \
            ../libcam/libcam_audio/*.o
QMAKE_POST_LINK += $$quote($$ClEAR_LIB_OBJ)
}

QMAKE_CXXFLAGS += -Wno-enum-compare
