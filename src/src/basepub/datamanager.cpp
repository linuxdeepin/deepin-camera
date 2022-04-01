/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
*             zhangcheng <zhangchenga@uniontech.com>
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
}

