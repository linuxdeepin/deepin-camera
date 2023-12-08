// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "globalutils.h"

#include <QDebug>
#include <QtDBus>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>

struct DMIInfo {
    QString biosManufacturer;
    QString biosVersion;
    QString biosRelease;
    QString productName;
    QString serialNumber;
    QString Manufacturer;
    QString version;
    QString sysProductName;
    QString Family;
    QString sysSerialNumber;
    QString sysUUID;
};
Q_DECLARE_METATYPE(DMIInfo)

// Marshall the MyStructure data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const DMIInfo &mystruct)
{
    argument.beginStructure();
    argument << mystruct.biosManufacturer << mystruct.biosVersion << mystruct.biosRelease << mystruct.productName
             << mystruct.serialNumber << mystruct.Manufacturer << mystruct.version << mystruct.sysProductName
             << mystruct.Family << mystruct.sysSerialNumber << mystruct.sysUUID;
    argument.endStructure();
    return argument;
}

// Retrieve the MyStructure data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, DMIInfo &mystruct)
{
    argument.beginStructure();
    argument >> mystruct.biosManufacturer >> mystruct.biosVersion >> mystruct.biosRelease >> mystruct.productName
            >> mystruct.serialNumber >> mystruct.Manufacturer >> mystruct.version >> mystruct.sysProductName
            >> mystruct.Family >> mystruct.sysSerialNumber >> mystruct.sysUUID;
    argument.endStructure();
    return argument;
}

bool GlobalUtils::isBXCBoard()
{
    qDBusRegisterMetaType<DMIInfo>();
    QDBusInterface *monitorInterface = new QDBusInterface("com.deepin.system.SystemInfo", "/com/deepin/system/SystemInfo", "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (!monitorInterface->isValid())
        return false;

    DMIInfo dmiInfo;
    QDBusReply<QDBusVariant> replay = monitorInterface->call("Get", "com.deepin.system.SystemInfo", "DMIInfo");

    replay.value().variant().value<QDBusArgument>() >> dmiInfo;

    qDebug() << __func__ << "dmiInfo.productName: " << dmiInfo.productName;
    if (dmiInfo.productName.contains("600F") || dmiInfo.productName.contains("NZ275D")) {
        return true;
    } else {
        return false;
    }
}
