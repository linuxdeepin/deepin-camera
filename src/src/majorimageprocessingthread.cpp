// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "majorimageprocessingthread.h"
#include "datamanager.h"
#include "camera.h"

extern "C" {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <libimagevisualresult6/visualresult.h>
#else
    #include <libimagevisualresult/visualresult.h>
#endif
}

#include <QFile>
#include <QDate>
#include <QDir>
#include <DSysInfo>
DCORE_USE_NAMESPACE

MajorImageProcessingThread::MajorImageProcessingThread():m_bHorizontalMirror(false)
{
    qDebug() << "Initializing MajorImageProcessingThread";
    m_yuvPtr = nullptr;
    m_rgbPtr = nullptr;
    m_bRecording = false;
    m_nVdWidth = 0;
    m_nVdHeight = 0;

    init();

    m_eEncodeEnv = DataManager::instance()->encodeEnv();
    if (QCamera_Env == m_eEncodeEnv)
        connect(Camera::instance(), &Camera::presentImage, this, &MajorImageProcessingThread::processingImage);
}

void MajorImageProcessingThread::stop()
{
    qDebug() << __func__;
    m_stopped = 1;
}

void MajorImageProcessingThread::init()
{
    qDebug() << "Initializing thread state";
    m_stopped = 0;
    m_majorindex = -1;
    m_frame = nullptr;
    m_bTake = false;
    m_videoDevice = nullptr;
    m_result = -1;
    m_nCount = 0;
    m_firstPts = 0;
}

void MajorImageProcessingThread::setFilter(QString filter)
{
    // qDebug() << "Setting filter:" << filter;
    m_filter = filter;
}

void MajorImageProcessingThread::setExposure(int exposure)
{
    // qDebug() << "Setting exposure value:" << exposure;
    m_exposure = exposure;
}

int MajorImageProcessingThread::getRecCount()
{
    // qDebug() << "Getting recording count:" << m_nCount;
    return m_nCount;
}

void MajorImageProcessingThread::ImageHorizontalMirror(const uint8_t* src, uint8_t* dst, int width, int height)
{
    qDebug() << "Function started: ImageHorizontalMirror";
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
    qDebug() << "Function completed: ImageHorizontalMirror";
}

void MajorImageProcessingThread::processingImage(QImage& img)
{
    qDebug() << "Processing image, size:" << img.size() << "format:" << img.format();
    // 镜像
    if (!m_bHorizontalMirror)
        img = img.mirrored(true,false);

    if (m_bPhoto) {
        qDebug() << "Processing image in photo mode";
        img = img.convertToFormat(QImage::Format_RGB888);

        // 滤镜预览
        m_filterImg = img.scaled(40,40,Qt::IgnoreAspectRatio);
        emit SendFilterImageProcessing(&m_filterImg);

        if (!m_filter.isEmpty() || m_exposure) {
            qDebug() << "Applying filter:" << m_filter << "and exposure:" << m_exposure;
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
            qDebug() << "Processing photo in photo mode";
            int nRet = -1;
            if (!img.isNull()) {
                if (img.save(m_strPath, "JPG")) {
                    nRet = 0;
                    qInfo() << "Successfully saved photo to:" << m_strPath;
                    emit sigReflushSnapshotLabel(m_strPath);
                }
            }

            if (nRet < 0) {
                qWarning() << "Failed to save photo to:" << m_strPath;
            }

            m_bTake = false;
        }

    }

    emit SendMajorImageProcessing(&img, 0);
    qDebug() << "Function completed: processingImage";
}

