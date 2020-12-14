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

#include <QtDBus>

#include "mainwindow.h"

class ApplicationAdaptor: public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.deepin.camera")
public:
    explicit ApplicationAdaptor(CMainWindow* mw);
    static QVariant redDBusProperty(const QString &service, const QString &path, const QString &interface = QString(), const char *propert = "");
    static QVariant redDBusMethod(const QString &service, const QString &path, const QString &interface, const char *method);

public slots:
    void Raise();

private:
    CMainWindow *_mw {nullptr};
    QTime oldTime;
};


#endif /* ifndef DBUS_ADAPTOR */

