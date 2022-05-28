/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
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
#include "datamanager.h"
#include "camera.h"

extern "C" {
#include <libimagevisualresult/visualresult.h>
}

#include <QFile>
#include <QDate>
#include <QDir>

MajorImageProcessingThread::MajorImageProcessingThread():m_bHorizontalMirror(false)
{
    m_yuvPtr = nullptr;
    m_rgbPtr = nullptr;
    m_bRecording = false;
    m_nVdWidth = 0;
    m_nVdHeight = 0;

    init();

    m_eEncodeEnv = DataManager::instance()->encodeEnv();
    if (QCamera_Env == m_eEncodeEnv)
        connect(Camera::instance(), &Camera::presentImage, this, &MajorImageProcessingThread::processingImage);
    QFile board("/sys/class/dmi/id/board_name");
    if (board.open(QIODevice::ReadOnly)) {
        QString name = board.readAll();
        m_specialTrea = name.contains("LXKT-ZXE");
        board.close();
    }
}

void MajorImageProcessingThread::stop()
{
    m_stopped = 1;
}

void MajorImageProcessingThread::init()
{
    m_stopped = 0;
    m_majorindex = -1;
    m_frame = nullptr;
    m_bTake = false;
    m_videoDevice = nullptr;
    m_result = -1;
}

void MajorImageProcessingThread::setFilter(QString filter)
{
    m_filter = filter;
}

void MajorImageProcessingThread::setExposure(int exposure)
{
    m_exposure = exposure;
}

void MajorImageProcessingThread::ImageHorizontalMirror(const uint8_t* src, uint8_t* dst, int width, int height)
{
    /*
    yu12
    y1 y2 y3 y4                       y4 y3 y2 y1
    y5 y6 y7 y8   HorizontalMirror    y8 y7 y6 y5
    u1 u2 u3 u4  =================>   u4 u3 u2 u1
    v1 v2 v3 v3                       v4 v3 v2 v1
    */
    int yLineStartIndex = 0;
    int uvLineStartIndex = width * height;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w += 2) {
            dst[yLineStartIndex + w] = src[yLineStartIndex + width - w - 1];
            dst[yLineStartIndex + w + 1] = src[yLineStartIndex + width - w - 2];
            if ((h & 1) == 0) {
                dst[uvLineStartIndex + w] = src[uvLineStartIndex + width - w - 1];
                dst[uvLineStartIndex + w + 1] = src[uvLineStartIndex + width - w - 2];
            }
        }
        yLineStartIndex += width;
        if ((h & 1) == 0) {
            uvLineStartIndex += width;
        }
    }
}

void MajorImageProcessingThread::processingImage(QImage& img)
{
    // 镜像
    if (!m_bHorizontalMirror)
        img = img.mirrored(true,false);

    if (m_bPhoto) {
        img = img.convertToFormat(QImage::Format_RGB888);

        // 滤镜预览
        m_filterImg = img.scaled(40,40,Qt::IgnoreAspectRatio);
        emit SendFilterImageProcessing(&m_filterImg);

        if (!m_filter.isEmpty() || m_exposure) {
            // 滤镜效果渲染
            uint8_t *rgb = img.bits();
            if (!m_filter.isEmpty())
                imageFilter24(rgb, img.width(), img.height(), m_filter.toStdString().c_str(), 100);
            // 曝光强度调节
            if(m_exposure)
                exposure(rgb, img.width(), img.height(), m_exposure);
        }

        /*拍照*/
        if (m_bTake) {
            int nRet = -1;
            if (!img.isNull()) {
                if (img.save(m_strPath, "JPG")) {
                    nRet = 0;
                    emit sigReflushSnapshotLabel();
                }
            }

            if (nRet < 0) {
                qWarning() << "保存照片失败";
            }

            m_bTake = false;
        }

    }

    emit SendMajorImageProcessing(&img, 0);
}

