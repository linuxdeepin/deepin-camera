// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "windowstatethread.h"
#include "qapplication.h"

#include <DWindowManagerHelper>
#include <QMutexLocker>

windowStateThread::windowStateThread(bool isWayland, QObject *parent /*= nullptr*/)
    : QThread (parent)
{
    setObjectName("windowStateThread");
#ifdef KF5_WAYLAND_FLAGE_ON
    if (isWayland) {
        qInfo() << __FUNCTION__ << __LINE__  << "KF5_WAYLAND_FLAGE_ON is open!!";
        //wayland自动识别窗口
        m_connectionThread = new QThread(this);
        m_connectionThreadObject = new ConnectionThread();
        connect(m_connectionThreadObject, &ConnectionThread::connected, this,
        [this] {
            m_eventQueue = new EventQueue(this);
            m_eventQueue->setup(m_connectionThreadObject);

            Registry *registry = new Registry(this);
            qDebug() << "开始安装注册wayland服务...";
            setupRegistry(registry);
        },
        Qt::QueuedConnection
               );
        m_connectionThreadObject->moveToThread(m_connectionThread);
        m_connectionThread->start();
        m_connectionThreadObject->initConnection();
    }
#endif
}

void windowStateThread::run()
{
    while (!isInterruptionRequested()) {
//        QMutexLocker locker(&m_mutex);
        //获取当前工作区域内所有的窗口
        auto list = workspaceWindows();

        foreach(DForeignWindow *window, list) {
            if (window && window->windowState() == Qt::WindowState::WindowFullScreen) {
                emit someWindowFullScreen();
            }
        }
        //线程休眠1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));
#ifdef UNITTEST
        break;
#endif
    }
    qInfo() << "windowStateThread end";
}

QList<DForeignWindow *> windowStateThread::workspaceWindows() const
{
    QList<DForeignWindow *> windowList;
    QList<WId> currentApplicationWindowList;
    const QWindowList &list = qApp->allWindows();

    currentApplicationWindowList.reserve(list.size());

    for (auto window : list) {
        if (window->property("_q_foreignWinId").isValid()) {
            continue;
        }

        currentApplicationWindowList.append(window->winId());
    }

    QVector<quint32> wmClientList = DWindowManagerHelper::instance()->currentWorkspaceWindowIdList();

    bool currentWindow = false;
    for (WId wid : wmClientList) {
        if (currentApplicationWindowList.contains(wid)){
            currentWindow = true;
            continue;
        }
        if (false == currentWindow){
            continue;
        }
        if (DForeignWindow *w = DForeignWindow::fromWinId(wid)) {
            windowList << w;
        }
    }
    return windowList;
}

#ifdef KF5_WAYLAND_FLAGE_ON
void windowStateThread::setupRegistry(Registry *registry)
{
    connect(registry, &Registry::compositorAnnounced, this,
    [this, registry](quint32 name, quint32 version) {
        qDebug() << "开始创建wayland合成器...";
        m_compositor = registry->createCompositor(name, version, this);
        qDebug() << "wayland合成器已创建";
    }
           );

    connect(registry, &Registry::clientManagementAnnounced, this,
    [this, registry](quint32 name, quint32 version) {
        qDebug() << "开始创建wayland客户端管理...";
        m_clientManagement = registry->createClientManagement(name, version, this);
        qDebug() << "wayland客户端管理已创建";
//        qDebug() << QDateTime::currentDateTime().toString(QLatin1String("hh:mm:ss.zzz ")) << "createClientManagement";
        connect(m_clientManagement, &ClientManagement::windowStatesChanged, this,
        [this] {
            m_windowStates = m_clientManagement->getWindowStates();
            qDebug() << "当前窗口的数量1(ClientManagement::windowStatesChanged):" << m_windowStates.count();
            qDebug() << "Get new window states";
            this->waylandwindowinfo(m_windowStates);
        }
               );
    }
           );

    connect(registry, &Registry::interfacesAnnounced, this,
    [this] {
        Q_ASSERT(m_compositor);
        Q_ASSERT(m_clientManagement);
        qDebug() << "request getWindowStates";
        m_windowStates = m_clientManagement->getWindowStates();
        qDebug() << "当前窗口的数量2(Registry::interfacesAnnounced):" << m_windowStates.count();
        this->waylandwindowinfo(m_windowStates);
    }
           );

    qDebug() << "设置wayland注册的事件队列...";
    registry->setEventQueue(m_eventQueue);
    qDebug() << "设置wayland注册的链接线程...";
    registry->create(m_connectionThreadObject);
    qDebug() << "开始执行wayland注册...";
    registry->setup();
    qDebug() << "wayland注册完成";
}

void windowStateThread::waylandwindowinfo(const QVector<ClientManagement::WindowState> &windowStates)
{
    if (windowStates.count() == 0) {
        return;
    }

    bool aheandInCamera = false;
    QRect rect;
    for (int i = 0; i < windowStates.count(); ++i) {
        if (QString(windowStates[i].resourceName) != "deepin-camera") {
            if (!aheandInCamera)
                continue;
            else {
                if (windowStates[i].isFullScreen) {
                    QRect fullscreenRect(windowStates[i].geometry.x, windowStates[i].geometry.y,
                                         windowStates[i].geometry.width, windowStates[i].geometry.height);
                    if (fullscreenRect.contains(rect))
                        emit someWindowFullScreen();
                }
            }
        } else {
            aheandInCamera = true;
            rect = QRect(windowStates[i].geometry.x, windowStates[i].geometry.y,
                         windowStates[i].geometry.width, windowStates[i].geometry.height);
        }
    }
}
#endif

