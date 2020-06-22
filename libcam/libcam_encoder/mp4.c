#include "mp4.h"
#include <libavutil/mathematics.h>


//static uint64_t last_pts = 0;

//OutputStream video_st = { 0 }, audio_st = { 0 };
static int64_t video_pts = 0;
static int64_t audio_pts = 0;
static AVRational video_time;
static AVRational audio_time;

AVFormatContext * mp4_create_context(const char *filename)
{
    AVFormatContext *mp4_ctx;
    avformat_alloc_output_context2(&mp4_ctx, NULL, NULL, filename);
    if (!mp4_ctx) {
        printf("Could not deduce output format from file extension: using MPEG.\n");
        avformat_alloc_output_context2(&mp4_ctx, NULL, "mpeg", filename);
    }
    if (!mp4_ctx)
    {
		fprintf(stderr, "ENCODER: FATAL memory allocation failure (mp4_create_context): %s\n", strerror(errno));
		exit(-1);
	}
    return mp4_ctx;
}

void mp4_add_video_stream(
		AVFormatContext *mp4_ctx,
        AVCodec **codec,
        encoder_codec_data_t *video_codec_data,
        OutputStream *video_stream,
		int32_t codec_id)
{
    //OutputStream *video_stream = (OutputStream *)calloc(sizeoof(OutputStream));

    //*codec = avcodec_find_encoder(codec_id);
//    if(!(*codec))
//    {
//        fprintf(stderr, "Could not find encoder for '%s'\n",
//                avcodec_get_name(codec_id));
//        exit(1);
//    }
    video_stream->st = avformat_new_stream(mp4_ctx, video_codec_data->codec);
    //printf("timebase:%d/%d", video_stream->st->time_base.num, video_stream->st->time_base.den);

    if(!video_stream->st)
    {
        fprintf(stderr,"Could not allocate stream\n");
        exit(1);
    }
    video_stream->st->id = mp4_ctx->nb_streams - 1;
    video_stream->enc = video_codec_data->codec_context;
    video_stream->st->time_base = video_codec_data->codec_context->time_base;

    if(mp4_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        video_stream->enc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
}

void mp4_add_audio_stream(
		AVFormatContext *mp4_ctx,
        AVCodec **codec,
        encoder_codec_data_t *audio_codec_data,
        OutputStream *audio_stream)
{
//    *codec = avcodec_find_encoder(audio_codec_data->codec_context->codec_id);
//    if(!(*codec))
//    {
//        fprintf(stderr, "Could not find encoder for '%s'\n",
//                avcodec_get_name(audio_codec_data->codec_context->codec_id));
//        exit(1);
//    }
    audio_stream->st = avformat_new_stream(mp4_ctx,audio_codec_data->codec);
    if(!audio_stream->st)
    {
        fprintf(stderr,"Could not allocate stream\n");
        exit(1);
    }
    audio_stream->st->id = mp4_ctx->nb_streams - 1;
    audio_stream->st->time_base = audio_codec_data->codec_context->time_base;
    audio_stream->enc = audio_codec_data->codec_context;
    if(mp4_ctx->oformat->flags & AVFMT_GLOBALHEADER)
    {
        audio_stream->enc->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }
}

int mp4_write_video_packet(
        AVFormatContext *mp4_ctx,
        int stream_index,
        encoder_video_context_t *enc_video_ctx,
        encoder_codec_data_t *video_codec_data)
{
    AVPacket *outvideopacket  = video_codec_data->outpkt;

    av_log_set_level(AV_LOG_ERROR);


//    int64_t ts = enc_video_ctx->pts - video_pts;
//    uint64_t time = (uint64_t)ts / 1000000;
//    video_pts = enc_video_ctx->pts;

    outvideopacket->data = (uint8_t*)calloc((unsigned int)enc_video_ctx->outbuf_size, sizeof(uint8_t));
    memcpy(outvideopacket->data, enc_video_ctx->outbuf, enc_video_ctx->outbuf_size);
    outvideopacket->size = enc_video_ctx->outbuf_size;
    outvideopacket->pts = video_pts/*enc_video_ctx->pts*/;
//    outvideopacket->dts = enc_video_ctx->dts;
    outvideopacket->duration = 0/*ts * 3*/;
    outvideopacket->flags = enc_video_ctx->flags;
    outvideopacket->stream_index = stream_index;

//    int64_t times = 0;
//    times +=outvideopacket->duration;
    //AVRational time =mp4_ctx->streams[stream_index]->time_base;
    //AVRational time_base = video_codec_data->codec_context->time_base;
//    if(ts>0){
//        int base = (int)time;
        //av_packet_rescale_ts(outvideopacket, , (AVRational){1, 30});

        if (outvideopacket->stream_index < 0 || outvideopacket->stream_index >= mp4_ctx->nb_streams) {
                printf("Invalid packet stream index: %d\n",outvideopacket->stream_index);
                return AVERROR(EINVAL);
            }

        if (mp4_ctx->streams[outvideopacket->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_ATTACHMENT) {
                printf("Received a packet for an attachment stream.\n");
                return AVERROR(EINVAL);
            }

        av_interleaved_write_frame(mp4_ctx,outvideopacket);
//    }
    free(outvideopacket->data);
    video_pts += 1;
}

static AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
    AVFrame *picture;
    int ret;

    picture = av_frame_alloc();
    if (!picture)
        return NULL;

    picture->format = pix_fmt;
    picture->width  = width;
    picture->height = height;

    /* allocate the buffers for the frame data */
    ret = av_frame_get_buffer(picture, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate frame data.\n");
        exit(1);
    }

    return picture;
}

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)  //log日志相关 打印一些信息 	Definition at line 70 of file muxing.c.
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

    printf("pts:%s pts_time:%s duration:%s\n",av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),av_ts2str(pkt->duration));
    /*
    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);  //这里pts和dts都源自编码前frame.pts 而duration没有被赋值，是初始化时的0
    */
}

