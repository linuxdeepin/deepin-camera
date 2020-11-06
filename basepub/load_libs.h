#ifndef LOAD_LIBS_H
#define LOAD_LIBS_H
#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <pthread.h>

//AVCodec *avcodec_find_encoder(enum AVCodecID id);
typedef AVCodec *(*uos_avcodec_find_encoder)(enum AVCodecID id);
//AVCodec *avcodec_find_decoder(enum AVCodecID id);
typedef AVCodec *(*uos_avcodec_find_decoder)(enum AVCodecID id);
typedef AVCodec *(*uos_avcodec_find_encoder_by_name)(const char *name);
typedef AVCodecContext *(*uos_avcodec_alloc_context)(const AVCodec *codec);
typedef AVCodecContext *(*uos_avcodec_alloc_context3)(const AVCodec *codec);
//int avcodec_get_context_defaults3(AVCodecContext *s, const AVCodec *codec);
typedef int (*uos_avcodec_get_context_defaults3)(AVCodecContext *s, const AVCodec *codec);
//int avcodec_open2(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
typedef int (*uos_avcodec_open2)(AVCodecContext *avctx, const AVCodec *codec, AVDictionary **options);
//void avcodec_flush_buffers(AVCodecContext *avctx);
typedef void (*uos_avcodec_flush_buffers)(AVCodecContext *avctx);
//int avcodec_close(AVCodecContext *avctx);
typedef int (*uos_avcodec_close)(AVCodecContext *avctx);
//int avcodec_parameters_from_context(AVCodecParameters *par,const AVCodecContext *codec);
typedef int (*uos_avcodec_parameters_from_context)(AVCodecParameters *par,const AVCodecContext *codec);
//int avcodec_send_packet(AVCodecContext *avctx, const AVPacket *avpkt);
typedef int (*uos_avcodec_send_packet)(AVCodecContext *avctx, const AVPacket *avpkt);
//int avcodec_receive_frame(AVCodecContext *avctx, AVFrame *frame);
typedef int (*uos_avcodec_receive_frame)(AVCodecContext *avctx, AVFrame *frame);
//int avcodec_receive_packet(AVCodecContext *avctx, AVPacket *avpkt);
typedef int (*uos_avcodec_receive_packet)(AVCodecContext *avctx, AVPacket *avpkt);
//void av_packet_free(AVPacket **pkt);
typedef void (*uos_av_packet_free)(AVPacket **pkt);
//int avcodec_fill_audio_frame(AVFrame *frame, int nb_channels,enum AVSampleFormat sample_fmt, const uint8_t *buf,int buf_size, int align);
typedef int (*uos_avcodec_fill_audio_frame)(AVFrame *frame, int nb_channels, enum AVSampleFormat sample_fmt, const uint8_t *buf, int buf_size, int align);
//void av_packet_unref(AVPacket *pkt);
typedef void (*uos_av_packet_unref)(AVPacket *pkt);
//int avcodec_send_frame(AVCodecContext *avctx, const AVFrame *frame);
typedef int (*uos_avcodec_send_frame)(AVCodecContext *avctx, const AVFrame *frame);
//void av_packet_rescale_ts(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);
typedef void (*uos_av_packet_rescale_ts)(AVPacket *pkt, AVRational tb_src, AVRational tb_dst);
//void av_init_packet(AVPacket *pkt);
typedef void (*uos_av_init_packet)(AVPacket *pkt);
//AVPacket *av_packet_alloc(void);
typedef AVPacket *(*uos_av_packet_alloc)(void);

typedef struct _LoadLibs
{
    uos_avcodec_find_encoder m_avcodec_find_encoder;
    uos_avcodec_find_decoder m_avcodec_find_decoder;
    uos_avcodec_find_encoder_by_name m_avcodec_find_encoder_by_name;
    uos_avcodec_alloc_context m_avcodec_alloc_context;
    uos_avcodec_alloc_context3 m_avcodec_alloc_context3;
    uos_avcodec_get_context_defaults3 m_avcodec_get_context_defaults3;
    uos_avcodec_open2 m_avcodec_open2;
    uos_avcodec_flush_buffers m_avcodec_flush_buffers;
    uos_avcodec_close m_avcodec_close;
    uos_avcodec_parameters_from_context m_avcodec_parameters_from_context;
    uos_avcodec_send_packet m_avcodec_send_packet;
    uos_avcodec_receive_frame m_avcodec_receive_frame;
    uos_avcodec_receive_packet m_avcodec_receive_packet;
    uos_av_packet_free m_av_packet_free;
    uos_avcodec_fill_audio_frame m_avcodec_fill_audio_frame;
    uos_av_packet_unref m_av_packet_unref;
    uos_avcodec_send_frame m_avcodec_send_frame;
    uos_av_packet_rescale_ts m_av_packet_rescale_ts;
    uos_av_init_packet m_av_init_packet;
    uos_av_packet_alloc m_av_packet_alloc;
} LoadLibs;

LoadLibs *getLoadLibsInstance();//饿汉式不支持延迟加载

#endif//LOAD_LIBS_H
