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

#ifndef DBUS_ADAPTOR
#define DBUS_ADAPTOR

#include "mainwindow.h"

#include <QtDBus>

/**
* @brief CloseDialog　窗口大小适配
* 窗口最小化后双击桌面图标弹出相应大小的窗口
*/
class ApplicationAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.camera")
public:
    explicit ApplicationAdaptor(CMainWindow *mw);

    ~ApplicationAdaptor();

public slots:
    /**
    * @brief Raise 升起窗口信号
    */
    void Raise();

private:
    CMainWindow *m_mw;
    QTime       m_oldTime;
};


#endif /* ifndef DBUS_ADAPTOR */

