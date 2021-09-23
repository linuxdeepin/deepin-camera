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

#include "rga/im2d.h"
#include "rga/rga.h"

#include <QFile>
#include <QDate>
#include <QDir>
int g_exchangeWidthHeight = 1;
MajorImageProcessingThread::MajorImageProcessingThread()
{
    m_yuvPtr = nullptr;
    init();
    m_rotation = IM_HAL_TRANSFORM_ROT_270;
    m_frame = new v4l2_frame_buff_t;
}

MajorImageProcessingThread::~MajorImageProcessingThread()
{
    delete m_frame;
    if (m_yuvPtr) {
        delete [] m_yuvPtr;
        m_yuvPtr = nullptr;
    }

    //config_clean();
    qDebug() << "~MajorImageProcessingThread";
}

void MajorImageProcessingThread::stop()
{
    m_stopped = 1;
}

void MajorImageProcessingThread::init()
{
    m_stopped = 0;
    m_majorindex = -1;
    m_bTake = false;
    m_videoDevice = nullptr;
    m_result = -1;
    m_isChanging = true;
}
void MajorImageProcessingThread::setRotation(int rotation)
{
    g_exchangeWidthHeight = 1;
    switch (rotation) {
    case 1:
        m_rotation = IM_HAL_TRANSFORM_ROT_270;
        break;
    case 2:
        g_exchangeWidthHeight = 0;
        m_rotation = 0;
        break;
    case 4:
        m_rotation = IM_HAL_TRANSFORM_ROT_90;
        break;
    case 8:
//  g_exchangeWidthHeight = 0;
        m_rotation = IM_HAL_TRANSFORM_ROT_180;
        break;
    default:
        break;
    }
    // qDebug() << "------------------- "  << rotation << "------------------";
}

void MajorImageProcessingThread::run()
{
    int framedely = 0;
    time_t lastGetTime = time(NULL);
    int64_t timespausestamp = 0;
    uint yuvsize = 0;
    start_rkisp_capture();
    while (m_stopped == 0) {
        m_result = -1;
        const struct rkisp_api_buf *buf;
        buf = get_rkisp_frame(1);
        if (buf == nullptr || buf->size == 0) {
            if (time(NULL) - lastGetTime > 3) { //3 sec
                emit changCurrent();
            }
            usleep(10);
            continue;
        }
        lastGetTime = time(NULL);
        m_frame->yuv_frame = (uint8_t *)buf->buf;
        m_frame->width = get_rkisp_ctx_width();
        m_frame->height = get_rkisp_ctx_height();

        m_isChanging = false;

        if (m_nVdWidth != static_cast<unsigned int>(m_frame->width)
                || m_nVdHeight != static_cast<unsigned int>(m_frame->height)) {
            m_nVdWidth = static_cast<unsigned int>(m_frame->width);
            m_nVdHeight = static_cast<unsigned int>(m_frame->height);
            if (m_yuvPtr != nullptr) {
                delete [] m_yuvPtr;
                m_yuvPtr = nullptr;
            }

            yuvsize = m_nVdWidth * m_nVdHeight * 3 / 2;
            m_yuvPtr = new uchar[yuvsize];
            emit sigRenderYuv(true);
        } else {
            yuvsize = m_nVdWidth * m_nVdHeight * 3 / 2;
            if (!m_yuvPtr) {
                m_yuvPtr = new uchar[yuvsize];
                emit sigRenderYuv(true);
            }
        }

        //旋转
        uint8_t *preframe = m_frame->yuv_frame;
        if (0 != m_rotation) {
            rga_buffer_t    src;
            rga_buffer_t    dst;
            RgaSURF_FORMAT format = RK_FORMAT_YCbCr_420_P;
            int w = m_frame->width;
            int h = m_frame->height;
            src = wrapbuffer_virtualaddr(m_frame->yuv_frame, w, h, format);
            //dst = wrapbuffer_virtualaddr(m_frame->yuv_frame, h, w, format);
            dst = wrapbuffer_virtualaddr(m_yuvPtr, h, w, format);
            //IM_USAGE age = IM_HAL_TRANSFORM_ROT_270;
            if (IM_STATUS_SUCCESS == imrotate(src, dst, m_rotation)) {
                m_frame->width = src.height;
                m_frame->height = src.width;
                m_frame->yuv_frame = (uint8_t *)m_yuvPtr;
            }
        } else {
            memcpy(m_yuvPtr, m_frame->yuv_frame, m_frame->width * m_frame->height * 3 / 2);
        }
        //end 旋转

#ifdef __mips__
        uint8_t *rgb = static_cast<uint8_t *>(calloc(m_frame->width * m_frame->height * 3, sizeof(uint8_t)));
        yu12_to_rgb24(rgb, m_frame->yuv_frame, m_frame->width, m_frame->height);
#endif

        /*录像*/
        if (video_capture_get_save_video()) {
            int size = (m_frame->width * m_frame->height * 3) / 2;
            uint8_t *input_frame = m_frame->yuv_frame;

            set_video_timestamptmp(static_cast<int64_t>(v4l2core_time_get_timestamp()));
            encoder_add_video_frame(input_frame, size, static_cast<int64_t>(v4l2core_time_get_timestamp()), 0);
        }

        /*拍照*/
        if (m_bTake) {
            uint8_t *bmp = (uint8_t *)calloc(m_frame->width * m_frame->height * 3, sizeof(uint8_t));
            yu12_to_rgb24(bmp, m_frame->yuv_frame, m_frame->width, m_frame->height);
            QImage curImg(bmp, m_frame->width, m_frame->height, QImage::Format_RGB888);
            curImg.save(m_strPath, "jpg");
            free(bmp);
            emit takePicture(m_strPath);
            m_bTake = false;
        }

        m_result = 0;
        framedely = 0;
        m_rwMtxImg.lock();
        if (m_frame->yuv_frame != nullptr && (m_stopped == 0)) {
#ifdef __mips__
            QImage imgTmp(rgb, m_frame->width, m_frame->height, QImage::Format_RGB888);
            if (!imgTmp.isNull()) {
                m_Img = imgTmp.copy();
                emit SendMajorImageProcessing(&m_Img, m_result);
            }
#else
            emit sigYUVFrame(m_yuvPtr, m_frame->width, m_frame->height);
#endif
        }

#ifndef __mips__
        if (m_frame->yuv_frame == nullptr) {
            emit sigRenderYuv(false);
        }

#endif
        m_rwMtxImg.unlock();

#ifdef __mips__
        free(rgb);
#endif
        m_frame->yuv_frame = preframe;
        put_rkisp_frame(buf);
        msleep(50);
    }
    stop_rkisp_capture();
    close_rkisp_ctx();
}

void MajorImageProcessingThread::setChangeState()
{
    m_isChanging = true;
}

bool MajorImageProcessingThread::getChangeState()
{
    return m_isChanging;
}
