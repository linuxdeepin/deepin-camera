// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DevNumTest.h"
#include "src/mainwindow.h"
#include "src/capplication.h"
#include "stub/stub_function.h"
#include <QtTest/qtest.h>

DevNumberTest::DevNumberTest()
{

}

DevNumberTest::~DevNumberTest()
{

}

void DevNumberTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
    if (m_mainwindow) {
        m_devnumMonitor = m_mainwindow->findChild<DevNumMonitor *>("DevMonitorThread");
    }
}

void DevNumberTest::TearDown()
{
    m_mainwindow = NULL;
    m_devnumMonitor = NULL;
}


TEST_F(DevNumberTest, startCheck)
{
    QTest::qWait(1000);
    Stub_Function::resetSub(::get_device_list, ADDR(Stub_Function, get_device_list_0));
    QTest::qWait(1000);
    Stub_Function::resetSub(::get_device_list, ADDR(Stub_Function, get_device_list_1));
    QTest::qWait(1000);
    Stub_Function::resetSub(::get_device_list, ADDR(Stub_Function, get_device_list_2));
    QTest::qWait(1000);
    Stub_Function::resetSub(::get_device_list, ADDR(Stub_Function, get_device_list_3));
    QTest::qWait(1000);
}
