/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
*
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

#ifdef __cplusplus
extern "C" {
#endif

#include "../libcam/libcam/camview.h"
#include "LPF_V4L2.h"
#include "gviewaudio.h"
#include "gviewv4l2core.h"
#include "v4l2_devices.h"
#include "v4l2_core.h"
#include "gviewencoder.h"
#include "gviewrender.h"
#include "audio.h"
#include "core_io.h"

int camInit(const char* devicename)
{
    const rlim_t kStackSize = 1024L * 1024L * 128L;
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
                        fprintf(stderr, "deepin-camera: setrlimit returned result = %d\n", result);
                    }
                }
    }

    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    options_t *my_options = options_get();
    char* config_path = smart_cat(getenv("HOME"),'/',".config/deepin-camera");
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

    int audio = AUDIO_PORTAUDIO;

    if(strcasecmp(my_config->audio, "none") == 0)
        audio = AUDIO_NONE;
    else if(strcasecmp(my_config->audio, "port") == 0)
        audio = AUDIO_PORTAUDIO;

    if(debug_level > 1)
        printf("deepin-camera: main thread (tid: %u)\n",
            (unsigned int) syscall (SYS_gettid));

    /*设置v4l2debug*/
    v4l2core_set_verbosity(debug_level);

    /*得到v4l2的句柄*/


    v4l2_dev_t *my_vd = create_v4l2_device_handler(devicename);
    if(my_vd == NULL){
        int i;
        v4l2_device_list_t* devlist = get_device_list();
        if(devlist == NULL)
        {
            return E_NO_DEVICE_ERR;
        }
        for(i = 0; i < devlist->num_devices; i++){
            my_vd = create_v4l2_device_handler(devlist->list_devices[i].device);
            if(my_vd != NULL){
                break;
            }
            else {
                continue;
            }
        }
        if(my_vd == NULL){
            char message[150];
            sprintf(message, "no video device (%s) found", my_options->device);
            options_clean();
            return E_NO_DEVICE_ERR;
        }
    }
    else{
        /*设置渲染方式*/
//        set_render_flag(render);
    }

    if(my_options->disable_libv4l2){
        v4l2core_disable_libv4l2(my_vd);
    }
    /*选择捕捉方式*/
    if(strcasecmp(my_config->capture,"read") == 0){
        v4l2core_set_capture_method(my_vd,IO_READ);
    }
    else{
        v4l2core_set_capture_method(my_vd,IO_MMAP);
    }

    /*设置软件自动对焦排序方法*/
    v4l2core_soft_autofocus_set_sort(AUTOF_SORT_QUICK);

    /*定义fps*/
    v4l2core_define_fps(my_vd, my_config->fps_num, my_config->fps_denom);

    /*设置音视频特效*/
    //set_render_fx_mask(my_config->video_fx);
    //set_audio_fx_mask(my_config->audio_fx);

//    my_config->osd_mask &= ~REND_OSD_VUMETER_MONO;
//    my_config->osd_mask &= ~REND_OSD_VUMETER_STEREO;
//    render_set_osd_mask(my_config->osd_mask);


    /*设置视频编码器*/
    if(debug_level > 1){
        printf("cheese: setting video codec to '%s'\n", my_config->video_codec);
    }
    int vcodec_ind = encoder_get_video_codec_ind_4cc(my_config->video_codec);
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
    int acodec_ind = encoder_get_audio_codec_ind_name(my_config->audio_codec);
    if(acodec_ind < 0)
    {
        char message[50];
        snprintf(message, 49, "invalid audio codec '%s' using pcm input", my_config->audio_codec);
        fprintf(stderr, "cheese: invalid audio codec '%s' using pcm input\n", my_config->audio_codec);
        acodec_ind = 0;
    }
    set_audio_codec_ind(acodec_ind);

    if(my_options->prof_filename){
        v4l2core_load_control_profile(my_vd, my_options->prof_filename);
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
        set_video_name(get_video_name());
    if(!my_config->video_path)
    set_video_path(my_config->video_path);

    /*设置照片文件保存地址*/
    if(!my_config->photo_name)
        my_config->photo_name = strdup(get_photo_name());
    if(!my_config->photo_path)
        my_config->photo_path = strdup(get_photo_path());
    set_photo_name(my_config->photo_name);
    set_photo_path(my_config->photo_path);

    if(!my_config->device_name)
        my_config->device_name = strdup("");
    if(!my_config->device_location)
        my_config->device_location = strdup(my_vd->videodevice);

    if(!my_config->device_location)
            my_config->device_location = strdup(my_vd->videodevice);

    /*设置音频接口debug等级*/
    audio_set_verbosity(debug_level);

    /*初始化音频context*/
    audio_context_t* audio_ctx = create_audio_context(audio, my_config->audio_device);
    if(audio_ctx != NULL)
    {
        my_config->audio_device = audio_get_device_index(audio_ctx);
    }
    else{
        fprintf(stderr, "cheese: couldn't get a valid audio context for the selected api - disabling audio\n");
    }

    encoder_set_verbosity(debug_level);

    config_save(config_file);
    if(config_file)
        free(config_file);

    if(!my_options->control_panel){
        int ret = E_NO_DATA;
        v4l2_device_list_t* devlist = get_device_list();
        if(devlist == NULL)
        {
            return E_NO_DEVICE_ERR;
        }
        //v4l2core_prepare_new_format(my_vd, my_config->format);
        //v4l2core_prepare_new_resolution(my_vd, my_config->width, my_config->height);
        //v4l2core_load_control_profile(my_vd,NULL);

        v4l2core_prepare_new_format(my_vd, (int)my_config->format);
        v4l2core_prepare_new_resolution(my_vd, my_config->width, my_config->height);

        if((strcmp(my_config->device_name, devlist->list_devices[my_vd->this_device].name) == 0) && (strcmp(my_config->device_location, my_vd->videodevice) == 0))
        {
            ret =v4l2core_update_old_format(my_vd,my_config->width, my_config->height, (int)my_config->format);
        }
        else {
            ret = v4l2core_update_current_format(my_vd);
        }


        if (ret != E_OK) {
            v4l2core_prepare_valid_format(my_vd);
            v4l2core_prepare_valid_resolution(my_vd);
            ret = v4l2core_update_current_format(my_vd);
            if (ret != E_OK) {
                fprintf(stderr, "cheese: also could not set the first listed stream format\n");
                fprintf(stderr, "cheese: Video capture failed\n");
            }
        }
        return ret;
    }
//    set_video_timer(MAX_REC_TIME);//设置最大录像定时器

    return E_OK;
}
#ifdef __cplusplus
}
#endif

















