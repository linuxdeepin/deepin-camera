HEADERS += \
    $$PWD/libcam_v4l2core/colorspaces.h \
    $$PWD/libcam_v4l2core/control_profile.h \
    $$PWD/libcam_v4l2core/core_time.h \
    $$PWD/libcam_v4l2core/dct.h \
    $$PWD/libcam_v4l2core/frame_decoder.h \
    $$PWD/libcam_v4l2core/gviewv4l2core.h \
    $$PWD/libcam_v4l2core/jpeg_decoder.h \
    $$PWD/libcam_v4l2core/save_image.h \
    $$PWD/libcam_v4l2core/soft_autofocus.h \
    $$PWD/libcam_v4l2core/uvc_h264.h \
    $$PWD/libcam_v4l2core/v4l2_controls.h \
    $$PWD/libcam_v4l2core/v4l2_core.h \
    $$PWD/libcam_v4l2core/v4l2_devices.h \
    $$PWD/libcam_v4l2core/v4l2_formats.h \
    $$PWD/libcam_v4l2core/v4l2_xu_ctrls.h \
    $$PWD/libcam_v4l2core/gui.h\
    $$PWD/libcam_v4l2core/gview.h \
    $$PWD/libcam_v4l2core/core_io.h \
    $$PWD/libcam_encoder/avi.h \
    $$PWD/libcam_encoder/encoder.h \
    $$PWD/libcam_encoder/file_io.h \
    $$PWD/libcam_encoder/gview.h \
    $$PWD/libcam_encoder/gviewencoder.h \
    $$PWD/libcam_encoder/matroska.h \
    $$PWD/libcam_encoder/stream_io.h \
    $$PWD/libcam_encoder/mp4.h \
    $$PWD/libcam_render/gview.h \
    $$PWD/libcam_render/gviewrender.h \
    $$PWD/libcam_render/render.h \
    $$PWD/libcam_audio/audio.h \
    $$PWD/libcam_audio/audio_portaudio.h \
    #$$PWD/libcam_audio/audio_pulseaudio.h \
    $$PWD/libcam_audio/gview.h \
    $$PWD/libcam_audio/gviewaudio.h \
    $$PWD/libcam/config.h \
    $$PWD/libcam/options.h \
    $$PWD/libcam/camview.h

SOURCES += \
    $$PWD/libcam_v4l2core/colorspaces.c \
    $$PWD/libcam_v4l2core/control_profile.c \
    $$PWD/libcam_v4l2core/core_time.c \
    $$PWD/libcam_v4l2core/dct.c \
    $$PWD/libcam_v4l2core/frame_decoder.c \
    $$PWD/libcam_v4l2core/jpeg_decoder.c \
    $$PWD/libcam_v4l2core/save_image.c \
    $$PWD/libcam_v4l2core/save_image_bmp.c \
    $$PWD/libcam_v4l2core/save_image_jpeg.c \
#    $$PWD/libcam_v4l2core/save_image_png.c \
    $$PWD/libcam_v4l2core/soft_autofocus.c \
    $$PWD/libcam_v4l2core/uvc_h264.c \
    $$PWD/libcam_v4l2core/v4l2_controls.c \
    $$PWD/libcam_v4l2core/v4l2_core.c \
    $$PWD/libcam_v4l2core/v4l2_devices.c \
    $$PWD/libcam_v4l2core/v4l2_formats.c \
    $$PWD/libcam_v4l2core/v4l2_xu_ctrls.c \
    $$PWD/libcam_v4l2core/core_io.c \
    $$PWD/libcam_v4l2core/gui.c\
    $$PWD/libcam_encoder/audio_codecs.c \
    $$PWD/libcam_encoder/avi.c \
    $$PWD/libcam_encoder/encoder.c \
    $$PWD/libcam_encoder/file_io.c \
    $$PWD/libcam_encoder/libav_encoder.c \
    $$PWD/libcam_encoder/matroska.c \
    $$PWD/libcam_encoder/muxer.c \
    $$PWD/libcam_encoder/stream_io.c \
    $$PWD/libcam_encoder/video_codecs.c \
    $$PWD/libcam_encoder/mp4.c \
    $$PWD/libcam_render/render.c \
    $$PWD/libcam_render/render_fx.c \
    $$PWD/libcam_render/render_osd_crosshair.c \
    $$PWD/libcam_render/render_osd_vu_meter.c \
    $$PWD/libcam_audio/audio.c \
    $$PWD/libcam_audio/audio_fx.c \
    $$PWD/libcam_audio/audio_portaudio.c \
    #$$PWD/libcam_audio/audio_pulseaudio.c \
    $$PWD/libcam/config.c \
    $$PWD/libcam/options.c \
    $$PWD/libcam/camview.c


