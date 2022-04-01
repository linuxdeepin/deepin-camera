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

#ifndef AudioProcessingThread_H
#define AudioProcessingThread_H

#include <QThread>
#include <QMutex>
#include <QDebug>

#ifdef __cplusplus
extern "C" {
#endif
#include "camview.h"
#include "audio.h"
#include "gviewrender.h"
#ifdef __cplusplus
}
#endif

/**
 * @brief 音频处理线程
 */
class AudioProcessingThread : public QThread
{
    Q_OBJECT
public:
    AudioProcessingThread();

    ~AudioProcessingThread();

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

protected:
    /**
     * @brief run 运行线程
     */
    void run();

signals:
    void sendAudioProcessing(uchar* data, uint size);

public:
    QMutex            m_rwMtxData;
private:
    QAtomicInt        m_stopped;
    audio_buff_t      *m_auidoBuffer;
    uchar             *m_data;

    uint              m_datasize;
};

#endif // AudioProcessingThread_H
