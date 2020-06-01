#ifndef  __MAJORV4L2_H_
#define __MAJORV4L2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <locale.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#include "gviewaudio.h"
#include "gviewv4l2core.h"
#include "v4l2_devices.h"
#include "v4l2_core.h"
#include "gviewencoder.h"
#include "gviewrender.h"
#include "config.h"
#include "options.h"
#include "gui.h"

extern int debug_level;

extern uint32_t my_render_mask;

int convert_yuv_to_rgb_buffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);

void close_audio_context();

audio_context_t *create_audio_context(int api, int device);

int start_encoder_thread();

int stop_encoder_thread();

void video_capture_save_video(int value);

void reset_video_timer();

audio_context_t *get_audio_context();

void *encoder_loop();

int video_capture_get_save_video();

int get_video_codec_ind();

int get_audio_codec_ind();

int LPF_StartRun();

int LPF_StopRun();

v4l2_dev_t *get_v4l2_dev();

#ifdef __cplusplus
}
#endif
#endif