void MajorImageProcessingThread::run()
{
    if (m_eEncodeEnv != QCamera_Env) {
        m_videoDevice = get_v4l2_device_handler();
        v4l2core_start_stream(m_videoDevice);
        int framedely = 0;
        int64_t timespausestamp = 0;
        uint yuvsize = 0;
        uint rgbsize = 0;
        uint8_t* pOldYuvFrame = nullptr;
        while (m_stopped == 0) {
            if (get_resolution_status()) {
                //reset
                request_format_update(0);
                v4l2core_stop_stream(m_videoDevice);
                m_rwMtxImg.lock();
                v4l2core_clean_buffers(m_videoDevice);
                m_rwMtxImg.unlock();

                int ret = v4l2core_update_current_format(m_videoDevice);

                if (ret != E_OK) {
                    fprintf(stderr, "camera: could not set the defined stream format\n");
                    fprintf(stderr, "camera: trying first listed stream format\n");

                    v4l2core_prepare_valid_format(m_videoDevice);
                    v4l2core_prepare_valid_resolution(m_videoDevice);
                    ret = v4l2core_update_current_format(m_videoDevice);

                    if (ret != E_OK) {
                        fprintf(stderr, "camera: also could not set the first listed stream format\n");
                        stop();
                    }

                }

                v4l2core_start_stream(m_videoDevice);

                //保存新的分辨率//后续修改为标准Qt用法
                QString config_file = QString(getenv("HOME")) + QDir::separator() + QString(".config") + QDir::separator() + QString("deepin") +
                                      QDir::separator() + QString("deepin-camera") + QDir::separator() + QString("deepin-camera");

                config_load(config_file.toLatin1().data());

                config_t *my_config = config_get();

                my_config->width = static_cast<int>(m_videoDevice->format.fmt.pix.width);
                my_config->height = static_cast<int>(m_videoDevice->format.fmt.pix.height);
                my_config->format = static_cast<uint>(m_videoDevice->format.fmt.pix.pixelformat);
                v4l2_device_list_t *devlist = get_device_list();
                set_device_name(devlist->list_devices[get_v4l2_device_handler()->this_device].name);
                config_save(config_file.toLatin1().data());
            }

            m_result = -1;
            m_frame = v4l2core_get_decoded_frame(m_videoDevice);

            if (m_frame == nullptr) {
                framedely++;
                if (framedely == MAX_DELAYED_FRAMES) {
                    m_stopped = 1;
                    //发送设备中断信号
                    emit reachMaxDelayedFrames();
                    m_filterImg =  QImage();
                    emit SendFilterImageProcessing(&m_filterImg);
    //                close_v4l2_device_handler();
                }

                continue;
            }

            QImage jpgImage;
            if (FFmpeg_Env == m_eEncodeEnv) {
                // FFmpeg环境下，解码后的帧数据为yu12格式
                if (m_nVdWidth != static_cast<unsigned int>(m_frame->width) || m_nVdHeight != static_cast<unsigned int>(m_frame->height)) {
                    m_nVdWidth = static_cast<unsigned int>(m_frame->width);
                    m_nVdHeight = static_cast<unsigned int>(m_frame->height);
                    if (m_yuvPtr != nullptr) {
                        delete [] m_yuvPtr;
                        m_yuvPtr = nullptr;
                    }

                    yuvsize = m_nVdWidth * m_nVdHeight * 3 / 2;
                    m_yuvPtr = new uchar[yuvsize];
                } else {
                    yuvsize = m_nVdWidth * m_nVdHeight * 3 / 2;
                }

                if (m_bHorizontalMirror) {
                    ImageHorizontalMirror(m_frame->yuv_frame, m_yuvPtr,m_frame->width,m_frame->height);
                } else {
                    memcpy(m_yuvPtr, m_frame->yuv_frame, yuvsize);
                }
                pOldYuvFrame = m_frame->yuv_frame;
                m_frame->yuv_frame = m_yuvPtr;
            } else if (GStreamer_Env == m_eEncodeEnv) {
                // GStreamer环境下，获取的帧数据为jpg格式，需要转换为rgb格式，GStreamer底层才能处理
                QByteArray temp;
                temp.append((const char *)m_frame->raw_frame, m_frame->raw_frame_max_size);
                jpgImage.loadFromData(temp);
                jpgImage = jpgImage.convertToFormat(QImage::Format_RGB888);
                if (m_bHorizontalMirror)
                    jpgImage = jpgImage.mirrored(true, false);
            }

            // 判断是否使用rgb数据
            bool bUseRgb = false;
    #ifdef __mips__
            bUseRgb = true;
    #endif
            if (get_wayland_status())
                bUseRgb = true;

            if (!m_filter.isEmpty() || m_exposure)
                bUseRgb = true;

            // GStreamer环境下，使用rgb格式显示帧数据
            if (GStreamer_Env == m_eEncodeEnv)
                bUseRgb = true;

            if (m_specialTrea)
                bUseRgb = true;

            if (bUseRgb || (m_bPhoto && m_filtersGroupDislay)) {
                if (m_nVdWidth != static_cast<unsigned int>(m_frame->width) || m_nVdHeight != static_cast<unsigned int>(m_frame->height)) {
                    m_nVdWidth = static_cast<unsigned int>(m_frame->width);
                    m_nVdHeight = static_cast<unsigned int>(m_frame->height);
                    if (m_rgbPtr != nullptr) {
                        free(m_rgbPtr);
                        m_rgbPtr = nullptr;
                    }

                    rgbsize = m_nVdWidth * m_nVdHeight * 3;
                    m_rgbPtr = static_cast<uint8_t *>(calloc(rgbsize, sizeof(uint8_t)));
                } else {
                    rgbsize = m_nVdWidth * m_nVdHeight * 3;
                    if (nullptr == m_rgbPtr)
                        m_rgbPtr = static_cast<uint8_t *>(calloc(rgbsize, sizeof(uint8_t)));
                }

                if (FFmpeg_Env == m_eEncodeEnv) {
                    // yu12到rgb数据高性能转换
                    yu12_to_rgb24_higheffic(m_rgbPtr, m_frame->yuv_frame, m_frame->width, m_frame->height);
                }
                else if (GStreamer_Env == m_eEncodeEnv) {
                    Q_ASSERT(m_rgbPtr);
                    memset(m_rgbPtr, 0, rgbsize * sizeof(uint8_t));
                    memcpy(m_rgbPtr, jpgImage.bits(), rgbsize);
                }
                m_filterImg = QImage(m_rgbPtr, m_frame->width, m_frame->height, QImage::Format_RGB888).scaled(40,40,Qt::IgnoreAspectRatio);

                // 拍照状态下，曝光和滤镜功能才有效
                if (m_bPhoto) {
                    // 滤镜效果渲染
                    if (!m_filter.isEmpty())
                        imageFilter24(m_rgbPtr, m_frame->width, m_frame->height, m_filter.toStdString().c_str(), 100);
                    // 曝光强度调节
                    if(m_exposure)
                        exposure(m_rgbPtr, m_frame->width, m_frame->height, m_exposure);
                }
            }

            /*录像*/
            if (video_capture_get_save_video()) {

                if (get_myvideo_bebin_timer() == 0)
                    set_myvideo_begin_timer(v4l2core_time_get_timestamp());

                int size = (m_frame->width * m_frame->height * 3) / 2;
                uint8_t *input_frame = m_frame->yuv_frame;

                /*
                 * TODO: check codec_id, format and frame flags
                 * (we may want to store a compressed format
                 */
                if (get_video_codec_ind() == 0) { //raw frame
                    switch (v4l2core_get_requested_frame_format(m_videoDevice)) {
                    case  V4L2_PIX_FMT_H264:
                        input_frame = m_frame->h264_frame;
                        size = static_cast<int>(m_frame->h264_frame_size);
                        break;
                    default:
                        input_frame = m_frame->raw_frame;
                        size = static_cast<int>(m_frame->raw_frame_size);
                        break;
                    }

                }

                /*把帧加入编码队列*/
                if (!get_capture_pause()) {
                    //设置时间戳
                    set_video_timestamptmp(static_cast<int64_t>(m_frame->timestamp));
                    encoder_add_video_frame(input_frame, size, static_cast<int64_t>(m_frame->timestamp), m_frame->isKeyframe);
                } else {
                    //设置暂停时长
                    timespausestamp = get_video_timestamptmp();
                    if (timespausestamp == 0) {
                        set_video_pause_timestamp(0);
                    } else {
                        set_video_pause_timestamp(static_cast<int64_t>(m_frame->timestamp) - timespausestamp);
                    }

                }

                /*
                 * exponencial scheduler
                 *  with 50% threshold (milisec)
                 *  and max value of 250 ms (4 fps)
                 */
                double time_sched = encoder_buff_scheduler(ENCODER_SCHED_LIN, 0.5, 250);
                if (time_sched > 0) {
                    switch (v4l2core_get_requested_frame_format(m_videoDevice)) {
                    case V4L2_PIX_FMT_H264: {
                        uint32_t framerate = static_cast<uint32_t>(lround(time_sched * 1E6)); /*nanosec*/
                        v4l2core_set_h264_frame_rate_config(m_videoDevice, framerate);
                        break;
                    }

                    default: {
                        struct timespec req = {0, static_cast<__syscall_slong_t>(time_sched * 1E6)}; /*nanosec*/
                        nanosleep(&req, nullptr);
                        break;
                    }
                    }

                }

            } else if (m_bRecording) {
                // GStreamer环境下，发送rgb格式帧数据到视频写入器，完成后续视频编码任务
                emit sigRecordFrame(m_rgbPtr, rgbsize);
            }

            QImage* imgTmp = nullptr;
            if (m_rgbPtr)
                imgTmp = new QImage(m_rgbPtr, m_frame->width, m_frame->height, QImage::Format_RGB888);

            /*拍照*/
            if (m_bTake) {
                int nRet = -1;
                if (((!m_filter.isEmpty() || m_exposure) && imgTmp)
                        || GStreamer_Env == m_eEncodeEnv) {
                    if (imgTmp->save(m_strPath, "JPG")) {
                        nRet = 0;
                        emit sigReflushSnapshotLabel();
                    }
                } else if (FFmpeg_Env == m_eEncodeEnv)
                    nRet = v4l2core_save_image(m_frame, m_strPath.toStdString().c_str(), IMG_FMT_JPG);

                if (nRet < 0) {
                    qWarning() << "保存照片失败";
                }

                m_bTake = false;
            }

            m_result = 0;
            framedely = 0;
            m_rwMtxImg.lock();
            if (m_stopped == 0) {
                if (bUseRgb) {
                    if (imgTmp && !imgTmp->isNull()) {
                        m_Img = imgTmp->copy();
                        emit SendMajorImageProcessing(&m_Img, m_result);
                    }
                } else if (m_frame->yuv_frame){
    #ifndef __mips__
                    emit sigRenderYuv(true);
                    emit sigYUVFrame(m_yuvPtr, m_nVdWidth, m_nVdHeight);
    #endif
                }

                malloc_trim(0);
            }

            emit SendFilterImageProcessing(&m_filterImg);

    #ifndef __mips__
            if (m_frame->yuv_frame == nullptr) {
                emit sigRenderYuv(false);
            }

    #endif
            m_rwMtxImg.unlock();

            if (imgTmp)
                delete imgTmp;

            m_frame->yuv_frame = pOldYuvFrame;
            v4l2core_release_frame(m_videoDevice, m_frame);
    #ifdef UNITTEST
            break;
    #endif
        }

        v4l2core_stop_stream(m_videoDevice);
    }
}

MajorImageProcessingThread::~MajorImageProcessingThread()
{

    if (m_yuvPtr) {
        delete [] m_yuvPtr;
        m_yuvPtr = nullptr;
    }

    if (m_rgbPtr) {
        free(m_rgbPtr);
        m_rgbPtr = nullptr;
    }

    config_clean();
    qDebug() << "~MajorImageProcessingThread";
}
