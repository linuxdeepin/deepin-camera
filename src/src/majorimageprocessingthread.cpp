/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
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

#include "majorimageprocessingthread.h"


MajorImageProcessingThread::MajorImageProcessingThread()
{
    init();
}

void MajorImageProcessingThread::stop()
{
    stopped = true;
}

void MajorImageProcessingThread::init()
{
    stopped = false;
    majorindex = -1;
}

void MajorImageProcessingThread::run()
{
    vd1 = get_v4l2_device_handler();
    v4l2core_start_stream(vd1);
    int framedely = 0;
    while (!stopped) {
        msleep(1000 / 20);
        result = -1;
        frame = v4l2core_get_decoded_frame(vd1);
        if (frame == nullptr) {
            framedely++;
            if (framedely == MAX_DELAYED_FRAMES) {
                stopped = true;
                emit reachMaxDelayedFrames();
                //free(vd1);
                //vd1 = nullptr;
                close_v4l2_device_handler();
                return ;
            }
            continue;
        }
        result = 0;
        framedely = 0;
        if (video_capture_get_save_video()) {
            int size = (frame->width * frame->height * 3) / 2;

            uint8_t *input_frame = frame->yuv_frame;

            /*
             * TODO: check codec_id, format and frame flags
             * (we may want to store a compressed format
             */
            if (get_video_codec_ind() == 0) { //raw frame
                switch (v4l2core_get_requested_frame_format(vd1)) {
                case  V4L2_PIX_FMT_H264:
                    input_frame = frame->h264_frame;
                    size = static_cast<int>(frame->h264_frame_size);
                    break;
                default:
                    input_frame = frame->raw_frame;
                    size = static_cast<int>(frame->raw_frame_size);
                    break;
                }
            }
            /*把帧加入编码队列*/
            encoder_add_video_frame(input_frame, size, static_cast<int64_t>(frame->timestamp), frame->isKeyframe);

            /*
             * exponencial scheduler
             *  with 50% threshold (milisec)
             *  and max value of 250 ms (4 fps)
             */
            double time_sched = encoder_buff_scheduler(ENCODER_SCHED_LIN, 0.5, 250);
            if (time_sched > 0) {
                switch (v4l2core_get_requested_frame_format(vd1)) {
                case V4L2_PIX_FMT_H264: {
                    uint32_t framerate = static_cast<uint32_t>(lround(time_sched * 1E6)); /*nanosec*/
                    v4l2core_set_h264_frame_rate_config(vd1, framerate);
                    break;
                }
                default: {
                    struct timespec req = {0, static_cast<__syscall_slong_t>(time_sched * 1E6)}; /*nanosec*/
                    nanosleep(&req, nullptr);
                    break;
                }
                }
            }
        }
        if (!stopped) {
            rgb24 = static_cast<unsigned char *>(malloc(frame->width * frame->height * 3 * sizeof(char)));

            convert_yuv_to_rgb_buffer(static_cast<unsigned char *>(frame->raw_frame), rgb24, static_cast<unsigned int>(frame->width), static_cast<unsigned int>(frame->height));


            m_img = QImage(rgb24, frame->width, frame->height, QImage::Format_RGB888);

            emit SendMajorImageProcessing(m_img, result);
        }
        v4l2core_release_frame(vd1, frame);
        free(rgb24);
        rgb24 = nullptr;
        msleep(1000 / 30);
    }
    v4l2core_stop_stream(vd1);
}

