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

#ifndef MajorImageProcessingThread_H
#define MajorImageProcessingThread_H

#include <QThread>
#include <QPixmap>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>

#ifdef __cplusplus
extern "C" {
#endif
#include "core_io.h"
#include "LPF_V4L2.h"
#include "camview.h"
#include "stdlib.h"
#include "malloc.h"
#include "stdio.h"
#include "math.h"
#include "save_image.h"
#include "colorspaces.h"
#include "render.h"
#ifdef __cplusplus
}
#endif

/**
 * @brief stop 线程处理图片
 */
class MajorImageProcessingThread : public QThread
{
    Q_OBJECT
public:
    MajorImageProcessingThread();

    ~MajorImageProcessingThread();

    /**
     * @brief setHorizontalMirror 设置水平镜像
     * @param bMirror 是否镜像
     */
    void setHorizontalMirror(bool bMirror) {m_bHorizontalMirror = bMirror;}

    /**
     * @brief stop 停止线程
     */
    void stop();

    /**
     * @brief init 线程初始化
     */
    void init();

    /**
     * @brief getStatus 获取状态
     */
    QAtomicInt getStatus()
    {
        return m_stopped;
    }

    /**
     * @brief setFilter 设置滤镜
     * @param filter 滤镜名称
     */
    void setFilter(QString filter);

    /**
     * @brief setExposure 设置曝光
     * @param exposure 曝光值
     */
    void setExposure(int exposure);

    /**
     * @brief setState 设置拍照，录像状态
     * @param bPhoto  true 拍照状态， false 录像状态
     */
    void setState(bool bPhoto){
        m_bPhoto = bPhoto;
    }

protected:
    /**
     * @brief run 运行线程
     */
    void run();


signals:
    /**
     * @brief SendMajorImageProcessing 向预览界面发送帧数据  mips平台、wayland下使用该接口
     * @param image 图像
     * @param result 结果
     */
    void SendMajorImageProcessing(QImage *image, int result);

    /**
     * @brief SendFilterImageProcessing 向滤镜预览类发送帧数据
     * @param image 预览图像 大小40*40
     */
    void SendFilterImageProcessing(QImage *image);

#ifndef __mips__
    /**
     * @brief sigYUVFrame YUV框架信号
     * @param yuv YUV
     * @param width 宽度
     * @param height 高度
     */
    void sigYUVFrame(uchar *yuv, uint width, uint height);

    /**
     * @brief sigRenderYuv 发送Yuv信号
     * @param width
     */
    void sigRenderYuv(bool);
#endif

    /**
     * @brief reachMaxDelayedFrames 到达最大延迟信号
     */
    void reachMaxDelayedFrames();

public:
    QMutex  m_rwMtxImg;
    QString m_strPath;
    QMutex  m_rwMtxPath;
    bool    m_bTake; //是否拍照

private:
    void ImageHorizontalMirror(const uint8_t* src, uint8_t* dst, int width, int height);

private:
    int               m_result;
    uint              m_nVdWidth;
    uint              m_nVdHeight;
    volatile int      m_majorindex;
    QString           m_filter;//当前选择的滤镜名称
    QAtomicInt        m_stopped;
    v4l2_dev_t        *m_videoDevice;
    v4l2_frame_buff_t *m_frame;
    uint8_t           *m_yuvPtr;

    bool              m_bPhoto = true; //相机当前状态，默认为拍照状态
    bool              m_bHorizontalMirror;   //水平镜像
    int               m_exposure = 0;

    QImage             m_Img;   //mips、wayland下使用该变量
    QImage             m_filterImg; //滤镜预览类使用 大小40*40

};

#endif // MajorImageProcessingThread_H
