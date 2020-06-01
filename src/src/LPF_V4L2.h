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

