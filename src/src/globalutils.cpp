// Copyright (C) 2020 ~ 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "globalutils.h"

#include <QDebug>
#include <QtDBus>
#include <QDBusInterface>
#include <QDBusArgument>
#include <QDBusReply>

#ifdef DTKCORE_CLASS_DConfigFile
#include <DConfig>
#include <memory>
#endif

// 相机配置文件
const QString CAMERA_CONF_PATH = "/usr/share/deepin-camera/camera.conf";
#define LOW_PERFORMANCE_BOARD "LowPerformanceBoard"

QStringList GlobalUtils::m_LowPerformanceBoards = QStringList();
bool GlobalUtils::m_IsLowPerformanceDevice = false;

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

bool GlobalUtils::isLowPerformanceBoard()
{
#ifdef DTKCORE_CLASS_DConfigFile
    if (m_IsLowPerformanceDevice) {
        return true;
    }
#endif

    qDBusRegisterMetaType<DMIInfo>();
    QDBusInterface *monitorInterface = new QDBusInterface("com.deepin.system.SystemInfo", "/com/deepin/system/SystemInfo", "org.freedesktop.DBus.Properties", QDBusConnection::systemBus());
    if (!monitorInterface->isValid())
        return false;

    DMIInfo dmiInfo;
    QDBusReply<QDBusVariant> replay = monitorInterface->call("Get", "com.deepin.system.SystemInfo", "DMIInfo");

    replay.value().variant().value<QDBusArgument>() >> dmiInfo;

    qDebug() << __func__ << "dmiInfo.productName: " << dmiInfo.productName;

    // BXC 600F/NZ2750、CC GWMNDA1GL1等主板不能满足mp4封装的性能要求，因此需要特殊适配
    for (auto board : m_LowPerformanceBoards) {
        if (dmiInfo.productName.contains(board))
            return true;
    }

    return false;
}

void GlobalUtils::loadCameraConf()
{
    QSettings conf(CAMERA_CONF_PATH, QSettings::IniFormat);
    QVariant value = conf.value(LOW_PERFORMANCE_BOARD);
    if (value.isValid()) {
        m_LowPerformanceBoards = value.toStringList();
    } else {
        m_LowPerformanceBoards = QStringList();
    }

#ifdef DTKCORE_CLASS_DConfigFile
    m_IsLowPerformanceDevice = false;
    std::unique_ptr<DTK_CORE_NAMESPACE::DConfig> dconfig(
        DTK_CORE_NAMESPACE::DConfig::create("org.deepin.camera", "org.deepin.camera.encode"));
    if (dconfig && dconfig->isValid() && dconfig->keyList().contains("isLowPerformanceDevice")) {
        m_IsLowPerformanceDevice = dconfig->value("isLowPerformanceDevice").toBool();
    }
#endif
}

QStringList GlobalUtils::parseStringListConfig(const QVariant &value)
{
    QStringList rawList;

    if (value.type() == QVariant::StringList) {
        rawList = value.toStringList();
    } else if (value.type() == QVariant::List) {
        const QVariantList variantList = value.toList();
        for (const QVariant &item : variantList) {
            if (item.type() == QVariant::String) {
                rawList.append(item.toString());
            }
        }
    } else {
        return QStringList();
    }

    QStringList result;
    for (const QString &keyword : rawList) {
        if (!keyword.trimmed().isEmpty()) {
            result.append(keyword);
        }
    }
    return result;
}
