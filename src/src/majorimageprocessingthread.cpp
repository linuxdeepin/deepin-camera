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
#include <QFile>
#include <QDate>
#include <QDir>

MajorImageProcessingThread::MajorImageProcessingThread()
{
    init();
}

MajorImageProcessingThread::~MajorImageProcessingThread()
{
    if (m_img) {
        delete m_img;
    }
    vd1 = get_v4l2_device_handler();
    if (vd1 != nullptr) {
        if (frame != nullptr) {
            v4l2core_release_frame(vd1, frame);
        }
        stopped = 1;
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
    stopped = 1;
}

void MajorImageProcessingThread::init()
{
    stopped = 0;
    majorindex = -1;
    m_img = new QImage();
    frame = nullptr;
}

void MajorImageProcessingThread::run()
{
    vd1 = get_v4l2_device_handler();
    v4l2core_start_stream(vd1);
    int framedely = 0;
    int nID = 0;
    int64_t timespausestamp = 0;
    while (stopped == 0) {
        if (get_resolution_status()) {
//            int current_width = v4l2core_get_frame_width(vd1);
//            int current_height = v4l2core_get_frame_height(vd1);
            request_format_update(0); /*reset*/
            v4l2core_stop_stream(vd1);
            m_rwMtxImg.lock();
            v4l2core_clean_buffers(vd1);
//            framedely = -11;
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

//            options_t *my_options = options_get();
//            char *device_name = get_file_basename(my_options->device);
            QString config_file = QString(getenv("HOME")) + QString("/") + QString(".config/deepin/deepin-camera/") + QString("deepin-camera");
//            free(device_name);
            config_load(config_file.toLatin1().data());

            config_t *my_config = config_get();

            my_config->width = static_cast<int>(vd1->format.fmt.pix.width);
            my_config->height = static_cast<int>(vd1->format.fmt.pix.height);
            my_config->format = static_cast<uint>(vd1->format.fmt.pix.pixelformat);
            v4l2_device_list_t *devlist = get_device_list();
            set_device_name(devlist->list_devices[get_v4l2_device_handler()->this_device].name);
            config_save(config_file.toLatin1().data());
//            msleep(1000);//1000 / 30
            v4l2core_start_stream(vd1);
        }
        result = -1;
        frame = v4l2core_get_decoded_frame(vd1);

//        if (frame->width == 752 && frame->height == 423) {
//            v4l2core_save_image(frame, m_strPath.toStdString().c_str(), IMG_FMT_JPG);
//        }

        if (frame == nullptr) {
            framedely++;
            if (framedely == MAX_DELAYED_FRAMES) {
                stopped = 1;
                //发送设备中断信号
                emit reachMaxDelayedFrames();
                close_v4l2_device_handler();
            }
            continue;
        }


        if (video_capture_get_save_video()) {
            if (get_myvideo_bebin_timer() == 0) {
                set_myvideo_begin_timer(v4l2core_time_get_timestamp());
            }
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
                //设置时间戳
                set_video_timestamptmp(static_cast<int64_t>(frame->timestamp));
                encoder_add_video_frame(input_frame, size, static_cast<int64_t>(frame->timestamp), frame->isKeyframe);
            } else {
                //设置暂停时长
                timespausestamp = get_video_timestamptmp();
                if(timespausestamp == 0){
                    set_video_pause_timestamp(0);
                }else {
                    set_video_pause_timestamp(static_cast<int64_t>(frame->timestamp) - timespausestamp);
                }
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
//        if (frame && framedely < 0) {

//            framedely++;
//            v4l2core_release_frame(vd1, frame);
//            msleep(33);//1000 / 30
//            continue;
//        }
        jpeg_encoder_ctx_t *jpeg_ctx;

            jpeg_ctx = static_cast<jpeg_encoder_ctx_t *>(calloc(1, sizeof(jpeg_encoder_ctx_t)));

        if (jpeg_ctx == nullptr) {
            fprintf(stderr, "V4L2_CORE: FATAL memory allocation failure (save_image_jpeg): %s\n", strerror(errno));
            jpeg_ctx = nullptr;
            v4l2core_release_frame(vd1, frame);
            continue;
        }

        uint8_t *jpeg = static_cast<uint8_t *>(calloc(static_cast<size_t>(frame->width * frame->height) >> 1, sizeof(uint8_t)));

        if (jpeg == nullptr) {
            fprintf(stderr, "V4L2_CORE: FATAL memory allocation failure (save_image_jpeg): %s\n", strerror(errno));
            free(jpeg_ctx);
            jpeg_ctx = nullptr;
            jpeg = nullptr;
            v4l2core_release_frame(vd1, frame);
            continue;
        }

        initialization (jpeg_ctx, frame->width, frame->height);
        initialize_quantization_tables (jpeg_ctx);

        int jpeg_size = encode_jpeg(frame->yuv_frame, jpeg, jpeg_ctx, 1);

        result = 0;
        framedely = 0;
        m_rwMtxImg.lock();
        if (frame->yuv_frame != nullptr && (stopped == 0)) {
            if (m_img->loadFromData(jpeg, jpeg_size)) {
                emit SendMajorImageProcessing(m_img, result);
            } else {
                qDebug() << "loadFromData error";
                if (nID%30 == 0) {//1s记录一个就行
                    QString strFile = QString(getenv("HOME")) + QString("/") +
                            QString(".cache/deepin/deepin-camera/") +
                            QDateTime::currentDateTime().toString("yyyyMMddHHmmss") +
                            "_" + QString::number(nID) + "_" +
                            QString::number(static_cast<int>(frame->raw_frame_size)) + ".dat";

                    QFile file(strFile);
                    if (!(file.open(QIODevice::WriteOnly | QIODevice::Truncate))) {
                        qDebug() << "save file open false";
                    }
                    QDataStream aStream(&file);
                    aStream.setVersion(QDataStream::Qt_5_11);
                    aStream.writeRawData((char*)frame->raw_frame,static_cast<int>(frame->raw_frame_size));
                    file.close();
                }
                nID ++;
            }
            malloc_trim(0);
        }
        m_rwMtxImg.unlock();


        free(jpeg);
        free(jpeg_ctx);
        jpeg = nullptr;
        jpeg_ctx = nullptr;

        v4l2core_release_frame(vd1, frame);
        msleep(33);//1000 / 30
    }

    v4l2core_stop_stream(vd1);
}

