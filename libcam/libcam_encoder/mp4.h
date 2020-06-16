#ifndef MP4_H
#define MP4_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavutil/hwcontext.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>

#include <va/va.h>
#include <va/va_x11.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xv.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xvlib.h>
#include <sys/shm.h>
#include <X11/Xfuncproto.h>
#include <libavfilter/buffersink.h>
#include <libavutil/log.h>
#include <va/va_drm.h>
#include <libavutil/avstring.h>

#include "encoder.h"
#include "gviewencoder.h"

#ifdef __cplusplus
} // endof extern "C"
#endif

typedef struct OutputStream {
    AVStream *st;
    AVCodecContext *enc;

    /* pts of the next frame that will be generated */
    int64_t next_pts;
    int samples_count;

    AVFrame *frame;
    AVFrame *tmp_frame;

    float t, tincr, tincr2;

    struct SwsContext *sws_ctx;
    struct SwrContext *swr_ctx;
} OutputStream;

AVFormatContext *mp4_create_context(const char *filename);

void mp4_add_video_stream(
    AVFormatContext *mp4_ctx,
    AVCodec **codec,
    encoder_codec_data_t *video_codec_data,
    OutputStream *video_stream,
    int32_t codec_id);

void mp4_add_audio_stream(
    AVFormatContext *mp4_ctx,
    AVCodec **codec,
    encoder_codec_data_t *audio_codec_data,
    OutputStream *audio_stream);

int mp4_write_video_packet(
    AVFormatContext *mp4_ctx,
    int stream_index,
    encoder_video_context_t *enc_video_ctx,
    encoder_codec_data_t *video_codec_data);

int mp4_write_audio_packet(
    AVFormatContext *mp4_ctx,
    int stream_index,
    encoder_audio_context_t *enc_audio_ctx,
    encoder_codec_data_t *audio_codec_data);

int mp4_write_header(AVFormatContext *mp4_ctx);

// int mp4_close(AVFormatContext *mp4_ctx);

void mp4_destroy_context(AVFormatContext *mp4_ctx);

#endif
