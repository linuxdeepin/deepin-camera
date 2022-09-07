// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _VIDEO_WIDGET_TEST_H
#define _VIDEO_WIDGET_TEST_H
#include <gtest/gtest.h>

class CMainWindow;
class videowidget;
class VideoWidgetTest: public ::testing::Test
{
public:
    VideoWidgetTest();
    ~VideoWidgetTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow* m_mainwindow;
    videowidget* m_videoWidget;

};

#endif