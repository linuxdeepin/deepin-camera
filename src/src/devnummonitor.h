/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
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
#ifndef DEVNUMMONITOR_H
#define DEVNUMMONITOR_H

#include <QThread>
#include<QTimer>
#include <QMutex>

#ifdef __cplusplus
extern "C" {
#endif
#include "LPF_V4L2.h"
#include "v4l2_core.h"
#include"v4l2_devices.h"
#include "stdlib.h"
#include "malloc.h"
#include "stdio.h"
#include "math.h"

#ifdef __cplusplus
}
#endif

class DevNumMonitor: public QThread
{
    Q_OBJECT
public:
    DevNumMonitor();
    void stop();
    void init();

signals:
    void seltBtnStateEnable();
    void seltBtnStateDisable();
    void noDeviceFound();
protected:
    void run();

private slots:
    void timeOutSlot();

public:
    //sstatic QMap<QString, QString> devmap;
private:
    QTimer *m_pTimer;
    v4l2_device_list_t *devlist;
    QMutex mutex;

};

#endif // DEVNUMMONITOR_H
