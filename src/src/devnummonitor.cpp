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

#include "devnummonitor.h"


extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}
//QMap<QString, QString> DevNumMonitor::devmap;

DevNumMonitor::DevNumMonitor()
{
    v4l2_device_list_t *devlist = get_device_list();
    for (int i = 0; i < devlist->num_devices; i++) {
        //qDebug() << devlist->list_devices[i].name << "," << devlist->list_devices[i].device << endl;
    }
}
void DevNumMonitor::stop()
{
    if (m_pTimer != nullptr) {
        m_pTimer->stop();
        m_pTimer->deleteLater();
    }
    m_pTimer = nullptr;
    v4l2core_close_v4l2_device_list();
    this->exit();
}

void DevNumMonitor::init()
{

}

void DevNumMonitor::run()
{
    QTimer *m_pTimer = new QTimer;
    m_pTimer->setInterval(200);
    connect(m_pTimer, &QTimer::timeout, this, &DevNumMonitor::timeOutSlot);
    m_pTimer->start();
    this->exec();
}

void DevNumMonitor::timeOutSlot()
{
    QMutexLocker locker(&mutex);

    check_device_list_events(get_v4l2_device_handler());
    devlist = get_device_list();
    if (devlist->num_devices == 0) {
        //没有设备发送信号
        emit noDeviceFound();
    }
    if (devlist->num_devices > 1) {
        //显示切换按钮
        emit seltBtnStateEnable();
    } else {
        //隐藏切换按钮
        emit seltBtnStateDisable();
    }
}
