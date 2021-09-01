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

#ifndef  __MAJORV4L2_H_
#define __MAJORV4L2_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gviewaudio.h"
#include "gviewv4l2core.h"
#include "v4l2_devices.h"
#include "v4l2_core.h"
#include "gviewencoder.h"
#include "gviewrender.h"
#include "cameraconfig.h"
#include "options.h"
#include "gui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <locale.h>
#include <fcntl.h>

#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <linux/videodev2.h>
#include "rkisp_api.h"
/**
* @brief camInit　相机初始化
*/
int camInit(const char *device_name);



void close_rkisp_ctx();

void start_rkisp_capture();

void stop_rkisp_capture();

void create_rkisp_ctx(const char *device);

const struct rkisp_api_buf* get_rkisp_frame(int timeout_ms);

void put_rkisp_frame(const struct rkisp_api_buf *buf);

int get_rkisp_ctx_width();

int get_rkisp_ctx_height();

char *get_rkisp_ctx_device();

#ifdef __cplusplus
}
#endif
#endif

