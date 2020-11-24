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

CApplication::CApplication(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
{

}

void CApplication::setMainWindow(CMainWindow *window)
{
        if (nullptr != window) {
            m_mainwindow = window;
        }
}

CMainWindow *CApplication::getMainWindow()
{
    return m_mainwindow;
}

void CApplication::handleQuitAction()
{
    emit popupConfirmDialog();
}

