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

DataManager *DataManager::m_dataManager = nullptr;
MultiType DataManager::getMultiType()
{
    MultiType multiTypeStatus = MultiType::None;
    //几种状态：都没选，都选，选其中一个，选其中另一个
    if (!m_bCtrlMulti && !m_bShitfMulti)
        multiTypeStatus =  MultiType::None;
    else if (m_bCtrlMulti && m_bShitfMulti)
        multiTypeStatus = MultiType::Both;
    else {
        if (m_bCtrlMulti)
            multiTypeStatus = MultiType::Ctrl;

        if (m_bShitfMulti)
            multiTypeStatus = MultiType::Shift;
    }
    return multiTypeStatus;
}

void DataManager::setCtrlMulti(bool bCtrlMulti)
{
    m_bCtrlMulti = bCtrlMulti;
}

void DataManager::setShiftMulti(bool bShitfMulti)
{
    m_bShitfMulti = bShitfMulti;
}

int DataManager::getindexNow()
{
    return m_indexNow;
}

void DataManager::setindexNow(int indexNow)
{
    m_indexNow = indexNow;
}

QString &DataManager::getstrFileName()
{
    return m_strFileName;
}

void DataManager::setstrFileName(QString strFileName)
{
    m_strFileName = strFileName;
}

int &DataManager::getvideoCount()
{
    return m_videoCount;
}

void DataManager::setvideoCount(int videoCount)
{
    m_videoCount = videoCount;
}

enum DeviceStatus DataManager::getdevStatus()
{
    return m_devStatus;
}

void DataManager::setdevStatus(enum DeviceStatus devStatus)
{
    m_devStatus = devStatus;
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

    m_bCtrlMulti = false; //ctrl键多选
    m_bShitfMulti = false; //shift键连续多选
    m_indexLast = -1;
    m_indexNow = 0;
//    QMap<int, ImageItem *> m_indexImage;
    m_strFileName.clear();
    m_videoCount = 0;
    m_devStatus = DeviceStatus::NOCAM;
}

