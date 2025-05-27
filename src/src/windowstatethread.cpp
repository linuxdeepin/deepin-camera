// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
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
    qDebug() << "Initializing window state monitoring thread, Wayland:" << isWayland;
}

windowStateThread::~windowStateThread()
{
    qDebug() << "Cleaning up window state monitoring thread";
}

void windowStateThread::run()
{
    qDebug() << "Starting window state monitoring loop";
    while (!isInterruptionRequested()) {
        //获取当前工作区域内所有的窗口
        auto list = workspaceWindows();
        qDebug() << "Found" << list.size() << "windows in workspace";

        foreach(DForeignWindow *window, list) {
            if (window && window->windowState() == Qt::WindowState::WindowFullScreen) {
                qDebug() << "Detected fullscreen window:" << window->title();
                //去掉全屏的窗口信号处理
                //emit someWindowFullScreen();
            }
        }
        //线程休眠1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));
#ifdef UNITTEST
        break;
#endif
    }
    qInfo() << "Window state monitoring thread ended";
}

QList<DForeignWindow *> windowStateThread::workspaceWindows() const
{
    qDebug() << "Getting workspace windows list";
    QList<DForeignWindow *> windowList;
    QList<WId> currentApplicationWindowList;
    const QWindowList &list = qApp->allWindows();

    currentApplicationWindowList.reserve(list.size());
    qDebug() << "Total application windows:" << list.size();

    for (auto window : list) {
        if (window->property("_q_foreignWinId").isValid()) {
            qDebug() << "Skipping foreign window";
            continue;
        }

        currentApplicationWindowList.append(window->winId());
    }

    QVector<quint32> wmClientList = DWindowManagerHelper::instance()->currentWorkspaceWindowIdList();
    qDebug() << "Window manager client list size:" << wmClientList.size();

    bool currentWindow = false;
    for (WId wid : wmClientList) {
        if (currentApplicationWindowList.contains(wid)){
            currentWindow = true;
            qDebug() << "Found current application window:" << wid;
            continue;
        }
        if (false == currentWindow){
            continue;
        }
        if (DForeignWindow *w = DForeignWindow::fromWinId(wid)) {
            windowList << w;
            qDebug() << "Added foreign window to list:" << w->title();
        }
    }
    qDebug() << "Returning" << windowList.size() << "workspace windows";
    return windowList;
}


