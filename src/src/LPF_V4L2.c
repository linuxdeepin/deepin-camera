/*
 * (c) 2020, Uniontech Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifdef __cplusplus
extern "C" {
#endif
#include "LPF_V4L2.h"
#include "gviewaudio.h"
#include "gviewv4l2core.h"
#include "v4l2_devices.h"
#include "v4l2_core.h"
#include "gviewencoder.h"
#include "gviewrender.h"
#include "audio.h"
#include "core_io.h"

static __THREAD_TYPE capture_thread;
__MUTEX_TYPE capture_mutex = __STATIC_MUTEX_INIT;//初始化静态锁。
__COND_TYPE capture_cond;//初始化条件变量

extern int debug_level = 1;//定义debug等级

static v4l2_dev_t *vd2 = NULL;//摄像头设备

static audio_context_t *my_audio_ctx = NULL;//音频内容

static int save_image = 0; /*save image flag*///保存图片 true = 1,false = 0

static int save_video = 0; /*save video flag*///保存视频 true = 1,false = 0

static int my_encoder_status = 0;//当前编码状态

static audio_context_t *audio_ctx = NULL;//音频内容

static int vcodec_ind = 0;

static int acodec_ind = 0;

static int video_muxer = ENCODER_MUX_MKV;

uint32_t my_render_mask = REND_FX_YUV_NOFILT; /*render fx filter mask*/

uint32_t my_audio_mask = AUDIO_FX_NONE; /*audio fx filter mask*/

static int my_video_timer = 0;

static int my_video_begin_time = 0;

static __THREAD_TYPE encoder_thread;

unsigned char *rgb24 = NULL;


static int convert_yuv_to_rgb_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;
    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = r ;
    pixel[1] = g ;
    pixel[2] = b ;
    return pixel32;
}

int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;

    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 =
                yuv[in + 3] << 24 |
                               yuv[in + 2] << 16 |
                                              yuv[in + 1] <<  8 |
                                                              yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = convert_yuv_to_rgb_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
        pixel32 = convert_yuv_to_rgb_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}

void close_audio_context()
{
    if(my_audio_ctx != NULL)
        audio_close(my_audio_ctx);

    my_audio_ctx = NULL;
}

void close_v4l2_device_handler()
{
    /*closes the video device*/
    v4l2core_close_dev(vd2);

    vd2 = NULL;
}

audio_context_t *create_audio_context(int api, int device)
{

    close_audio_context();

    my_audio_ctx = audio_init(api, device);

    if(my_audio_ctx == NULL)
        fprintf(stderr, "GUVCVIEW: couldn't allocate audio context\n");

    return my_audio_ctx;
}

