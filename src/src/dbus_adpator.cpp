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

#include "dbus_adpator.h"

ApplicationAdaptor::ApplicationAdaptor(CMainWindow* mw)
    :QDBusAbstractAdaptor(mw), _mw(mw)
{
    oldTime = QTime::currentTime();
}


void ApplicationAdaptor::Raise(){
    qDebug()<<"raise window from dbus";
    _mw->showNormal();
    _mw->raise();
    _mw->activateWindow();
}

QVariant ApplicationAdaptor::redDBusProperty(const QString &service, const QString &path, const QString &interface, const char *propert)
{
    // 创建QDBusInterface接口
    QDBusInterface ainterface(service, path,
                              interface,
                              QDBusConnection::sessionBus());
    if (!ainterface.isValid()) {
        qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
        QVariant v(0) ;
        return  v;
    }
    //调用远程的value方法
    QList<QByteArray> q = ainterface.dynamicPropertyNames();
    QVariant v = ainterface.property(propert);
    return  v;
}
QVariant ApplicationAdaptor::redDBusMethod(const QString &service, const QString &path, const QString &interface, const char *method)
{
    // 创建QDBusInterface接口
    QDBusInterface ainterface(service, path,
                              interface,
                              QDBusConnection::sessionBus());
    if (!ainterface.isValid()) {
        qDebug() <<  "error:" << qPrintable(QDBusConnection::sessionBus().lastError().message());
        QVariant v(0) ;
        return  v;
    }
    //调用远程的value方法
    QDBusReply<QDBusVariant> reply = ainterface.call(method);
    if (reply.isValid()) {
//        return reply.value();
        QVariant v(0) ;
        return  v;
    } else {
        qDebug() << "error1:" << qPrintable(QDBusConnection::sessionBus().lastError().message());
        QVariant v(0) ;
        return  v;
    }
}


