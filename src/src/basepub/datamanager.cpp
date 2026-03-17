// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "datamanager.h"
#include "../capplication.h"

DataManager *DataManager::m_dataManager = nullptr;

QString &DataManager::getstrFileName()
{
    return m_strFileName;
}

enum DeviceStatus DataManager::getdevStatus()
{
    return m_devStatus;
}

void DataManager::setdevStatus(enum DeviceStatus devStatus)
{
    m_devStatus = devStatus;
}

void DataManager::setNowTabIndex(uint tabindex)
{
    m_tabIndexNow = tabindex;
}

uint DataManager::getNowTabIndex()
{
    return m_tabIndexNow;
}

void DataManager::setEncodeEnv(EncodeEnv env)
{
    m_encodeEnv = env;
}

EncodeEnv DataManager::encodeEnv()
{
#ifdef UNITTEST
    return FFmpeg_Env;
#endif
    return m_encodeEnv;
}

void DataManager::setEncExists(bool status)
{
    m_H264EncoderExists = status;
}

bool DataManager::encExists(){
    return m_H264EncoderExists;
}

void DataManager::setDeviceBlacklist(const QStringList &blacklist)
{
    // 无论是否为空，都先清空现有黑名单
    m_deviceBlacklistSet.clear();
    if (blacklist.isEmpty()) {
        qInfo() << "Empty blacklist provided";
        return;
    }

    // 每一条黑名单项的格式都是 vid,pid,name
    // 其中vid和pid都是4位十六进制数，name是设备名称
    // 后续匹配操作的时候，我们会忽略大小写
    for (const QString &item : blacklist) {
        QStringList parts = item.split(",");
        if (parts.size() != 3) {
            qWarning() << "Drop blacklist item(format error):" << item;
            continue;
        }

        // 验证VID和PID是否为4位十六进制数
        static const QRegularExpression hexPattern("^[0-9a-fA-F]{4}$");
        if (!hexPattern.match(parts[0]).hasMatch() || !hexPattern.match(parts[1]).hasMatch()) {
            qWarning() << "Drop blacklist item(invalid VID/PID):" << item;
            continue;
        }

        // 验证设备名称不为空，且长度不超过100个字符
        if (parts[2].trimmed().isEmpty() || parts[2].size() > 100) {
            qWarning() << "Drop blacklist item(empty device name OR too long):" << item;
            continue;
        }

        qInfo() << "Add blacklist item:" << item;
        m_deviceBlacklistSet.insert(item.toLower());
    }
};

bool DataManager::isDeviceValid(const QString &vid, const QString &pid, const QString &name)
{
    // 参数验证
    if (vid.isEmpty() || pid.isEmpty() || name.isEmpty()) {
        return true; // 空参数视为有效，避免误判
    }

    QString key = vid.toLower() + "," + pid.toLower() + "," + name.toLower();
    return !m_deviceBlacklistSet.contains(key);
}

DataManager *DataManager::instance()
{
    if (m_dataManager == nullptr) {
        m_dataManager = new DataManager;
    }
    return m_dataManager;
}

DataManager::DataManager()
{
    m_bTabEnter = false;
    m_tabIndex = 0;
    m_tabIndexNow = 0;
    m_strFileName.clear();
    m_videoCount = 0;
    m_encodeEnv = GStreamer_Env;
    m_devStatus = DeviceStatus::NOCAM;
    m_H264EncoderExists = false;
}

