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

    void *handle1 = dlopen("libavformat.so",RTLD_LAZY);
    if (!handle1) {
        fprintf (stderr, "%s ", dlerror());
    }
    dlsym(handle1, "cos");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }

    void *handle2 = dlopen("libavutil.so",RTLD_LAZY);
    if (!handle2) {
        fprintf (stderr, "%s ", dlerror());
    }
    dlsym(handle2, "cos");
    if ((error = dlerror()) != NULL)  {
        fprintf (stderr, "%s ", error);
    }
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

