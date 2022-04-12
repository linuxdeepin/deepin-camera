/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     houchengqiu <houchengqiu@uniontech.com>
* Maintainer: houchengqiu <houchengqiu@uniontech.com>
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

#pragma once

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/base/gstbaseparse.h>

#include <QString>

class GstVideoWriter
{

public:
    GstVideoWriter(const QString& videoPath = "test.webm");
    ~GstVideoWriter();

    // 开始录制
    void start();
    // 停止录制
    void stop();
    // 设置视频保存路径
    void setVideoPath(const QString& videoPath);
    // 设置视频成像质量
    void setQuantizer(uint quantizer);
    // 设置编码线程数
    void setEncodeThreadNum(uint num);
    // 写入视频帧数据，格式为rgb
    bool writeFrame(uchar *rgb, uint size);
    // 写入音频帧数据，格式为f32le
    bool writeAudio(uchar *audio, uint size);
    // 获取当前录制时长
    float getRecrodTime();

    // 获取管道对象
    inline GstElement* getPipeLine() { return m_pipeline; };
    // 获取GStreamer主循环对象
    inline GMainLoop* getGloop() { return m_gloop; };

protected:
    void init();
    void loadAppSrcCaps();

private:
    QString                     m_videoPath; //视频保存路径
    uint                        m_nWidth; // 录制视频分辨率宽度
    uint                        m_nHeight; // 录制视频分辨率高度
    uint                        m_nFrameRate; // 帧率
    uint                        m_nQuantizer; // 成像质量 范围4-63, 默认为44 值约小，成像质量越高，视频画面越清晰，编码耗时越长，视频文件占用磁盘控件越大
    uint                        m_nEncodeThreadNum; //编码多线程数量 默认为 6
    uint                        m_nSkipFrames; // 跳帧处理,默认不抽帧
    uint                        m_nFrameNum; // 帧编号

    GstElement                 *m_pipeline; // 管道实例
    GMainLoop                  *m_gloop;
    GstElement                 *m_appsrc;
    GstElement                 *m_audsrc;
    GstElement                 *m_vp8enc;
    GstElement                 *m_filesink;
    GstBus                     *m_bus;

};
