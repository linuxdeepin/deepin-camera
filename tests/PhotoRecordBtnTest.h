// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _PHOTO_RECORD_BTN_TEST_H
#define _PHOTO_RECORD_BTN_TEST_H
#include <gtest/gtest.h>

class CMainWindow;
class photoRecordBtn;
class PhotoRecordBtnTest: public ::testing::Test
{
public:
    PhotoRecordBtnTest();
    ~PhotoRecordBtnTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow* m_mainwindow;
    photoRecordBtn* m_photoRecordBtn;


};

#endif
