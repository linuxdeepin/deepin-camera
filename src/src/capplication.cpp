// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "capplication.h"

CApplication *CApplication::cameraCore = nullptr;

CApplication::CApplication(int &argc, char **argv)
    : QtSingleApplication(argc, argv)
{
    qDebug() << "Function started: CApplication constructor";
    m_mainwindow = nullptr;
#ifndef TABLE_ENVIRONMENT
    m_bpanel = false;
#else
    m_bpanel = true;
#endif

    if (cameraCore == nullptr) {
        qDebug() << "Condition check: Setting cameraCore instance";
        cameraCore = this;
    }

    dApplication()->installEventFilter(this);
    dApplication()->setQuitOnLastWindowClosed(true);

    Dtk::Core::DVtableHook::overrideVfptrFun(dApplication(), &DApplication::handleQuitAction, CamApp, &CApplication::QuitAction);

    qDebug() << "Function completed: CApplication constructor";
}

CApplication *CApplication::CamApplication()
{
    // qDebug() << "Function started: CamApplication";
    return CApplication::cameraCore;
}

void CApplication::setMainWindow(CMainWindow *window)
{
    // qDebug() << "Function started: setMainWindow";
    if (!m_mainwindow)
        m_mainwindow = window;
}

bool CApplication::isPanelEnvironment()
{
    // qDebug() << "Function started: isPanelEnvironment";
    return m_bpanel;
}

CMainWindow *CApplication::getMainWindow()
{
    // qDebug() << "Function started: getMainWindow";
    return m_mainwindow;
}

void CApplication::QuitAction()
{
    // qDebug() << "Function started: QuitAction";
    emit CamApp->popupConfirmDialog();
}

