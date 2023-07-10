// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WINDOWSTATETHREAD_H
#define WINDOWSTATETHREAD_H

#include <DMainWindow>
#include <DForeignWindow>

#include <thread>
#include <QThread>
#include <QMutex>

#ifdef KF5_WAYLAND_FLAGE_ON
#include <KF5/KWayland/Client/connection_thread.h>
#include <KF5/KWayland/Client/clientmanagement.h>
#include <KF5/KWayland/Client/event_queue.h>
#include <KF5/KWayland/Client/registry.h>
using namespace KWayland::Client;
#endif

class windowStateThread : public QThread
{
    Q_OBJECT

public:
    explicit windowStateThread(bool isWayland, QObject *parent = nullptr);
    QList<DForeignWindow *> workspaceWindows() const;
    void run();

signals:
    void someWindowFullScreen();

private:
#ifdef KF5_WAYLAND_FLAGE_ON
    /**
     * @brief wayland获取屏幕窗口信息的安装注册函数
     * @param registry
     */
    void setupRegistry(Registry *registry);

    /**
     * @brief wayland获取屏幕窗口信息
     * @param m_windowStates
     */
    void waylandwindowinfo(const QVector<ClientManagement::WindowState> &m_windowStates);
#endif

private:
#ifdef KF5_WAYLAND_FLAGE_ON
    // 获取wayland窗口信息相关。 wayland获取窗口的方法对于x11有很大的区别
    QThread *m_connectionThread;
    EventQueue *m_eventQueue = nullptr;
    ConnectionThread *m_connectionThreadObject;
    Compositor *m_compositor = nullptr;
    PlasmaWindowManagement *m_windowManagement = nullptr;
    ClientManagement *m_clientManagement = nullptr;
    QVector<ClientManagement::WindowState> m_windowStates;
    /**
      * @brief mips平台创建缓存文件的路径
      */
    std::string m_tempPath;
#endif

};

#endif  // WINDOWSTATETHREAD_H
