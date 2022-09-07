// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _TAKE_PHOTO_SETTING_TEST_H
#define _TAKE_PHOTO_SETTING_TEST_H

#include <gtest/gtest.h>


class CMainWindow;
class takePhotoSettingAreaWidget;

class TakePhotoSettingTest: public ::testing::Test
{
public:
    TakePhotoSettingTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow *m_mainwindow;//主窗口
    takePhotoSettingAreaWidget *m_takePhotoSet;//左侧设置窗口
};
#endif
