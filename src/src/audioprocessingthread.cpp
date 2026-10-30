// Copyright (C) 2021 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "audioprocessingthread.h"
#include "ddlog.h"
using namespace DDLog;

#define FRAME_SIZE 512

AudioProcessingThread::AudioProcessingThread()
{
    qCDebug(appLog) << "Initializing AudioProcessingThread";
    m_auidoBuffer = nullptr;
    m_data = nullptr;
    m_datasize = 0;
    init();
}

void AudioProcessingThread::stop()
{
    qCDebug(appLog) << "Stopping audio processing thread";
    m_stopped = 1;
}

void AudioProcessingThread::init()
{
    qCDebug(appLog) << "Initializing audio processing thread state";
    m_stopped = 0;
}

void AudioProcessingThread::run()
{
    qCDebug(appLog) << "Starting audio processing thread";
    uint datasize = 0;
    audio_context_t *audio_ctx = get_audio_context();
    if(!audio_ctx)
    {
        qCWarning(appLog) << "No audio context available: skipping audio processing";
        return;
    }

    //初始化音频样本获取缓存大小
    audio_set_cap_buffer_size(audio_ctx,
        FRAME_SIZE * audio_get_channels(audio_ctx));
    qCDebug(appLog) << "Audio capture buffer size set to:" << FRAME_SIZE * audio_get_channels(audio_ctx);
    
    audio_start(audio_ctx);
    qCDebug(appLog) << "Audio capture started";

    datasize = audio_ctx->capture_buff_size * sizeof(sample_t);
    qCDebug(appLog) << "Audio data size:" << datasize << "bytes";

    /*
     * alloc the buffer after audio_start
     * otherwise capture_buff_size may not
     * be correct
     * allocated data is big enough for float samples (32 bit)
     * although it may contain int16 samples (16 bit)
     */
    m_auidoBuffer = audio_get_buffer(audio_ctx);

    uint sample_type = GV_SAMPLE_TYPE_FLOAT;
    qCDebug(appLog) << "Using sample type: FLOAT";

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
                qCDebug(appLog) << "Resizing audio data buffer from" << m_datasize << "to" << datasize << "bytes";
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
            qCWarning(appLog) << "Error getting next audio buffer, return code:" << ret;
        }
    }

    qCDebug(appLog) << "Stopping audio capture and cleaning up resources";
    audio_stop(audio_ctx);
    audio_delete_buffer(m_auidoBuffer);
    qCInfo(appLog) << "Audio processing thread stopped";

    return;
}

AudioProcessingThread::~AudioProcessingThread()
{
    qCDebug(appLog) << "Destroying AudioProcessingThread";
    if (m_data) {
        delete [] m_data;
        m_data = nullptr;
    }
    qCDebug(appLog) << "AudioProcessingThread destroyed";
}
