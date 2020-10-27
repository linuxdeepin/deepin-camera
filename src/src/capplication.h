/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*
* Maintainer: hujianbo <hujianbo@uniontech.com>
*
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
#ifndef CAPPLICATION_H
#define CAPPLICATION_H

#include <DApplication>
#include <DGuiApplicationHelper>
#include "mainwindow.h"

#include "qtsingleapplication/qtsingleapplication.h"

class CApplication;

#if  defined(dApp)
#undef dApp
#endif
#define dApp (static_cast<CApplication *>(QCoreApplication::instance()))

DWIDGET_USE_NAMESPACE

class CApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    CApplication(int &argc, char **argv);
    void setMainWindow(CMainWindow *window);
    CMainWindow *getMainWindow();
signals:
    void popupConfirmDialog();

protected:
    void handleQuitAction() override;
private:
    CMainWindow* m_mainwindow = nullptr;
};
#endif // CAPPLICATION_H
