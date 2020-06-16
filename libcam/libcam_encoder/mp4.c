#include "mp4.h"




//OutputStream video_st = { 0 }, audio_st = { 0 };

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
    printf("timebase:%d/%d", video_stream->st->time_base.num, video_stream->st->time_base.den);
    if(!video_stream->st)
    {
        fprintf(stderr,"Could not allocate stream\n");
        exit(1);
    }
    video_stream->st->id = mp4_ctx->nb_streams - 1;
    video_stream->st->time_base = video_codec_data->codec_context->time_base;
    video_stream->enc = video_codec_data->codec_context;
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
   // video_codec_data->outpkt = (AVPacket*)realloc();
//    video_codec_data->outpkt->data = (uint8_t*)calloc((unsigned int)enc_video_ctx->outbuf_size, sizeof(uint8_t));
//    memcpy(video_codec_data->outpkt->data, enc_video_ctx->outbuf, enc_video_ctx->outbuf_size);
//    video_codec_data->outpkt->size = enc_video_ctx->outbuf_size;

    AVPacket *outvideopacket  = video_codec_data->outpkt;
    //outvideopacket = (AVPacket*)calloc(1,sizeof (AVPacket));
    //av_init_packet(&outvideopacket);
    av_log_set_level(AV_LOG_ERROR);
//    outvideopacket.data = enc_video_ctx->outbuf;
//    outvideopacket.size = enc_video_ctx->outbuf_size;
//    outvideopacket.pts = enc_video_ctx->pts;
//    outvideopacket.dts = enc_video_ctx->dts;
//    outvideopacket.duration = enc_video_ctx->duration;
//    outvideopacket.flags = enc_video_ctx->flags;
//    outvideopacket.stream_index = stream_index;

    outvideopacket->data = (uint8_t*)calloc((unsigned int)enc_video_ctx->outbuf_size, sizeof(uint8_t));
    memcpy(outvideopacket->data, enc_video_ctx->outbuf, enc_video_ctx->outbuf_size);
    outvideopacket->size = enc_video_ctx->outbuf_size;
    outvideopacket->pts = video_codec_data->frame->pts;
    outvideopacket->dts = enc_video_ctx->dts;
    outvideopacket->duration = enc_video_ctx->duration;
    outvideopacket->flags = enc_video_ctx->flags;
    outvideopacket->stream_index = stream_index;


    //AVRational time_base =mp4_ctx->streams[stream_index]->time_base;
    AVRational time_base = video_codec_data->codec_context->time_base;
    //av_packet_rescale_ts(outvideopacket, time_base, (AVRational){time_base.num,(time_base.den/2)});
    av_packet_rescale_ts(outvideopacket, time_base, (AVRational){time_base.num,(time_base.den/2)});

    //    printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
//           av_ts2str(outvideopacket.pts), av_ts2timestr(outvideopacket.pts, time_base),
//           //av_ts2str(outvideopacket.dts), av_ts2timestr(outvideopacket.dts, time_base),
//           av_ts2str(outvideopacket.duration), av_ts2timestr(outvideopacket.duration, time_base),
//           outvideopacket.stream_index);
    if (outvideopacket->stream_index < 0 || outvideopacket->stream_index >= mp4_ctx->nb_streams) {
            printf("Invalid packet stream index: %d\n",outvideopacket->stream_index);
            return AVERROR(EINVAL);
        }

    if (mp4_ctx->streams[outvideopacket->stream_index]->codecpar->codec_type == AVMEDIA_TYPE_ATTACHMENT) {
            printf("Received a packet for an attachment stream.\n");
            return AVERROR(EINVAL);
        }

    av_interleaved_write_frame(mp4_ctx,outvideopacket);
    //printf("av_write_frame(mp4_ctx,&outvideopacket):return %d",i);
    free(outvideopacket->data);
}

int mp4_write_audio_packet(
    AVFormatContext *mp4_ctx,
    int stream_index,
    encoder_audio_context_t *enc_audio_ctx,
    encoder_codec_data_t * audio_codec_data)
{

    AVPacket *outaudiopacket  = audio_codec_data->outpkt;
    //outvideopacket = (AVPacket*)calloc(1,sizeof (AVPacket));
    //av_init_packet(&outvideopacket);
    //av_log_set_level(AV_LOG_ERROR);
    outaudiopacket->data = (uint8_t*)calloc((unsigned int)enc_audio_ctx->outbuf_size, sizeof(uint8_t));
    memcpy(outaudiopacket->data, enc_audio_ctx->outbuf, enc_audio_ctx->outbuf_size);
    outaudiopacket->size = enc_audio_ctx->outbuf_size;
    outaudiopacket->pts = audio_codec_data->frame->pts;
    outaudiopacket->dts = enc_audio_ctx->dts;
    outaudiopacket->duration = enc_audio_ctx->duration;
    outaudiopacket->flags = enc_audio_ctx->flags;
    outaudiopacket->stream_index = stream_index;

    AVRational time =mp4_ctx->streams[stream_index]->time_base;
    AVRational time_base = audio_codec_data->codec_context->time_base;
    av_packet_rescale_ts(outaudiopacket, time, (AVRational){1,time_base.den/44100});

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
