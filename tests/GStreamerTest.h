// Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _DEV_NUM_TEST_H
#define _DEV_NUM_TEST_H
#include <gtest/gtest.h>

class CMainWindow;
class MajorImageProcessingThread;
class GStreamerTest: public ::testing::Test
{
public:
    GStreamerTest();
    ~GStreamerTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow*                m_mainwindow;
    MajorImageProcessingThread* m_processThread;
};


#endif