void MajorImageProcessingThread::run()
{
    qInfo() << "Starting MajorImageProcessingThread run loop";
    if (m_eEncodeEnv != QCamera_Env) {
        qDebug() << "Processing video frame in video mode";
        m_videoDevice = get_v4l2_device_handler();
        v4l2core_start_stream(m_videoDevice);
        int framedely = 0;
        int64_t timespausestamp = 0;
        uint yuvsize = 0;
        uint rgbsize = 0;
        uint8_t* pOldYuvFrame = nullptr;
        while (m_stopped == 0) {
            if (get_resolution_status()) {
                // qDebug() << "Resolution change detected, updating format";
                //reset
                request_format_update(0);
                v4l2core_stop_stream(m_videoDevice);
                m_rwMtxImg.lock();
                v4l2core_clean_buffers(m_videoDevice);
                m_rwMtxImg.unlock();

                int ret = v4l2core_update_current_format(m_videoDevice);

                if (ret != E_OK) {
                    qWarning() << "Failed to set defined stream format, trying first listed format";
                    fprintf(stderr, "camera: could not set the defined stream format\n");
                    fprintf(stderr, "camera: trying first listed stream format\n");

                    v4l2core_prepare_valid_format(m_videoDevice);
                    v4l2core_prepare_valid_resolution(m_videoDevice);
                    ret = v4l2core_update_current_format(m_videoDevice);

                    if (ret != E_OK) {
                        qCritical() << "Failed to set first listed stream format, stopping thread";
                        fprintf(stderr, "camera: also could not set the first listed stream format\n");
                        stop();
                    }
                }

                v4l2core_start_stream(m_videoDevice);
                // qDebug() << "Stream restarted with new format";

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
                // qDebug() << "Saved new resolution config:" << my_config->width << "x" << my_config->height;
            }

            m_result = -1;
            if (config_get()->verbosity > 0) {
                static QElapsedTimer decodeTimer;
                static int decodeCount = 0;
                if (decodeCount % LOGGING_INTERVAL == LOGGING_INTERVAL - 1) {
                    decodeTimer.start();
                    m_frame = v4l2core_get_decoded_frame(m_videoDevice);
                    qDebug() << "v4l2 decoded time:" << decodeTimer.elapsed() << "ms";
                    decodeCount = 0;
                } else {
                    m_frame = v4l2core_get_decoded_frame(m_videoDevice);
                    decodeCount++;
                }
            } else {
                m_frame = v4l2core_get_decoded_frame(m_videoDevice);
            }

            if (m_frame == nullptr) {
                framedely++;
                if (framedely == MAX_DELAYED_FRAMES) {
                    // qWarning() << "Reached maximum delayed frames, stopping thread";
                    m_stopped = 1;
                    //发送设备中断信号
                    emit reachMaxDelayedFrames();
                    m_filterImg =  QImage();
                    emit SendFilterImageProcessing(&m_filterImg);
                }
                continue;
            }

            QImage jpgImage;
            if (FFmpeg_Env == m_eEncodeEnv) {
                // qDebug() << "Processing video frame in FFmpeg environment";
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
                    if (m_rgbPtr != nullptr) {
                        free(m_rgbPtr);
                        m_rgbPtr = nullptr;
                    }

                    rgbsize = m_nVdWidth * m_nVdHeight * 3;
                    m_rgbPtr = static_cast<uint8_t *>(calloc(rgbsize, sizeof(uint8_t)));
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
                // qDebug() << "Processing video frame in GStreamer environment";
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
#if defined(_loongarch) || defined(__loongarch__) || defined(__loongarch64) || defined (__mips__)
            bUseRgb = true;
#endif
            if(DSysInfo::majorVersion().toInt() >= 23) {
                bUseRgb = true;
            }
            if (get_wayland_status())
                bUseRgb = true;

            if (!m_filter.isEmpty() || m_exposure)
                bUseRgb = true;

            // GStreamer环境下，使用rgb格式显示帧数据
            if (GStreamer_Env == m_eEncodeEnv)
                bUseRgb = true;

            if (bUseRgb || (m_bPhoto && m_filtersGroupDislay)) {
                // qDebug() << "Processing video frame in rgb mode";
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
                } else if (GStreamer_Env == m_eEncodeEnv) {
                    Q_ASSERT(m_rgbPtr);
                    memset(m_rgbPtr, 0, rgbsize * sizeof(uint8_t));
                    memcpy(m_rgbPtr, jpgImage.bits(), rgbsize);
                }
                m_filterImg = QImage(m_rgbPtr, m_frame->width, m_frame->height, QImage::Format_RGB888).scaled(40,40,Qt::IgnoreAspectRatio);

                // 拍照状态下，曝光和滤镜功能才有效
                if (m_bPhoto) {
                    // qDebug() << "Processing photo in photo mode";
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
                // qDebug() << "Processing video frame for recording";
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
                    // qDebug() << "Processing video frame for recording";
                    //设置时间戳
                    set_video_timestamptmp(static_cast<int64_t>(m_frame->timestamp));

                    if (m_firstPts == 0) {
                        m_firstPts = m_frame->timestamp;
                        // qDebug() << "First video frame timestamp:" << m_firstPts;
                    }
                    m_nCount = (m_frame->timestamp - m_firstPts) / 1000000000;

                    lasttimestamp = m_frame->timestamp;
                    encoder_add_video_frame(input_frame, size, static_cast<int64_t>(m_frame->timestamp), m_frame->isKeyframe);
                } else {
                    // qDebug() << "Processing video frame for recording";
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
                    // qDebug() << "Processing video frame for recording";
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
                // qDebug() << "Processing video frame for recording";
                // GStreamer环境下，发送rgb格式帧数据到视频写入器，完成后续视频编码任务
                emit sigRecordFrame(m_rgbPtr, rgbsize);
            } else {
                // qDebug() << "Processing video frame for recording";
                m_nCount = 0;
                m_firstPts = 0;
            }

            QImage* imgTmp = nullptr;
            if (m_rgbPtr)
                imgTmp = new QImage(m_rgbPtr, m_frame->width, m_frame->height, QImage::Format_RGB888);

            /*拍照*/
            if (m_bTake) {
                // qDebug() << "Processing photo capture";
                int nRet = -1;
                if (((!m_filter.isEmpty() || m_exposure) && imgTmp)
                        || GStreamer_Env == m_eEncodeEnv) {
                    // qDebug() << "Processing photo capture";
                    if (imgTmp->save(m_strPath, "JPG")) {
                        nRet = 0;
                        qInfo() << "Successfully saved photo to:" << m_strPath;
                        emit sigReflushSnapshotLabel(m_strPath);
                    }
                } else if (FFmpeg_Env == m_eEncodeEnv) {
                    // qDebug() << "Processing photo capture";
                    uint8_t *rgbPtr = nullptr;
                    uint nVdWidth = static_cast<unsigned int>(m_frame->width);
                    uint nVdHeight = static_cast<unsigned int>(m_frame->height);

                    rgbsize = nVdWidth * nVdHeight * 3;
                    rgbPtr = static_cast<uint8_t *>(calloc(rgbsize, sizeof(uint8_t)));

                    yu12_to_rgb24_higheffic(rgbPtr, m_frame->yuv_frame, m_frame->width, m_frame->height);
                    QImage* saveImg = nullptr;
                    if (rgbPtr)
                        saveImg = new QImage(rgbPtr, m_frame->width, m_frame->height, QImage::Format_RGB888);
                    if (saveImg->save(m_strPath, "JPG")) {
                        nRet = 0;
                        qInfo() << "Successfully saved photo to:" << m_strPath;
                        emit sigReflushSnapshotLabel(m_strPath);
                    }
                    if (saveImg)
                        delete saveImg;

                    if (rgbPtr != nullptr) {
                        free(rgbPtr);
                        rgbPtr = nullptr;
                    }
                }

                if (nRet < 0) {
                    qWarning() << "Failed to save photo to:" << m_strPath;
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
            //保证画面流畅的前提下降低刷新率
            if(m_nVdWidth <= 1920) {
                msleep(33);
            }
        }

        // qDebug() << "Stopping video stream";
        v4l2core_stop_stream(m_videoDevice);
    }
}

MajorImageProcessingThread::~MajorImageProcessingThread()
{
    // qDebug() << "Cleaning up MajorImageProcessingThread resources";
    if (m_yuvPtr) {
        // qDebug() << "Cleaning up MajorImageProcessingThread resources";
        delete [] m_yuvPtr;
        m_yuvPtr = nullptr;
    }

    if (m_rgbPtr) {
        // qDebug() << "Cleaning up MajorImageProcessingThread resources";
        free(m_rgbPtr);
        m_rgbPtr = nullptr;
    }

    config_clean();
    // qDebug() << "MajorImageProcessingThread cleanup complete";
}
