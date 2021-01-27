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

#ifndef DEVNUMMONITOR_H
#define DEVNUMMONITOR_H

#include <QThread>
#include <QTimer>

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

/**
* @brief DevNumMonitor　设备数目监视器
*/
class DevNumMonitor: public QThread
{
    Q_OBJECT
public:
    explicit DevNumMonitor(QTimer *timer = nullptr);

    ~DevNumMonitor();

signals:
    /**
    * @brief seltBtnStateEnable 相机选择按钮可用信号
    */
    void seltBtnStateEnable();

    /**
    * @brief seltBtnStateDisable 相机选择按钮不可用信号
    */
    void seltBtnStateDisable();

    /**
    * @brief noDeviceFound 未发现相机设备信号
    */
    void noDeviceFound();

    /**
    * @brief existDevice 存在相机设备信号
    */
    void existDevice();

protected:
    /**
    * @brief run 运行
    */
    void run();

private slots:
    /**
    * @brief timeOutSlot 监视设备数目槽，根据相机数目执行相应的处理
    */
    void timeOutSlot();

private:
    QTimer             *m_pTimer;

};

#endif // DEVNUMMONITOR_H
