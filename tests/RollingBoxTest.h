// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _ROLLING_BOX_TEST_H
#define _ROLLING_BOX_TEST_H
#include <gtest/gtest.h>

class CMainWindow;
class RollingBox;
class RollingBoxTest: public ::testing::Test
{
public:
    RollingBoxTest();
    ~RollingBoxTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow* m_mainwindow;
    RollingBox* m_rollingBox;


};

#endif
