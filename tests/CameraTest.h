// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _DEV_NUM_TEST_H
#define _DEV_NUM_TEST_H
#include <gtest/gtest.h>

class CMainWindow;
class DevNumMonitor;
class MajorImageProcessingThread;
class CameraTest: public ::testing::Test
{
public:
    CameraTest();
    ~CameraTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow*                m_mainwindow;
    DevNumMonitor*  m_devnumMonitor;
    MajorImageProcessingThread* m_processThread;
};


#endif
