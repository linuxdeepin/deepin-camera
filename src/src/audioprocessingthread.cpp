// Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audioprocessingthread.h"

#define FRAME_SIZE 512

AudioProcessingThread::AudioProcessingThread()
{
    qDebug() << "Initializing AudioProcessingThread";
    m_auidoBuffer = nullptr;
    m_data = nullptr;
    m_datasize = 0;
    init();
}

void AudioProcessingThread::stop()
{
    qDebug() << "Stopping audio processing thread";
    m_stopped = 1;
}

void AudioProcessingThread::init()
{
    qDebug() << "Initializing audio processing thread state";
    m_stopped = 0;
}

void AudioProcessingThread::run()
{
    qDebug() << "Starting audio processing thread";
    uint datasize = 0;
    audio_context_t *audio_ctx = get_audio_context();
    if(!audio_ctx)
    {
        qWarning() << "No audio context available: skipping audio processing";
        return;
    }

    //初始化音频样本获取缓存大小
    audio_set_cap_buffer_size(audio_ctx,
        FRAME_SIZE * audio_get_channels(audio_ctx));
    qDebug() << "Audio capture buffer size set to:" << FRAME_SIZE * audio_get_channels(audio_ctx);
    
    audio_start(audio_ctx);
    qDebug() << "Audio capture started";

    datasize = audio_ctx->capture_buff_size * sizeof(sample_t);
    qDebug() << "Audio data size:" << datasize << "bytes";

    /*
     * alloc the buffer after audio_start
     * otherwise capture_buff_size may not
     * be correct
     * allocated data is big enough for float samples (32 bit)
     * although it may contain int16 samples (16 bit)
     */
    m_auidoBuffer = audio_get_buffer(audio_ctx);

    uint sample_type = GV_SAMPLE_TYPE_FLOAT;
    qDebug() << "Using sample type: FLOAT";

    while (m_stopped == 0) {
        int ret = audio_get_next_buffer(audio_ctx, m_auidoBuffer, sample_type, get_audio_fx_mask());
        if (ret > 0) {
            /*
             * no buffers to process
             * sleep a couple of milisec
             */
            struct timespec req = {
               .tv_sec = 0,
               .tv_nsec = 1000000};/*nanosec*/
            nanosleep(&req, NULL);
        }
        else if (ret == 0) {
            if (m_datasize != datasize) {
                qDebug() << "Resizing audio data buffer from" << m_datasize << "to" << datasize << "bytes";
                m_datasize = datasize;
                if (m_data != nullptr) {
                    delete [] m_data;
                    m_data = nullptr;
                }
                m_data = new uchar[datasize];
            }

            memcpy(m_data, m_auidoBuffer->data, datasize);

            m_rwMtxData.lock();
            emit sendAudioProcessing(m_data, datasize);
            m_rwMtxData.unlock();
        }
        else {
            qWarning() << "Error getting next audio buffer, return code:" << ret;
        }
    }

    qDebug() << "Stopping audio capture and cleaning up resources";
    audio_stop(audio_ctx);
    audio_delete_buffer(m_auidoBuffer);
    qInfo() << "Audio processing thread stopped";

    return;
}

AudioProcessingThread::~AudioProcessingThread()
{
    qDebug() << "Destroying AudioProcessingThread";
    if (m_data) {
        delete [] m_data;
        m_data = nullptr;
    }
    qDebug() << "AudioProcessingThread destroyed";
}
