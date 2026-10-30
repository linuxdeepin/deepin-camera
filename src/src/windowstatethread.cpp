// Copyright (C) 2020 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "windowstatethread.h"
#include "qapplication.h"

#include <DWindowManagerHelper>
#include <QMutexLocker>
#include "ddlog.h"
using namespace DDLog;

windowStateThread::windowStateThread(bool isWayland, QObject *parent /*= nullptr*/)
    : QThread (parent)
{
    setObjectName("windowStateThread");
    qCDebug(appLog) << "Initializing window state monitoring thread, Wayland:" << isWayland;
}

windowStateThread::~windowStateThread()
{
    qCDebug(appLog) << "Cleaning up window state monitoring thread";
}

void windowStateThread::run()
{
    qCDebug(appLog) << "Starting window state monitoring loop";
    while (!isInterruptionRequested()) {
#ifdef UNITTEST
        // 单元测试环境下跳过 workspaceWindows()，
        // 因为 DForeignWindow::fromWinId() 在工作线程中调用会违反 Qt 线程安全规则
        break;
#endif
        //获取当前工作区域内所有的窗口
        auto list = workspaceWindows();
        qCDebug(appLog) << "Found" << list.size() << "windows in workspace";

        foreach(DForeignWindow *window, list) {
            if (window && window->windowState() == Qt::WindowState::WindowFullScreen) {
                qCDebug(appLog) << "Detected fullscreen window:" << window->title();
                //去掉全屏的窗口信号处理
                //emit someWindowFullScreen();
            }
        }
        //线程休眠1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    qCInfo(appLog) << "Window state monitoring thread ended";
}

QList<DForeignWindow *> windowStateThread::workspaceWindows() const
{
    qCDebug(appLog) << "Getting workspace windows list";
    QList<DForeignWindow *> windowList;
    QList<WId> currentApplicationWindowList;
    const QWindowList &list = qApp->allWindows();

    currentApplicationWindowList.reserve(list.size());
    qCDebug(appLog) << "Total application windows:" << list.size();

    for (auto window : list) {
        if (window->property("_q_foreignWinId").isValid()) {
            qCDebug(appLog) << "Skipping foreign window";
            continue;
        }

        currentApplicationWindowList.append(window->winId());
    }

    QVector<quint32> wmClientList = DWindowManagerHelper::instance()->currentWorkspaceWindowIdList();
    qCDebug(appLog) << "Window manager client list size:" << wmClientList.size();

    bool currentWindow = false;
    for (WId wid : wmClientList) {
        if (currentApplicationWindowList.contains(wid)){
            currentWindow = true;
            qCDebug(appLog) << "Found current application window:" << wid;
            continue;
        }
        if (false == currentWindow){
            continue;
        }
        if (DForeignWindow *w = DForeignWindow::fromWinId(wid)) {
            windowList << w;
            qCDebug(appLog) << "Added foreign window to list:" << w->title();
        }
    }
    qCDebug(appLog) << "Returning" << windowList.size() << "workspace windows";
    return windowList;
}


