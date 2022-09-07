// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RollingBoxTest.h"
#include "src/rollingbox.h"
#include "src/photorecordbtn.h"
#include "src/videowidget.h"
#include "stub.h"
#include "stub_function.h"
#include "src/capplication.h"
#include "ac-deepin-camera-define.h"
#include "stub/addr_pri.h"
#include "datamanager.h"
#include <QtTest/qtest.h>

RollingBoxTest::RollingBoxTest()
{

}

RollingBoxTest::~RollingBoxTest()
{

}

void RollingBoxTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
    if (m_mainwindow){
        m_rollingBox = m_mainwindow->findChild<RollingBox *>(MODE_SWITCH_BOX);
    }
}

void RollingBoxTest::TearDown()
{
    m_mainwindow = nullptr;
    m_rollingBox = nullptr;
}

TEST_F(RollingBoxTest, setRange)
{
    int curVal = m_rollingBox->getCurrentValue();
    if (curVal == 0)
    {
        m_rollingBox->setRange(1,1);
    }
    m_rollingBox->setRange(0,0);

    m_rollingBox->setFocus();
}

TEST_F(RollingBoxTest, Event)
{
    QTest::mouseMove(m_rollingBox, QPoint(0, 0), 500);
    QTest::qWait(100);
    QTest::mousePress(m_rollingBox, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(100);
    QTest::mouseRelease(m_rollingBox, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(1000);
}

TEST_F(RollingBoxTest, KeyEvent)
{
    QTest::keyPress(m_rollingBox, Qt::Key_Down, Qt::NoModifier, 500);
    QTest::keyPress(m_rollingBox, Qt::Key_Up, Qt::NoModifier, 500);
}
