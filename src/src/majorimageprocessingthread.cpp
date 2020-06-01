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

#include "majorimageprocessingthread.h"

MajorImageProcessingThread::MajorImageProcessingThread()
{
    stopped = false;
    majorindex = -1;
}

void MajorImageProcessingThread::stop()
{
    stopped = true;
}

void MajorImageProcessingThread::init()
{
    //condition = new QWaitCondition();
}

void MajorImageProcessingThread::run()
{

    v4l2_dev_t *vd1 = get_v4l2_dev();
    v4l2_frame_buff_t *frame;
    unsigned char *rgb24;

    v4l2core_start_stream(vd1);

    while (!stopped) {
        msleep(1000 / 20);
        frame = v4l2core_get_decoded_frame(vd1);
        if (frame == nullptr) {
            continue;
        }

        int result = -1;
        if (frame != nullptr) {
            result = 0;
        }

//        printf("(raw)frame->timestamp:%lu\n", static_cast<long unsigned int>(frame->timestamp));

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
            /*add the frame to the encoder buffer*/
            encoder_add_video_frame(input_frame, size, static_cast<int64_t>(frame->timestamp), frame->isKeyframe);

            //printf("(video)frame->timestamp:%llu\n", frame->timestamp);

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
        rgb24 = static_cast<unsigned char *>(malloc(frame->width * frame->height * 3 * sizeof(char)));

        convert_yuv_to_rgb_buffer(static_cast<unsigned char *>(frame->raw_frame), rgb24, static_cast<unsigned int>(frame->width), static_cast<unsigned int>(frame->height));

        QImage img;
        img = QImage(rgb24, frame->width, frame->height, QImage::Format_RGB888);
        emit SendMajorImageProcessing(img, result);

        v4l2core_release_frame(vd1, frame);
        free(rgb24);
        rgb24 = nullptr;
        msleep(1000 / 30);
    }
    v4l2core_stop_stream(vd1);

}