int LPF_StartRun()
{
    const rlim_t kStackSize = 128L * 1024L * 1024L;
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if(result == 0)
    {
        if (rl.rlim_cur < kStackSize)
                {
                    rl.rlim_cur = kStackSize;
                    result = setrlimit(RLIMIT_STACK, &rl);
                    if (result != 0)
                    {
                        fprintf(stderr, "GUVCVIEW: setrlimit returned result = %d\n", result);
                    }
                }
    }

    /*本地化*/
    char* lc_all = setlocale (LC_ALL, "");
    char* lc_dir = bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    char* txtdom = textdomain (GETTEXT_PACKAGE);
    options_t *my_options = options_get();
    char* config_path = smart_cat(getenv("HOME"),'/',".config/cheese");
    mkdir(config_path, 0777);
    char *device_name = get_file_basename(my_options->device);
    char *config_file = smart_cat(config_path, '/', device_name);
    /*释放字符串内存*/
    free(config_path);
    free(device_name);
    /*加载配置文件*/
    config_load(config_file);
    /*用option更新配置文件*/
    config_update(my_options);
    /*获取配置项数据*/
    config_t *my_config = config_get();
    debug_level = my_options->verbosity;
    if (debug_level > 1) printf("cheese: language catalog=> dir:%s type:%s cat:%s.mo\n",
        lc_dir, lc_all, txtdom);
    int render = RENDER_SDL;
    if(strcasecmp(my_config->render, "none") == 0)
            render = RENDER_NONE;
        else if(strcasecmp(my_config->render, "sdl") == 0)
        {
#if ENABLE_SDL2
        render = RENDER_SDL;
#elif ENABLE_SFML
        render = RENDER_SFML;
        printf("GUVCVIEW: not build with sdl2 support (rebuild with --enable-sdl2)\n");
        printf("GUVCVIEW: using sfml render instead\n");
#else
        render = RENDER_NONE;
        printf("GUVCVIEW: not build with sfml or sdl2 support\n");
        printf("GUVCVIEW: not rendering any video preview\n");
#endif
    }
    else if(strcasecmp(my_config->render, "sfml") == 0)
    {
#if ENABLE_SFML
        render = RENDER_SFML;
#elif ENABLE_SDL2
        render = RENDER_SDL;
        printf("cheese: not build with sfml support (rebuild with --enable-sfml)\n");
        printf("cheese: using sdl2 render instead\n");
#else
        render = RENDER_NONE;
        printf("GUVCVIEW: not build with sfml or sdl2 support\n");
        printf("GUVCVIEW: not rendering any video preview\n");
#endif
    }
    int audio = AUDIO_PORTAUDIO;

    if(strcasecmp(my_config->audio, "none") == 0)
        audio = AUDIO_NONE;
    else if(strcasecmp(my_config->audio, "port") == 0)
        audio = AUDIO_PORTAUDIO;
#if HAS_PULSEAUDIO
    else if(strcasecmp(my_config->audio, "pulse") == 0)
        audio = AUDIO_PULSE;
#endif

    if(debug_level > 1)
        printf("GUVCVIEW: main thread (tid: %u)\n",
            (unsigned int) syscall (SYS_gettid));

    /*设置v4l2debug*/
    v4l2core_set_verbosity(debug_level);

    /*得到v4l2的句柄*/
    vd2 = create_v4l2_device_handler(my_options->device);
    if(vd2 == NULL){
        char message[50];
        snprintf(message, 49, "no video device (%s) found", my_options->device);
        options_clean();
        return -1;
    }
    else{
        /*设置渲染方式*/
        set_render_flag(render);
    }

    if(my_options->disable_libv4l2){
        v4l2core_disable_libv4l2(vd2);
    }
    /*选择捕捉方式*/
    if(strcasecmp(my_config->capture,"read") == 0){
        v4l2core_set_capture_method(vd2,IO_READ);
    }
    else{
        v4l2core_set_capture_method(vd2,IO_MMAP);
    }

    /*设置软件自动对焦排序方法*/
    v4l2core_soft_autofocus_set_sort(AUTOF_SORT_INSERT);

    /*定义fps*/
    v4l2core_define_fps(vd2, my_config->fps_num, my_config->fps_denom);

    /*设置音视频特效*/
    set_render_fx_mask(my_config->video_fx);
    set_audio_fx_mask(my_config->audio_fx);

    my_config->osd_mask &= ~REND_OSD_VUMETER_MONO;
    my_config->osd_mask &= ~REND_OSD_VUMETER_STEREO;
    render_set_osd_mask(my_config->osd_mask);


    /*设置视频编码器*/
    if(debug_level > 1){
        printf("cheese: setting video codec to '%s'\n", my_config->video_codec);
    }
    vcodec_ind = encoder_get_video_codec_ind_4cc(my_config->video_codec);
    if(vcodec_ind < 0)
    {
        char message[50];
                snprintf(message, 49, "invalid video codec '%s' using raw input", my_config->video_codec);
                fprintf(stderr, "cheese: invalid video codec '%s' using raw input\n", my_config->video_codec);
                vcodec_ind = 0;
    }
    set_video_codec_ind(vcodec_ind);

    /*设置音频编码器*/
    if(debug_level > 1){
        printf("cheese: setting audio codec to '%s'\n", my_config->audio_codec);
    }
    acodec_ind = encoder_get_audio_codec_ind_name(my_config->audio_codec);
    if(acodec_ind < 0)
    {
        char message[50];
        snprintf(message, 49, "invalid audio codec '%s' using pcm input", my_config->audio_codec);
        fprintf(stderr, "cheese: invalid audio codec '%s' using pcm input\n", my_config->audio_codec);
        acodec_ind = 0;
    }
    set_audio_codec_ind(acodec_ind);

    if(my_options->prof_filename){
        v4l2core_load_control_profile(vd2, my_options->prof_filename);
    }
    /*设置保存的配置文件路径*/
    if(!my_config->profile_name)
        my_config->profile_name = strdup(get_profile_name());
    if(!my_config->profile_path)
        my_config->profile_path = strdup(get_profile_path());
    set_profile_name(my_config->profile_name);
    set_profile_path(my_config->profile_path);

    /*设置视频文件地址*/
    if(!my_config->video_name)
        my_config->video_name = strdup(get_video_name());
    if(!my_config->video_path)
        my_config->video_path = strdup(get_video_path());
    set_video_name(my_config->video_name);
    set_video_path(my_config->video_path);

    /*设置照片文件保存地址*/
    if(!my_config->photo_name)
        my_config->photo_name = strdup(get_photo_name());
    if(!my_config->photo_path)
        my_config->photo_path = strdup(get_photo_path());
    set_photo_name(my_config->photo_name);
    set_photo_path(my_config->photo_path);

    /*设置音频接口debug等级*/
    audio_set_verbosity(debug_level);

    /*初始化音频context*/
    audio_ctx = create_audio_context(audio, my_config->audio_device);
    if(audio_ctx != NULL)
    {
        my_config->audio_device = audio_get_device_index(audio_ctx);
    }
    else{
        fprintf(stderr, "cheese: couldn't get a valid audio context for the selected api - disabling audio\n");
    }

    encoder_set_verbosity(debug_level);
    if(!my_options->control_panel){

        //v4l2core_prepare_new_format(vd2, my_config->format);
        //v4l2core_prepare_new_resolution(vd2, my_config->width, my_config->height);
        //v4l2core_load_control_profile(vd2,NULL);
        v4l2core_prepare_new_format(vd2, V4L2_PIX_FMT_YUYV);
        v4l2core_prepare_new_resolution(vd2, 640, 480);
        int ret = v4l2core_update_current_format(vd2);

        if (ret != E_OK) {
            v4l2core_prepare_valid_format(vd2);
            v4l2core_prepare_valid_resolution(vd2);
            ret = v4l2core_update_current_format(vd2);
            if (ret != E_OK) {
                fprintf(stderr, "cheese: also could not set the first listed stream format\n");
                fprintf(stderr, "cheese: Video capture failed\n");
            }
        }

        return ret;
    }
    return E_OK;
}





