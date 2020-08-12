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

#include "majorimageprocessingthread.h"



MajorImageProcessingThread::MajorImageProcessingThread()
{
    init();
}

MajorImageProcessingThread::~MajorImageProcessingThread()
{
    if (m_img) {
        delete m_img;
    }
    if (frame) {

    }
    vd1 = get_v4l2_device_handler();
    if (vd1 != nullptr) {
        stopped = true;
        if (video_capture_get_save_video() > 0) {
            qDebug() << "stop_encoder_thread";
            stop_encoder_thread();
        }
        qDebug() << "close_v4l2_device_handler";
        close_v4l2_device_handler();
    }
    qDebug() << "~MajorImageProcessingThread";
}

void MajorImageProcessingThread::stop()
{
    stopped = true;
}

void MajorImageProcessingThread::init()
{
    stopped = false;
    majorindex = -1;
    m_img = new QPixmap();
    frame = nullptr;
}

void MajorImageProcessingThread::run()
{
    vd1 = get_v4l2_device_handler();
    v4l2core_start_stream(vd1);
    int framedely = 0;
    while (!stopped) {
        if (get_resolution_status()) {
//            int current_width = v4l2core_get_frame_width(vd1);
//            int current_height = v4l2core_get_frame_height(vd1);
            request_format_update(0); /*reset*/
            v4l2core_stop_stream(vd1);
            m_rwMtxImg.lock();
            v4l2core_clean_buffers(vd1);
            m_rwMtxImg.unlock();
            /*try new format (values prepared by the request callback)*/
            int ret = v4l2core_update_current_format(vd1);


            /*try to set the video stream format on the device*/
            if (ret != E_OK) {
                fprintf(stderr, "camera: could not set the defined stream format\n");
                fprintf(stderr, "camera: trying first listed stream format\n");

                v4l2core_prepare_valid_format(vd1);
                v4l2core_prepare_valid_resolution(vd1);
                ret = v4l2core_update_current_format(vd1);

                if (ret != E_OK) {
                    fprintf(stderr, "camera: also could not set the first listed stream format\n");
                    stop();
                }
            }

            options_t *my_options = options_get();
            char *device_name = get_file_basename(my_options->device);
            QString config_file = QString(getenv("HOME")) + QString("/") + QString(".config/deepin-camera/") + QString(device_name);
            free(device_name);
            config_load(config_file.toLatin1().data());

            config_t *my_config = config_get();

            my_config->width = static_cast<int>(vd1->format.fmt.pix.width);
            my_config->height = static_cast<int>(vd1->format.fmt.pix.height);
            v4l2_device_list_t *devlist = get_device_list();
            set_device_name(devlist->list_devices[get_v4l2_device_handler()->this_device].name);
            config_save(config_file.toLatin1().data());
            v4l2core_start_stream(vd1);
        }
        result = -1;
        frame = v4l2core_get_decoded_frame(vd1);
        m_rwMtxImg.lock();
//        if (frame->width == 752 && frame->height == 423) {
//            v4l2core_save_image(frame, m_strPath.toStdString().c_str(), IMG_FMT_JPG);
//        }

        if (frame == nullptr) {
            framedely++;
            if (framedely == MAX_DELAYED_FRAMES) {
                stopped = true;
                //发送设备中断信号
                emit reachMaxDelayedFrames();
                close_v4l2_device_handler();
            }
            continue;
        }
        result = 0;
        framedely = 0;
        if (frame->raw_frame != nullptr) {
            if (m_img->loadFromData(frame->raw_frame, static_cast<uint>(frame->raw_frame_size)) == true) {
                emit SendMajorImageProcessing(m_img, result);
            }
        }
        m_rwMtxImg.unlock();

        if (video_capture_get_save_video()) {
//            if (get_myvideo_bebin_timer() == 0) {
//                set_myvideo_begin_timer(v4l2core_time_get_timestamp());
//            }
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
            if (!get_capture_pause()) {
                set_video_timestamptmp(static_cast<int64_t>(frame->timestamp));
                encoder_add_video_frame(input_frame, size, static_cast<int64_t>(frame->timestamp), frame->isKeyframe);
            } else {
                //设置暂停时长
                int64_t timespausestamp = get_video_timestamptmp();
                set_video_pause_timestamp(static_cast<int64_t>(frame->timestamp) - timespausestamp);
            }
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
        if (m_bTake) {
            int nRet = v4l2core_save_image(frame, m_strPath.toStdString().c_str(), IMG_FMT_JPG);
            if (nRet < 0) {
                qDebug() << "保存照片失败";
            }
            m_bTake = false;
        }
        v4l2core_release_frame(vd1, frame);
        msleep(33);//1000 / 30
    }
    v4l2core_stop_stream(vd1);
}

