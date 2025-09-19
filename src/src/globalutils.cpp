// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "globalutils.h"

#include <DSysInfo>

#include <QDebug>
#include <QtDBus>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>

DCORE_USE_NAMESPACE

// 相机配置文件
const QString CAMERA_CONF_PATH = "/usr/share/deepin-camera/camera.conf";
#define LOW_PERFORMANCE_BOARD "LowPerformanceBoard"

QStringList GlobalUtils::m_LowPerformanceBoards = QStringList();

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
    // qDebug() << "Marshall the MyStructure data into a D-Bus argument";
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
    // qDebug() << "Retrieving MyStructure data from D-Bus argument";
    argument.beginStructure();
    argument >> mystruct.biosManufacturer >> mystruct.biosVersion >> mystruct.biosRelease >> mystruct.productName
            >> mystruct.serialNumber >> mystruct.Manufacturer >> mystruct.version >> mystruct.sysProductName
            >> mystruct.Family >> mystruct.sysSerialNumber >> mystruct.sysUUID;
    argument.endStructure();
    return argument;
}

bool GlobalUtils::isLowPerformanceBoard()
{
    // qDebug() << "Checking for low performance board";
    qDBusRegisterMetaType<DMIInfo>();

    QString systemInfoService = "com.deepin.system.SystemInfo";
    QString systemInfoPath = "/com/deepin/system/SystemInfo";
    QString systemInfoInterface = "com.deepin.system.SystemInfo";
    auto ver = DSysInfo::majorVersion().toInt();
    if (ver > 20) {
        qDebug() << "Condition check: System version > 20, using newer D-Bus interface";
        systemInfoService = "org.deepin.dde.SystemInfo1";
        systemInfoPath = "/org/deepin/dde/SystemInfo1";
        systemInfoInterface = "org.deepin.dde.SystemInfo1";
    }

    QDBusInterface *monitorInterface = new QDBusInterface(systemInfoService, systemInfoPath, "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (!monitorInterface->isValid()) {
        qWarning() << "Failed to connect to system info D-Bus interface";
        return false;
    }

    DMIInfo dmiInfo;
    QDBusReply<QDBusVariant> replay = monitorInterface->call("Get", systemInfoInterface, "DMIInfo");

    replay.value().variant().value<QDBusArgument>() >> dmiInfo;

    qDebug() << "System DMI info - Product Name:" << dmiInfo.productName;
    qDebug() << "System DMI info - Manufacturer:" << dmiInfo.Manufacturer;
    qDebug() << "System DMI info - Version:" << dmiInfo.version;

    // BXC 600F/NZ2750、CC GWMNDA1GL1等主板不能满足mp4封装的性能要求，因此需要特殊适配
    for (auto board : m_LowPerformanceBoards) {
        if (dmiInfo.productName.contains(board)) {
            qInfo() << "Detected low performance board:" << board;
            return true;
        }
    }

    qDebug() << "No low performance board detected";
    return false;
}

void GlobalUtils::loadCameraConf()
{
    qDebug() << "Loading camera configuration from:" << CAMERA_CONF_PATH;
    QSettings conf(CAMERA_CONF_PATH, QSettings::IniFormat);
    QVariant value = conf.value(LOW_PERFORMANCE_BOARD);
    if (value.isValid()) {
        qDebug() << "Condition check: Configuration value valid";
        m_LowPerformanceBoards = value.toStringList();
    } else {
        qWarning() << "No low performance boards configuration found in" << CAMERA_CONF_PATH;
        m_LowPerformanceBoards = QStringList();
    }
    qDebug() << "Function completed: loadCameraConf";
}