v4l2_dev_t *get_v4l2_dev()
{
    return vd2;
}

void video_capture_save_video(int value)
{
    save_video = value;

    if(debug_level > 1)
        printf("GUVCVIEW: save video flag changed to %i\n", save_video);
}

audio_context_t *get_audio_context()
{
    if(!my_audio_ctx)
        return NULL;

    /*force a valid number of channels*/
    if(audio_get_channels(my_audio_ctx) > 2)
        audio_set_channels(my_audio_ctx, 2);

    return my_audio_ctx;
}

int video_capture_get_save_video()
{
    return save_video;
}

static void *audio_processing_loop(void *data)
{
    encoder_context_t *encoder_ctx = (encoder_context_t *) data;

    audio_context_t *audio_ctx = get_audio_context();
    if(!audio_ctx)
    {
        fprintf(stderr, "GUVCVIEW: no audio context: skiping audio processing\n");
        return ((void *) -1);
    }
    audio_buff_t *audio_buff = NULL;

    /*start audio capture*/
    int frame_size = encoder_get_audio_frame_size(encoder_ctx);

    //if(frame_size < 1024)
    //	frame_size = 1024;

    audio_set_cap_buffer_size(audio_ctx,
        frame_size * audio_get_channels(audio_ctx));
    audio_start(audio_ctx);
    /*
     * alloc the buffer after audio_start
     * otherwise capture_buff_size may not
     * be correct
     * allocated data is big enough for float samples (32 bit)
     * although it may contain int16 samples (16 bit)
     */
    audio_buff = audio_get_buffer(audio_ctx);

    int sample_type = encoder_get_audio_sample_fmt(encoder_ctx);

    uint32_t osd_mask = render_get_osd_mask();

    /*enable vu meter OSD display*/
    if(audio_get_channels(audio_ctx) > 1)
        osd_mask |= REND_OSD_VUMETER_STEREO;
    else
        osd_mask |= REND_OSD_VUMETER_MONO;

    render_set_osd_mask(osd_mask);

    while(video_capture_get_save_video())
    {
        int ret = audio_get_next_buffer(audio_ctx, audio_buff,
                sample_type, my_audio_mask);

        if(ret > 0)
        {
            /*
             * no buffers to process
             * sleep a couple of milisec
             */
             struct timespec req = {
                .tv_sec = 0,
                .tv_nsec = 1000000};/*nanosec*/
             nanosleep(&req, NULL);
        }
        else if(ret == 0)
        {
            encoder_ctx->enc_audio_ctx->pts = audio_buff->timestamp;

            /*OSD vu meter level*/
            render_set_vu_level(audio_buff->level_meter);

            encoder_process_audio_buffer(encoder_ctx, audio_buff->data);
        }

    }

    /*flush any delayed audio frames*/
    encoder_flush_audio_buffer(encoder_ctx);

    /*reset vu meter*/
    audio_buff->level_meter[0] = 0;
    audio_buff->level_meter[1] = 0;
    render_set_vu_level(audio_buff->level_meter);

    /*disable OSD vumeter*/
    osd_mask &= ~REND_OSD_VUMETER_STEREO;
    osd_mask &= ~REND_OSD_VUMETER_MONO;

    render_set_osd_mask(osd_mask);

    audio_stop(audio_ctx);
    audio_delete_buffer(audio_buff);

    return ((void *) 0);
}

