/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     wuzhigang <wuzhigang@uniontech.com>
* Maintainer: wuzhigang <wuzhigang@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MajorImageProcessingThreadTest.h"
#include "src/majorimageprocessingthread.h"
#include "src/capplication.h"
#include "stub/stub_function.h"
#include <QtTest/qtest.h>
extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}


MajorImagePThTest::MajorImagePThTest()
{

}

MajorImagePThTest::~MajorImagePThTest()
{

}

void MajorImagePThTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
    if (m_mainwindow){
        m_processThread = m_mainwindow->findChild<MajorImageProcessingThread *>("MajorThread");
    }
}

void MajorImagePThTest::TearDown()
{
    m_mainwindow = NULL;
    m_processThread= nullptr;
}

/**
 *  @brief 最大延迟帧
 */
TEST_F(MajorImagePThTest, Thread)
{
    m_processThread->setHorizontalMirror(true);
    Stub_Function::clearSub(ADDR(MajorImageProcessingThread, start));
    m_processThread->start();
    QTest::qWait(1000);
    m_processThread->stop();
    m_processThread->wait();
    m_processThread->quit();
    EXPECT_EQ(m_processThread->getStatus(), 1);
    Stub_Function::resetSub(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
}

/**
 *  @brief 镜像
 */
TEST_F(MajorImagePThTest, Horizon)
{
    m_processThread->setHorizontalMirror(false);
    Stub_Function::clearSub(ADDR(MajorImageProcessingThread, start));
    m_processThread->start();
    QTest::qWait(1000);
    m_processThread->stop();
    m_processThread->wait();
    m_processThread->quit();
    EXPECT_EQ(m_processThread->getStatus(), 1);
    Stub_Function::resetSub(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
}

/**
 *  @brief 没有接收到视频
 */
TEST_F(MajorImagePThTest, EmptyFrame)
{
    m_processThread->setHorizontalMirror(false);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame_none));
    Stub_Function::clearSub(ADDR(MajorImageProcessingThread, start));
    m_processThread->start();
    QTest::qWait(1000);
    m_processThread->stop();
    m_processThread->wait();
    m_processThread->quit();
    EXPECT_EQ(m_processThread->getStatus(), 1);
    Stub_Function::resetSub(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame));
}

/**
 *  @brief 信号中断
 */
TEST_F(MajorImagePThTest, shutDownFrame)
{
    m_processThread->setHorizontalMirror(false);
    Stub_Function::clearSub(ADDR(MajorImageProcessingThread, start));
    m_processThread->start();
    QTest::qWait(1000);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame_none));
    QTest::qWait(1000);
    m_processThread->stop();
    m_processThread->wait();
    m_processThread->quit();
    EXPECT_EQ(m_processThread->getStatus(), 1);
    Stub_Function::resetSub(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame));
}

/**
 *  @brief 分辨率修改
 */
TEST_F(MajorImagePThTest, ChangedFrame)
{
    m_processThread->setHorizontalMirror(false);
    Stub_Function::clearSub(ADDR(MajorImageProcessingThread, start));
    m_processThread->start();
    QTest::qWait(1000);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame_changed));
    QTest::qWait(1000);
    m_processThread->stop();
    m_processThread->wait();
    m_processThread->quit();
    EXPECT_EQ(m_processThread->getStatus(), 1);
    Stub_Function::resetSub(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame));
}

/**
 *  @brief 拍照
 */
TEST_F(MajorImagePThTest, TakePic)
{
    m_processThread->m_bTake = true;
    m_processThread->m_strPath = "~/Pictures/Camera/1.webm";
    m_processThread->setHorizontalMirror(false);
    Stub_Function::clearSub(ADDR(MajorImageProcessingThread, start));
    m_processThread->start();
    QTest::qWait(1000);
    m_processThread->stop();
    m_processThread->wait();
    m_processThread->quit();
    EXPECT_EQ(m_processThread->getStatus(), 1);
    Stub_Function::resetSub(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
}
