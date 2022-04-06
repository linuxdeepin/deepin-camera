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
#include "gstvideowriter.h"
extern "C" {
#include <gst/app/gstappsrc.h>
#include "v4l2_core.h"
#include "camview.h"
#include "gviewaudio.h"
#include "audio.h"
}

#include <QtConcurrent>

GST_DEBUG_CATEGORY(appsrc_pipeline_debug);

static gboolean
bus_message(GstBus * bus, GstMessage  * message, GstVideoWriter * app)
{
    GST_DEBUG("got message %s",
        gst_message_type_get_name(GST_MESSAGE_TYPE(message)));

    switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR: {
        GError *err = NULL;
        gchar *dbg_info = NULL;

        gst_message_parse_error(message, &err, &dbg_info);

        gchar* elename = GST_OBJECT_NAME(message->src);

        g_printerr("ERROR from element %s: %s\n", elename, err->message);
        g_printerr("Debugging info: %s\n", (dbg_info) ? dbg_info : "none");

        g_error_free(err);
        g_free(dbg_info);

        // 结束录制任务
        g_main_loop_quit(app->getGloop());
        // 停止管道
        gst_element_set_state(app->getPipeLine(), GST_STATE_NULL);
        break;
    }
    case GST_MESSAGE_EOS: {
        // 结束录制任务
        g_main_loop_quit(app->getGloop());
        // 停止管道
        gst_element_set_state(app->getPipeLine(), GST_STATE_NULL);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

GstVideoWriter::GstVideoWriter(const QString& videoPath):
    m_videoPath(videoPath)
  , m_nWidth(1920)
  , m_nHeight(1080)
  , m_nFrameRate(30)
  , m_nQuantizer(44)
  , m_nEncodeThreadNum(6)
  , m_pipeline(nullptr)
  , m_gloop(nullptr)
  , m_appsrc(nullptr)
  , m_audsrc(nullptr)
  , m_vp8enc(nullptr)
  , m_filesink(nullptr)
  , m_bus(nullptr)
{
    init();
}

GstVideoWriter::~GstVideoWriter()
{
    gst_object_unref(m_pipeline);
    gst_object_unref(m_appsrc);
    gst_object_unref(m_audsrc);
    gst_object_unref(m_vp8enc);
    gst_object_unref(m_filesink);
    gst_object_unref(m_bus);
}

void GstVideoWriter::start()
{
    // 设置视频帧数据格式
    loadAppSrcCaps();

    // 启动管道
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);

    // 将录制任务放到线程中运行，bus回调收到eos信号，结束录制任务
    QtConcurrent::run(g_main_loop_run, m_gloop);
}

void GstVideoWriter::stop()
{
    GstFlowReturn ret;

    // 停止音频流
    if (m_audsrc)
        g_signal_emit_by_name(m_audsrc, "end-of-stream", &ret);

    // 停止视频流
    if (m_appsrc)
        g_signal_emit_by_name(m_appsrc, "end-of-stream", &ret);
}

void GstVideoWriter::setVideoPath(const QString &videoPath)
{
    if (!videoPath.isEmpty() && m_videoPath != videoPath)
        m_videoPath = videoPath;

    if (m_filesink)
        g_object_set(m_filesink, "location", videoPath.toStdString().c_str(), NULL);
}

void GstVideoWriter::setQuantizer(uint quantizer)
{
    m_nQuantizer = quantizer;

    if (m_vp8enc)
        g_object_set(m_vp8enc, "min-quantizer", m_nQuantizer, NULL);
}

void GstVideoWriter::setEncodeThreadNum(uint num)
{
    m_nEncodeThreadNum = num;

    if (m_vp8enc)
        g_object_set(m_vp8enc, "threads", m_nEncodeThreadNum, NULL);
}

bool GstVideoWriter::writeFrame(uchar *rgb, uint size)
{
    GstFlowReturn ret = GST_FLOW_CUSTOM_ERROR;
    guint8 *ptr = (guint8 *)g_malloc(size * sizeof(uchar));
    if (ptr)
    {
        memcpy(ptr, rgb, size);
        GstBuffer *buffer = gst_buffer_new_wrapped((void*)ptr, size);

        //设置时间戳
        if (m_pipeline)
            GST_BUFFER_PTS(buffer) = gst_clock_get_time(m_pipeline->clock) - m_pipeline->base_time;

        //注入视频帧数据
        if (m_appsrc)
            g_signal_emit_by_name(m_appsrc, "push-buffer", buffer, &ret);

        gst_buffer_unref(buffer);
    }

    return ret == GST_FLOW_OK;
}

bool GstVideoWriter::writeAudio(uchar *audio, uint size)
{
    GstFlowReturn ret = GST_FLOW_CUSTOM_ERROR;
    guint8 *ptr = (guint8 *)g_malloc(size * sizeof(uchar));
    if (ptr)
    {
        memcpy(ptr, audio, size);
        GstBuffer *buffer = gst_buffer_new_wrapped((void*)ptr, size);

        // 设置时间戳
        if (m_pipeline)
            GST_BUFFER_PTS(buffer) = gst_clock_get_time(m_pipeline->clock) - m_pipeline->base_time;

        // 注入音频帧数据
        if (m_audsrc) {
            g_signal_emit_by_name(m_audsrc, "push-buffer", buffer, &ret);
        }

        gst_buffer_unref(buffer);
    }

    return ret == GST_FLOW_OK;
}

float GstVideoWriter::getRecrodTime()
{
    float pts = 0;
    if (m_pipeline)
        pts = gst_clock_get_time(m_pipeline->clock) - m_pipeline->base_time;

    return pts / 1000 / 1000 / 1000;
}

void GstVideoWriter::init()
{
    // vp8编码在高分辨(800*600以上或高成像质量下存在巨大编码耗时，因此在编码时使用多线程提升编码速度
    QString pipDesc = QString("webmmux name=mux ! filesink name=filename "
                              "appsrc name=source ! queue ! videoconvert primaries-mode=2  name=convert ! queue ! "
                              "vp8enc name=encoder ! queue ! mux.video_0 "
                              "appsrc name=audiosource ! queue ! audioconvert ! audioresample ! vorbisenc ! queue ! mux.audio_0");
    m_pipeline = gst_parse_launch(pipDesc.toStdString().c_str(), NULL);
    g_assert(m_pipeline);

    m_gloop = g_main_loop_new(NULL, TRUE);

    // 设置视频保存路径
    m_filesink = gst_bin_get_by_name(GST_BIN(m_pipeline), "filename");
    setVideoPath(m_videoPath);

    // 设置vp8编码器线程数和成像质量
    m_vp8enc = gst_bin_get_by_name(GST_BIN(m_pipeline), "encoder");
    setEncodeThreadNum(6);
    setQuantizer(44);

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    if (m_bus)
        gst_bus_add_watch(m_bus, (GstBusFunc)bus_message, this);

    /* 设置视频src属性 */
    m_appsrc = gst_bin_get_by_name(GST_BIN(m_pipeline), "source");
    if (m_appsrc) {
        loadAppSrcCaps();
        g_object_set(m_appsrc, "format", GST_FORMAT_TIME, NULL);
        g_object_set(m_appsrc, "is-live", TRUE, NULL);
    }

    /* 设置音频src属性 */
    m_audsrc = gst_bin_get_by_name(GST_BIN(m_pipeline), "audiosource");
    if (m_audsrc) {
        GstCaps *audiocaps = gst_caps_new_simple("audio/x-raw",
            "format", G_TYPE_STRING, "F32LE",
            "layout", G_TYPE_STRING, "interleaved",
            "channels", G_TYPE_INT, 2,
            "rate", G_TYPE_INT, 44100,
            NULL);
        gst_app_src_set_caps(GST_APP_SRC(m_audsrc), audiocaps);
        g_object_set(m_audsrc, "format", GST_FORMAT_TIME, NULL);
        g_object_set(m_audsrc, "is-live", TRUE, NULL);
    }
}

void GstVideoWriter::loadAppSrcCaps()
{
    // 获取当前相机分辨率
    m_nWidth = v4l2core_get_frame_width(get_v4l2_device_handler());
    m_nHeight = v4l2core_get_frame_height(get_v4l2_device_handler());
    if (m_nWidth > 0 && m_nHeight > 0 && m_appsrc) {
        GstCaps *caps = gst_caps_new_simple("video/x-raw",
            "format", G_TYPE_STRING, "RGB",
            "width", G_TYPE_INT, m_nWidth,
            "height", G_TYPE_INT, m_nHeight,
            "framerate", GST_TYPE_FRACTION, m_nFrameRate, 1,
            NULL);
        gst_app_src_set_caps(GST_APP_SRC(m_appsrc), caps);
    }
}