void *encoder_loop()
{
    my_encoder_status = 1;
    __THREAD_TYPE encoder_audio_thread;

    int channels = 0;
    int samprate = 0;

    if(audio_ctx)
    {
        channels = audio_get_channels(audio_ctx);
        samprate = audio_get_samprate(audio_ctx);
    }

    /*create the encoder context*/
    encoder_context_t *encoder_ctx = encoder_init(
        v4l2core_get_requested_frame_format(vd2),
        vcodec_ind,
        acodec_ind,
        video_muxer,
        v4l2core_get_frame_width(vd2),
        v4l2core_get_frame_height(vd2),
        v4l2core_get_fps_num(vd2),
        v4l2core_get_fps_denom(vd2),
        channels,
        samprate);

    /*store external SPS and PPS data if needed*/
    if(encoder_ctx->video_codec_ind == 0 && /*raw - direct input*/
        v4l2core_get_requested_frame_format(vd2) == V4L2_PIX_FMT_H264)
    {
        /*request a IDR (key) frame*/
        v4l2core_h264_request_idr(vd2);

        if(debug_level > 0)
            printf("GUVCVIEW: storing external pps and sps data in encoder context\n");
        encoder_ctx->h264_pps_size = v4l2core_get_h264_pps_size(vd2);
        if(encoder_ctx->h264_pps_size > 0)
        {
            encoder_ctx->h264_pps = calloc(encoder_ctx->h264_pps_size, sizeof(uint8_t));
            if(encoder_ctx->h264_pps == NULL)
            {
                fprintf(stderr,"GUVCVIEW: FATAL memory allocation failure (encoder_loop): %s\n", strerror(errno));
                exit(-1);
            }
            memcpy(encoder_ctx->h264_pps, v4l2core_get_h264_pps(vd2), encoder_ctx->h264_pps_size);
        }

        encoder_ctx->h264_sps_size = v4l2core_get_h264_sps_size(vd2);
        if(encoder_ctx->h264_sps_size > 0)
        {
            encoder_ctx->h264_sps = calloc(encoder_ctx->h264_sps_size, sizeof(uint8_t));
            if(encoder_ctx->h264_sps == NULL)
            {
                fprintf(stderr,"GUVCVIEW: FATAL memory allocation failure (encoder_loop): %s\n", strerror(errno));
                exit(-1);
            }
            memcpy(encoder_ctx->h264_sps, v4l2core_get_h264_sps(vd2), encoder_ctx->h264_sps_size);
        }
    }

    uint32_t current_framerate = 0;
    if(v4l2core_get_requested_frame_format(vd2) == V4L2_PIX_FMT_H264)
    {
        /* store framerate since it may change due to scheduler*/
        current_framerate = v4l2core_get_h264_frame_rate_config(vd2);
    }

    char *video_filename = smart_cat(getenv("HOME"),'/',"Videos/my_video.mkv");
    /*muxer initialization*/
    encoder_muxer_init(encoder_ctx, video_filename);

    /*start video capture*/
    video_capture_save_video(1);

    int treshold = 102400; /*100 Mbytes*/
    int64_t last_check_pts = 0; /*last pts when disk supervisor called*/

    /*start audio processing thread*/
    if(encoder_ctx->enc_audio_ctx != NULL && audio_get_channels(audio_ctx) > 0)
    {
        if(debug_level > 1)
            printf("GUVCVIEW: starting encoder audio thread\n");

        int ret = __THREAD_CREATE(&encoder_audio_thread, audio_processing_loop, (void *) encoder_ctx);

        if(ret)
            fprintf(stderr, "GUVCVIEW: encoder audio thread creation failed (%i)\n", ret);
        else if(debug_level > 2)
            printf("GUVCVIEW: created audio encoder thread with tid: %u\n",
                (unsigned int) encoder_audio_thread);
    }

    while(video_capture_get_save_video())
    {
        /*process the video buffer*/
        if(encoder_process_next_video_buffer(encoder_ctx) > 0)
        {
            /*
             * no buffers to process
             * sleep a couple of milisec
             */
             struct timespec req = {
                .tv_sec = 0,
                .tv_nsec = 1000000};/*nanosec*/
             nanosleep(&req, NULL);

        }

        /*disk supervisor*/
        if(encoder_ctx->enc_video_ctx->pts - last_check_pts > 2 * NSEC_PER_SEC)
        {
            last_check_pts = encoder_ctx->enc_video_ctx->pts;
        }
    }
    encoder_flush_video_buffer(encoder_ctx);

    /*make sure the audio processing thread has stopped*/
    if(encoder_ctx->enc_audio_ctx != NULL && audio_get_channels(audio_ctx) > 0)
    {
        if(debug_level > 1)
            printf("GUVCVIEW: join encoder audio thread\n");
        __THREAD_JOIN(encoder_audio_thread);
    }

    /*close the muxer*/
    encoder_muxer_close(encoder_ctx);

    /*close the encoder context (clean up)*/
    encoder_close(encoder_ctx);

    if(v4l2core_get_requested_frame_format(vd2) == V4L2_PIX_FMT_H264)
    {
        /* restore framerate */
        v4l2core_set_h264_frame_rate_config(vd2, current_framerate);
    }

    /*clean strings*/
    //free(video_filename);

    my_encoder_status = 0;

    return ((void *) 0);
}
void reset_video_timer()
{
    my_video_timer = 0;
    my_video_begin_time = 0;
}

int start_encoder_thread()
{
    int ret = __THREAD_CREATE(&encoder_thread, encoder_loop, NULL);
    return ret;
}

int stop_encoder_thread()
{
        video_capture_save_video(0);

        __THREAD_JOIN(encoder_thread);

        if(debug_level > 1)
            printf("GUVCVIEW: encoder thread terminated and joined\n");

        return 0;
}

int get_video_codec_ind()
{
    return vcodec_ind;
}

int get_audio_codec_ind()
{
    return acodec_ind;
}
#ifdef __cplusplus
}
#endif







