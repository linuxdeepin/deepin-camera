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

    void start();
    void stop();
    void setVideoPath(const QString& videoPath);
    void setQuantizer(uint quantizer);
    void setEncodeThreadNum(uint num);
    bool writeFrame(uchar *yu12, uint size);
    bool writeAudio(uchar *yu12, uint size);

    float getRecrodTime();

    inline GstElement* getPipeLine() { return m_pipeline; };
    inline GMainLoop* getGloop() { return m_gloop; };

protected:
    void init();
    void loadAppSrcCaps();

private:
    QString                     m_videoPath;
    uint                        m_nWidth;
    uint                        m_nHeight;
    uint                        m_nFrameRate;
    uint                        m_nQuantizer;//成像质量 范围4-63, 默认为35 值约小，成像质量越高，视频画面越清晰，编码耗时越长，视频文件占用磁盘控件越大
    uint                        m_nEncodeThreadNum;//编码多线程数量 默认为 4

    GstElement                 *m_pipeline;
    GMainLoop                  *m_gloop;
    GstElement                 *m_appsrc;
    GstElement                 *m_audsrc;
    GstElement                 *m_vp8enc;
    GstElement                 *m_filesink;
    GstBus                     *m_bus;

};
