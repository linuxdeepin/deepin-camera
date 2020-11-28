/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <dlfcn.h>
#include "load_libs.h"
#include <encoder.h>
#include <libavformat/avformat.h>
static LoadLibs *instance = NULL;
static LoadLibs *newClass(void)
{
    instance = (LoadLibs *)malloc(sizeof(LoadLibs));
    char *error;
//    RTLD_NOW：在dlopen返回前，解析出全部没有定义的符号，解析不出来返回NULL。
//    RTLD_LAZY：暂缓决定，等有需要时再解出符号
    void *handle = dlopen("libavcodec.so",RTLD_LAZY);
    if (!handle) {
        fprintf (stderr, "%s ", dlerror());
    }


#if !LIBAVCODEC_VER_AT_LEAST(58,9)
    dlsym(handle, "avcodec_register_all");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif

#if LIBAVCODEC_VER_AT_LEAST(57,64)
#else
    dlsym(handle, "avcodec_decode_video2");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif

#if !LIBAVCODEC_VER_AT_LEAST(53,34)
    dlsym(handle, "avcodec_init");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif


#if LIBAVCODEC_VER_AT_LEAST(53,6)
    instance->m_avcodec_alloc_context3 = (uos_avcodec_alloc_context3)dlsym(handle, "avcodec_alloc_context3");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_get_context_defaults3 = (uos_avcodec_get_context_defaults3)dlsym(handle, "avcodec_get_context_defaults3");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#else
    instance->m_avcodec_alloc_context = (uos_avcodec_alloc_context)dlsym(handle, "avcodec_alloc_context");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    dlsym(handle, "avcodec_get_context_defaults");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif

#if LIBAVCODEC_VER_AT_LEAST(53,6)
    dlsym(handle, "avcodec_open2");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

#else
    dlsym(handle, "avcodec_open");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif

#if LIBAVCODEC_VER_AT_LEAST(55,28)
    dlsym(handle, "av_frame_alloc");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    dlsym(handle, "av_frame_unref");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#else
    dlsym(handle, "avcodec_alloc_frame");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    dlsym(handle, "avcodec_get_frame_defaults");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif

#if LIBAVCODEC_VER_AT_LEAST(55,28)
    dlsym(handle, "av_frame_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#else
    #if LIBAVCODEC_VER_AT_LEAST(54,28)
        dlsym(handle, "avcodec_free_frame");
        if ((error = dlerror()) != NULL)  {
            fprintf (stderr, "%s ", error);
        }
    #else
        dlsym(handle, "av_freep");
        if ((error = dlerror()) != NULL)  {
            fprintf (stderr, "%s ", error);
        }
    #endif
#endif

    instance->m_avcodec_find_encoder = (uos_avcodec_find_encoder)dlsym(handle, "avcodec_find_encoder");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_find_decoder = (uos_avcodec_find_decoder)dlsym(handle, "avcodec_find_decoder");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_find_encoder_by_name = (uos_avcodec_find_encoder_by_name)dlsym(handle, "avcodec_find_encoder_by_name");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

    instance->m_avcodec_open2 = (uos_avcodec_open2)dlsym(handle, "avcodec_open2");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_flush_buffers = (uos_avcodec_flush_buffers)dlsym(handle, "avcodec_flush_buffers");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_close = (uos_avcodec_close)dlsym(handle, "avcodec_close");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_parameters_from_context = (uos_avcodec_parameters_from_context)dlsym(handle, "avcodec_parameters_from_context");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#if LIBAVFORMAT_VERSION_MAJOR >= 57 && LIBAVFORMAT_VERSION_MINOR <= 25
    dlsym(handle, "avcodec_find_decoder");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#else
    dlsym(handle, "avcodec_parameters_to_context");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
#endif
    instance->m_avcodec_send_packet = (uos_avcodec_send_packet)dlsym(handle, "avcodec_send_packet");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_receive_frame = (uos_avcodec_receive_frame)dlsym(handle, "avcodec_receive_frame");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_receive_packet = (uos_avcodec_receive_packet)dlsym(handle, "avcodec_receive_packet");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_packet_free = (uos_av_packet_free)dlsym(handle, "av_packet_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_fill_audio_frame = (uos_avcodec_fill_audio_frame)dlsym(handle, "avcodec_fill_audio_frame");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_packet_unref = (uos_av_packet_unref)dlsym(handle, "av_packet_unref");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avcodec_send_frame = (uos_avcodec_send_frame)dlsym(handle, "avcodec_send_frame");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_packet_rescale_ts = (uos_av_packet_rescale_ts)dlsym(handle, "av_packet_rescale_ts");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_init_packet = (uos_av_init_packet)dlsym(handle, "av_init_packet");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_packet_alloc = (uos_av_packet_alloc)dlsym(handle, "av_packet_alloc");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

    //libavformat
    void *handle1 = dlopen("libavformat.so",RTLD_LAZY);
    if (!handle1) {
        fprintf (stderr, "%s ", dlerror());
    }
    instance->m_avformat_open_input = (uos_avformat_open_input)dlsym(handle1, "avformat_open_input");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avformat_find_stream_info = (uos_avformat_find_stream_info)dlsym(handle1, "avformat_find_stream_info");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_find_best_stream = (uos_av_find_best_stream)dlsym(handle1, "av_find_best_stream");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_dump_format = (uos_av_dump_format)dlsym(handle1, "av_dump_format");//目前未使用
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avformat_close_input = (uos_avformat_close_input)dlsym(handle1, "avformat_close_input");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avformat_alloc_output_context2 = (uos_avformat_alloc_output_context2)dlsym(handle1, "avformat_alloc_output_context2");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avformat_new_stream = (uos_avformat_new_stream)dlsym(handle1, "avformat_new_stream");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avformat_free_context = (uos_avformat_free_context)dlsym(handle1, "avformat_free_context");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avformat_write_header = (uos_avformat_write_header)dlsym(handle1, "avformat_write_header");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avio_open = (uos_avio_open)dlsym(handle1, "avio_open");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_write_frame = (uos_av_write_frame)dlsym(handle1, "av_write_frame");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_write_trailer = (uos_av_write_trailer)dlsym(handle1, "av_write_trailer");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_avio_closep = (uos_avio_closep)dlsym(handle1, "avio_closep");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }


    //libffmpegthumbnailer
    void *handle2 = dlopen("libffmpegthumbnailer.so",RTLD_LAZY);
    if (!handle2) {
        fprintf (stderr, "%s ", dlerror());
    }
    instance->m_video_thumbnailer = (uos_video_thumbnailer)dlsym(handle2, "video_thumbnailer_create");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_video_thumbnailer_destroy = (uos_video_thumbnailer_destroy)dlsym(handle2, "video_thumbnailer_destroy");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_video_thumbnailer_create_image_data = (uos_video_thumbnailer_create_image_data)dlsym(handle2, "video_thumbnailer_create_image_data");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_video_thumbnailer_destroy_image_data = (uos_video_thumbnailer_destroy_image_data)dlsym(handle2, "video_thumbnailer_destroy_image_data");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_video_thumbnailer_generate_thumbnail_to_buffer = (uos_video_thumbnailer_generate_thumbnail_to_buffer)dlsym(handle2, "video_thumbnailer_generate_thumbnail_to_buffer");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

    void *handle3 = dlopen("libswresample.so",RTLD_LAZY);
    if (!handle3) {
        fprintf (stderr, "%s ", dlerror());
    }
    instance->m_swr_free = (uos_swr_free)dlsym(handle3, "swr_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }


    void *handle4 = dlopen("libswscale.so",RTLD_LAZY);
    if (!handle4) {
        fprintf (stderr, "%s ", dlerror());
    }
    instance->m_sws_freeContext = (uos_sws_freeContext)dlsym(handle4, "sws_freeContext");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

    //libavutil
    void *handle5 = dlopen("libavutil.so",RTLD_LAZY);
    if (!handle5) {
        fprintf (stderr, "%s ", dlerror());
    }
    instance->m_av_dict_get = (uos_av_dict_get)dlsym(handle5, "av_dict_get");//目前未使用
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_strerror = (uos_av_strerror)dlsym(handle5, "av_strerror");//目前未使用
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

    instance->m_av_dict_copy = (uos_av_dict_copy)dlsym(handle5, "av_dict_copy");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_dict_free = (uos_av_dict_free)dlsym(handle5, "av_dict_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_dict_set = (uos_av_dict_set)dlsym(handle5, "av_dict_set");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_dict_set_int = (uos_av_dict_set_int)dlsym(handle5, "av_dict_set_int");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_log_set_level = (uos_av_log_set_level)dlsym(handle5, "av_log_set_level");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_image_copy_to_buffer = (uos_av_image_copy_to_buffer)dlsym(handle5, "av_image_copy_to_buffer");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_frame_free = (uos_av_frame_free)dlsym(handle5, "av_frame_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_frame_alloc = (uos_av_frame_alloc)dlsym(handle5, "av_frame_alloc");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_freep = (uos_av_freep)dlsym(handle5, "av_freep");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_frame_unref = (uos_av_frame_unref)dlsym(handle5, "av_frame_unref");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_free = (uos_av_free)dlsym(handle5, "av_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_samples_get_buffer_size = (uos_av_samples_get_buffer_size)dlsym(handle5, "av_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_get_media_type_string = (uos_av_get_media_type_string)dlsym(handle5, "av_free");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
    instance->m_av_image_get_buffer_size = (uos_av_image_get_buffer_size)dlsym(handle5, "av_image_get_buffer_size");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
 ;

    assert(instance != NULL);
    return instance;
}

/**
 * 饿汉式
 * 支持延迟加载，但是为了多线程安全，性能有所降低
 * 注意：方法内部要加锁，防止多线程多次创建
 * */
LoadLibs *getLoadLibsInstance()
{    
    static pthread_mutex_t mutex;
    //双检锁
    if (instance == NULL) {
        // 这里要对instance加锁
        pthread_mutex_lock(&mutex);
        if (instance == NULL)
        {
            instance = newClass();
        }
        //退出时解锁
        pthread_mutex_unlock(&mutex);
    }

    return instance;
}

