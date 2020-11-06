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

#include <DVtableHook>
#include <QObject>
#include <DGuiApplicationHelper>
#include "mainwindow.h"
#include <qtsingleapplication/qtsingleapplication.h>

class CApplication;

#if  defined(dApp)
#undef dApp
#endif
#define cApp (static_cast<CApplication *>(QCoreApplication::instance()))
#define CamApp (CApplication::CamApplication())

DWIDGET_USE_NAMESPACE

bool get_application(int &argc, char **argv);

class CApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    CApplication(int &argc, char **argv);

    /**
     * @brief Application 返回顶层topToolbar
     * @return 返回程序的指针
     */
    static CApplication *CamApplication();
    void setMainWindow(CMainWindow *window);
    CMainWindow *getMainWindow();
    void QuitAction();
signals:
    void popupConfirmDialog();

private:
    static bool createAppInstance(int &argc, char **argv);


protected:

private:
    CMainWindow* m_mainwindow = nullptr;
    static CApplication *m_cameracore;
};
#endif // CAPPLICATION_H
