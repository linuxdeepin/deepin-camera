// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _IMAGE_ITEM_TEST_H
#define _IMAGE_ITEM_TEST_H

#include <gtest/gtest.h>
#include <QString>


class CMainWindow;
class ImageItem;
class ImageItemTest: public ::testing::Test
{
public:
    ImageItemTest();
    ~ImageItemTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

    QString getImageFileName();
    QString getVideoFileName();

protected:
    CMainWindow* m_mainwindow;
    ImageItem*  m_imageItem;

};




#endif
