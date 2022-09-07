// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _MAJOR_IMAGE_PROCESSING_THREAD_TEST_H
#define _MAJOR_IMAGE_PROCESSING_THREAD_TEST_H

#include <gtest/gtest.h>

class CMainWindow;
class MajorImageProcessingThread;
class  MajorImagePThTest: public ::testing::Test
{
public:
    MajorImagePThTest();
    ~MajorImagePThTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow* m_mainwindow;
    MajorImageProcessingThread* m_processThread;

};

#endif
