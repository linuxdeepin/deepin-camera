// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QLibrary>
#include <QDir>
#include <QLibraryInfo>
#include <QJsonDocument>

#include "eventlogutils.h"

EventLogUtils *EventLogUtils::m_instance(nullptr);

EventLogUtils &EventLogUtils::get()
{
    // qDebug() << "Function started: get";
    if (m_instance == nullptr) {
        // qDebug() << "Condition check: Creating new EventLogUtils instance";
        m_instance = new EventLogUtils;
    }
    // qDebug() << "Function completed: get";
    return *m_instance;
}

EventLogUtils::EventLogUtils()
{
    // qDebug() << "Function started: EventLogUtils constructor";
    QLibrary library("libdeepin-event-log.so");

    init =reinterpret_cast<bool (*)(const std::string &, bool)>(library.resolve("Initialize"));
    writeEventLog = reinterpret_cast<void (*)(const std::string &)>(library.resolve("WriteEventLog"));

    if (init == nullptr)
        return;

    init("deepin-camera", true);
    // qDebug() << "Function completed: EventLogUtils constructor";
}

void EventLogUtils::writeLogs(QJsonObject &data)
{
    // qDebug() << "Function started: writeLogs";
    if (writeEventLog == nullptr)
        return;

    //std::string str = QJsonDocument(data).toJson(QJsonDocument::Compact).toStdString();
    writeEventLog(QJsonDocument(data).toJson(QJsonDocument::Compact).toStdString());
    // qDebug() << "Function completed: writeLogs";
}
