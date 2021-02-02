/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
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
class MajorImageProcessingThread : public QThread
{
    Q_OBJECT
public:
    MajorImageProcessingThread();
    ~MajorImageProcessingThread();

    void stop();
    void init();

public:
    QMutex m_rwMtxImg;
    QString m_strPath;
    QMutex m_rwMtxPath;
    bool m_bTake; //是否拍照

    QAtomicInt getStatus()
    {
        return stopped;
    }
protected:
    void run();

private:
    volatile int majorindex;
    QAtomicInt stopped;
    v4l2_dev_t *vd1;
    v4l2_frame_buff_t *frame;
    int result;
    QImage m_Img;
signals:
    void SendMajorImageProcessing(QImage *image, int result);
    void reachMaxDelayedFrames();

};

#endif // MajorImageProcessingThread_H
