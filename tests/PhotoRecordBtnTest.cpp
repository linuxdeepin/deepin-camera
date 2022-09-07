// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PhotoRecordBtnTest.h"
#include "src/photorecordbtn.h"
#include "src/videowidget.h"
#include "stub.h"
#include "stub_function.h"
#include "src/capplication.h"
#include "ac-deepin-camera-define.h"
#include "stub/addr_pri.h"
#include "datamanager.h"
#include <QtTest/qtest.h>


PhotoRecordBtnTest::PhotoRecordBtnTest()
{
    m_mainwindow = CamApp->getMainWindow();
    if (m_mainwindow){
        m_photoRecordBtn = m_mainwindow->findChild<photoRecordBtn *>(BUTTON_PICTURE_VIDEO);
    }
}

PhotoRecordBtnTest::~PhotoRecordBtnTest()
{
    m_mainwindow = NULL;
    m_photoRecordBtn = NULL;
}

void PhotoRecordBtnTest::SetUp()
{
   
}

void PhotoRecordBtnTest::TearDown()
{

}

TEST_F(PhotoRecordBtnTest, Photo)
{
    bool state = m_photoRecordBtn->photoState();
    m_photoRecordBtn->setState(!state);
    m_photoRecordBtn->setState(state);
}

TEST_F(PhotoRecordBtnTest, PreRecord)
{
    bool state = m_photoRecordBtn->photoState();
    m_photoRecordBtn->setState(!state);
    m_photoRecordBtn->setRecordState(photoRecordBtn::preRecord);

    m_photoRecordBtn->setState(state);
    m_photoRecordBtn->setRecordState(photoRecordBtn::Normal);
}

TEST_F(PhotoRecordBtnTest, Recording)
{
    bool state = m_photoRecordBtn->photoState();
    m_photoRecordBtn->setState(!state);
    m_photoRecordBtn->setRecordState(photoRecordBtn::Recording);

    m_photoRecordBtn->setState(state);
    m_photoRecordBtn->setRecordState(photoRecordBtn::Normal);
}

TEST_F(PhotoRecordBtnTest, Mouse)
{
    QTest::mouseMove(m_photoRecordBtn, QPoint(5, 5), 500);
    QTest::qWait(100);
    QTest::mousePress(m_photoRecordBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(100);
    QTest::mouseRelease(m_photoRecordBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(1000);
}
