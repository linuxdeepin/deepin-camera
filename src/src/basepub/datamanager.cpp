// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "datamanager.h"
#include "../capplication.h"

DataManager *DataManager::m_dataManager = nullptr;

QString &DataManager::getstrFileName()
{
    // qDebug() << "Function started: getstrFileName";
    return m_strFileName;
}

enum DeviceStatus DataManager::getdevStatus()
{
    // qDebug() << "Function started: getdevStatus";
    return m_devStatus;
}

void DataManager::setdevStatus(enum DeviceStatus devStatus)
{
    // qDebug() << "Function started: setdevStatus";
    m_devStatus = devStatus;
}

void DataManager::setNowTabIndex(uint tabindex)
{
    // qDebug() << "Function started: setNowTabIndex";
    m_tabIndexNow = tabindex;
}

uint DataManager::getNowTabIndex()
{
    // qDebug() << "Function started: getNowTabIndex";
    return m_tabIndexNow;
}

void DataManager::setEncodeEnv(EncodeEnv env)
{
    // qDebug() << "Function started: setEncodeEnv";
    m_encodeEnv = env;
}

EncodeEnv DataManager::encodeEnv()
{
    // qDebug() << "Function started: encodeEnv";
#ifdef UNITTEST
    return FFmpeg_Env;
#endif
    return m_encodeEnv;
}

void DataManager::setEncExists(bool status)
{
    // qDebug() << "Function started: setEncExists";
    m_H264EncoderExists = status;
}

bool DataManager::encExists(){
    // qDebug() << "Function started: encExists";
    return m_H264EncoderExists;
}

DataManager *DataManager::instance()
{
    // qDebug() << "Function started: instance";
    if (m_dataManager == nullptr) {
        // qDebug() << "Condition check: Creating new DataManager instance";
        m_dataManager = new DataManager;
    }
    // qDebug() << "Function completed: instance";
    return m_dataManager;
}

DataManager::DataManager()
{
    // qDebug() << "Function started: DataManager constructor";
    m_bTabEnter = false;
    m_tabIndex = 0;
    m_tabIndexNow = 0;
    m_strFileName.clear();
    m_videoCount = 0;
    m_encodeEnv = GStreamer_Env;
    m_devStatus = DeviceStatus::NOCAM;
    m_H264EncoderExists = false;
    // qDebug() << "Function completed: DataManager constructor";
}

