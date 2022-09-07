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

class windowStateThread : public QThread
{
    Q_OBJECT

public:
    explicit windowStateThread(QObject *parent = nullptr);
    QList<DForeignWindow *> workspaceWindows() const;
    void run();

signals:
    void someWindowFullScreen();

private:

};

#endif  // WINDOWSTATETHREAD_H
