// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _WINDOWN_STATE_TEST_H
#define _WINDOWN_STATE_TEST_H


#include <gtest/gtest.h>

class CMainWindow;
class windowStateThread;
class  WindowStateTest: public ::testing::Test
{
public:
    WindowStateTest();
    ~WindowStateTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow* m_mainwindow;
    windowStateThread* m_pThread;

};




#endif
