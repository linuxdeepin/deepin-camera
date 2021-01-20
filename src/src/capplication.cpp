/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
* Maintainer: hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
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

#include "capplication.h"

CApplication *CApplication::cameraCore = nullptr;

CApplication::CApplication(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
{
    m_mainwindow = nullptr;

    if (cameraCore == nullptr)
        cameraCore = this;

    dApplication()->installEventFilter(this);
    dApplication()->setQuitOnLastWindowClosed(true);

    Dtk::Core::DVtableHook::overrideVfptrFun(dApplication(), &DApplication::handleQuitAction, CamApp, &CApplication::QuitAction);

}

CApplication *CApplication::CamApplication()
{
    return CApplication::cameraCore;
}

void CApplication::setMainWindow(CMainWindow *window)
{
    if (!m_mainwindow)
        m_mainwindow = window;
}

void CApplication::setprocess(QList<QProcess *> &process)
{
    m_camprocesslist.clear();

    if (m_camprocesslist.isEmpty()) {
        for (QList<QProcess *>::iterator i = process.begin(); i != process.end(); i++)
            m_camprocesslist.append(*i);
    }
}

QList<QProcess *> CApplication::getprocess()
{
    return m_camprocesslist;
}

CMainWindow *CApplication::getMainWindow()
{
    return m_mainwindow;
}

void CApplication::QuitAction()
{
    emit CamApp->popupConfirmDialog();
}

