// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbus_adpator.h"

ApplicationAdaptor::ApplicationAdaptor(CMainWindow *mw)
    : QDBusAbstractAdaptor(mw), m_mw(mw)
{
    // qDebug() << "Function started: ApplicationAdaptor constructor";
}

void ApplicationAdaptor::Raise()
{
    // qDebug() << "raise window from dbus";
    m_mw->showNormal();
    m_mw->raise();
    m_mw->activateWindow();
}

ApplicationAdaptor::~ApplicationAdaptor()
{
    // qDebug() << "Function started: ApplicationAdaptor destructor";
    if (m_mw) {
        // qDebug() << "Deleting main window";
        m_mw->deleteLater();
        m_mw = nullptr;
    }
}