int mp4_write_packet(
        AVFormatContext *mp4_ctx,
        encoder_codec_data_t *codec_data,
        int stream_index,
        uint8_t *outbuf,
        uint32_t outbuf_size,
        uint64_t pts,
        int duration,
        int flags)
{
//    AVCodecContext *codec_ctx = codec_data->codec_context;
//    AVFrame* picture = alloc_picture(codec_ctx->pix_fmt,codec_ctx->height,codec_ctx->width);
//    AVRational video_time;
//    AVRational audio_time;
    AVPacket *outpacket  = codec_data->outpkt;
    outpacket->data = (uint8_t*)calloc((unsigned int)outbuf_size, sizeof(uint8_t));
    memcpy(outpacket->data, outbuf, outbuf_size);
    outpacket->size =(int)outbuf_size;

    if(codec_data->codec_context->codec_type == AVMEDIA_TYPE_VIDEO){
        uint64_t ts = (pts - 0) / 1000000;
        //video_pts = pts;
        //video_pts = audio_pts;
        outpacket->pts = video_pts/*MAX(0,(int64_t) ts)*/;/*enc_video_ctx->pts*/;
        outpacket->dts = video_pts;
    //    outvideopacket->dts = enc_video_ctx->dts;
        outpacket->duration = 0 /*+ durationts * 3*/;
        outpacket->flags = flags;
        outpacket->stream_index = stream_index;
        video_time = mp4_ctx->streams[stream_index]->time_base;
        AVRational time_base = codec_data->codec_context->time_base;
        av_packet_rescale_ts(outpacket, time_base, video_time);
        log_packet(mp4_ctx, outpacket);
        //outpacket->pts = av_rescale_q(outpacket->pts, time_base, (AVRational){1, 30});
        int ret = av_interleaved_write_frame(mp4_ctx, outpacket);
        if(ret < 0)
        {
            fprintf(stderr, "Error muxing packet\n");
        }
        free(outpacket->data);
        /*if(audio_pts)*/
        video_pts++;
    }
    if(codec_data->codec_context->codec_type == AVMEDIA_TYPE_AUDIO) {

        uint64_t ts = (pts - 0) / 1000000;
        //audio_pts = pts;
//         outpacket->data = (uint8_t*)calloc((unsigned int)outbuf_size, sizeof(uint8_t));
//         memcpy(outpacket->data, outbuf, (uint8_t)outbuf_size);
//         outpacket->size = (int)outbuf_size;
        //video_pts = 0;
        outpacket->pts = audio_pts; /*enc_audio_ctx->pts*/;
        //outpacket->dts = ts;
        //outpacket->duration = MAX(duration, (int64_t)ts /*+ duration*/)/*MAX(enc_audio_ctx->duration, (int64_t)ts + duration)*/;
        outpacket->flags = flags;
        outpacket->stream_index = stream_index;
        audio_time = mp4_ctx->streams[stream_index]->time_base;
        AVRational time_base = codec_data->codec_context->time_base;
        av_packet_rescale_ts(outpacket, time_base, audio_time);
        log_packet(mp4_ctx, outpacket);
    //         audio_pts += outpacket->pts;
        av_write_frame(mp4_ctx, outpacket);
    //         free(outpacket->data);
         free(outpacket->data);
         audio_pts+= 1152;

         //audio_pts += 1152;
    }
    else {
//        free(outpacket->data);
        av_packet_unref(outpacket);
    }




}

