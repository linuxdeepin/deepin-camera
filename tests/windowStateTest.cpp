// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "windowStateTest.h"
#include "src/capplication.h"
#include "src/mainwindow.h"
#include "src/windowstatethread.h"
#include "stub/stub_function.h"
#include <QtTest/qtest.h>
#include "addr_pri.h"
ACCESS_PRIVATE_FUN(windowStateThread, void(), run);

WindowStateTest::WindowStateTest()
{

}

WindowStateTest::~WindowStateTest()
{

}

void WindowStateTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
    if (m_mainwindow){
        m_pThread = m_mainwindow->findChild<windowStateThread *>("windowStateThread");
    }
}

void WindowStateTest::TearDown()
{
    m_mainwindow = NULL;
    m_pThread= nullptr;
}

TEST_F(WindowStateTest, Thread)
{
    call_private_fun::windowStateThreadrun(*m_pThread);
}
