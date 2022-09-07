// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _DEV_NUM_TEST_H
#define _DEV_NUM_TEST_H
#include <gtest/gtest.h>

class DevNumMonitor;
class CMainWindow;
class DevNumberTest: public ::testing::Test
{
public:
    DevNumberTest();
    ~DevNumberTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow*    m_mainwindow;
    DevNumMonitor*  m_devnumMonitor;


};


#endif
