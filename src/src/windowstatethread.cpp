/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     wuzhigang <wuzhigang@uniontech.com>
* Maintainer: wuzhigang <wuzhigang@uniontech.com>
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

#include "windowstatethread.h"
#include "qapplication.h"

#include <DWindowManagerHelper>
#include <QMutexLocker>

windowStateThread::windowStateThread(QObject *parent /*= nullptr*/)
    : QThread (parent)
{
    setObjectName("windowStateThread");
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

