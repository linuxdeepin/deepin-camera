// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "devnummonitor.h"

#include <QDebug>

extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}

DevNumMonitor::DevNumMonitor()
{
    qDebug() << "Initializing DevNumMonitor";
    m_noDevice = false;
    m_pTimer = nullptr;
}

/**
 * @brief 之前的代码timer 在子线程里面创建，timeOutSlot在主线程运行，修改为在主线程创建timer，无需开启一个挂起的线程。
 * 注意： timeOutSlot函数会修改摄像机管理链表内存、摄像机链表未加锁，并且发送信号的几个处理函数都将使用到摄像机链表，所以timeOutSlot必须在主线中使用
 */
void DevNumMonitor::startCheck()
{
    qDebug() << "Starting device number monitoring";
    m_pTimer = new QTimer;
    m_pTimer->setInterval(500);
    connect(m_pTimer, &QTimer::timeout, this, &DevNumMonitor::timeOutSlot);
    m_pTimer->start();
    qDebug() << "Device monitoring timer started with 500ms interval";
}

//void DevNumMonitor::run()
//{
//    m_pTimer = new QTimer;
//    m_pTimer->setInterval(500);
//    connect(m_pTimer, &QTimer::timeout, this, &DevNumMonitor::timeOutSlot);
//    QObject::connect(this, &QThread::destroyed, m_pTimer, &QTimer::deleteLater);
//    m_pTimer->start();
//    this->exec();
//    qDebug() << "Start monitoring the number of devices!";
//}

void DevNumMonitor::timeOutSlot()
{
    check_device_list_events(get_v4l2_device_handler());

    int deviceCount = get_device_list()->num_devices;

    if (deviceCount <= 1) {
        emit seltBtnStateDisable();
        
        if (deviceCount < 1) {
            if (!m_noDevice) {
                qWarning() << "No camera devices found";
                emit noDeviceFound();
                m_noDevice = true;
            }
        } else {
            if (m_noDevice) {
                qWarning() << "Device found";
            }
            m_noDevice = false;
            emit existDevice();
        }
    } else {
        m_noDevice = false;
        emit existDevice();
        //显示切换按钮
        emit seltBtnStateEnable();
    }
}

DevNumMonitor::~DevNumMonitor()
{
    qDebug() << "Destroying DevNumMonitor";
    if (nullptr != m_pTimer) {
        qDebug() << "Stopping and cleaning up monitoring timer";
        m_pTimer->stop();
        m_pTimer->deleteLater();
        m_pTimer = nullptr;
    }
}
