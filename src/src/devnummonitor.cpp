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

#include "devnummonitor.h"

#include <QDebug>

extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}

DevNumMonitor::DevNumMonitor()
{
    
}

DevNumMonitor::~DevNumMonitor()
{
    if (m_pTimer != nullptr) {
        m_pTimer->stop();
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    }
}

void DevNumMonitor::run()
{
    m_pTimer = new QTimer;
    m_pTimer->setInterval(500);
    connect(m_pTimer, &QTimer::timeout, this, &DevNumMonitor::timeOutSlot);
    m_pTimer->start();
    this->exec();
    qDebug() << "Start monitoring the number of devices!";
}

void DevNumMonitor::timeOutSlot()
{
    check_device_list_events(get_v4l2_device_handler());

    if (get_device_list()->num_devices <= 1) {
        emit seltBtnStateDisable();
        if (get_device_list()->num_devices < 1) {
            //没有设备发送信号
            emit noDeviceFound();
            qDebug() << "There is no camera connected!";
        } else {
            emit existDevice();
            qDebug() << "There is a camera connected!";
        }
    } else {
        emit existDevice();
        //显示切换按钮
        emit seltBtnStateEnable();
        qDebug() << "There are two or more cameras connected!";
    }

}