int mp4_write_audio_packet(
    AVFormatContext *mp4_ctx,
    int stream_index,
    encoder_audio_context_t *enc_audio_ctx,
    encoder_codec_data_t * audio_codec_data)
{
    if(audio_pts<video_pts){
    //    uint64_t ts = ((uint64_t)enc_audio_ctx->pts /*- last_pts*/) / 1000000;

    //    last_pts = (uint64_t)enc_audio_ctx->pts;
        AVPacket *outaudiopacket  = audio_codec_data->outpkt;
        //outvideopacket = (AVPacket*)calloc(1,sizeof (AVPacket));
        //av_init_packet(&outvideopacket);
        //av_log_set_level(AV_LOG_ERROR);
        outaudiopacket->data = (uint8_t*)calloc((unsigned int)enc_audio_ctx->outbuf_size, sizeof(uint8_t));
        memcpy(outaudiopacket->data, enc_audio_ctx->outbuf, enc_audio_ctx->outbuf_size);
        outaudiopacket->size = enc_audio_ctx->outbuf_size;
        outaudiopacket->pts = audio_pts /*enc_audio_ctx->pts*/;
        //outaudiopacket->dts = enc_audio_ctx->dts;
        //outaudiopacket->duration = enc_audio_ctx->duration/*MAX(enc_audio_ctx->duration, (int64_t)ts + duration)*/;
        outaudiopacket->flags = enc_audio_ctx->flags;
        outaudiopacket->stream_index = stream_index;

        //AVRational time =mp4_ctx->streams[stream_index]->time_base;
        //AVRational time_base = audio_codec_data->codec_context->time_base;
        //av_packet_rescale_ts(outaudiopacket, time_base, (AVRational){1, 1});

        if (outaudiopacket->stream_index < 0 || outaudiopacket->stream_index >= mp4_ctx->nb_streams) {
                printf("Invalid packet stream index: %d\n",outaudiopacket->stream_index);
                return AVERROR(EINVAL);
            }

        if (mp4_ctx->streams[outaudiopacket->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_ATTACHMENT) {
                printf("Received a packet for an attachment stream.\n");
                return AVERROR(EINVAL);
            }

        av_interleaved_write_frame(mp4_ctx,outaudiopacket);
        free(outaudiopacket->data);
        audio_pts += 201;
    }
}
// int mp4_close(AVFormatContext *mp4_ctx)
// {
//     av_write_trailer(mp4_ctx);
// }

void mp4_destroy_context(AVFormatContext *mp4_ctx)
{
    if(mp4_ctx != NULL)
    avformat_free_context(mp4_ctx);
}


