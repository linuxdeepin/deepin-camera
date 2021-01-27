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

ApplicationAdaptor::ApplicationAdaptor(CMainWindow *mw)
    : QDBusAbstractAdaptor(mw), m_mw(mw)
{
}

ApplicationAdaptor::~ApplicationAdaptor()
{
    if (m_mw) {
        m_mw->deleteLater();
        m_mw = nullptr;
    }
}

void ApplicationAdaptor::Raise()
{
    qDebug() << "raise window from dbus";
    m_mw->showNormal();
    m_mw->raise();
    m_mw->activateWindow();
}
