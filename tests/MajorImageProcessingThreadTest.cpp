// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
#include "addr_pri.h"

ACCESS_PRIVATE_FUN(MajorImageProcessingThread, void(), run);

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
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
}

/**
 *  @brief 镜像
 */
TEST_F(MajorImagePThTest, Horizon)
{
    m_processThread->setHorizontalMirror(false);
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    m_processThread->setHorizontalMirror(true);
}

/**
 *  @brief 没有接收到视频
 */
TEST_F(MajorImagePThTest, EmptyFrame)
{
    m_processThread->setHorizontalMirror(false);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame_none));
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame));
    m_processThread->setHorizontalMirror(true);
}

/**
 *  @brief 分辨率修改
 */
TEST_F(MajorImagePThTest, ChangedFrame)
{
    m_processThread->setHorizontalMirror(false);
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame_changed));
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    Stub_Function::resetSub(::v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame));
    m_processThread->setHorizontalMirror(true);
}

/**
 *  @brief 拍照暂停
 */
TEST_F(MajorImagePThTest, CapturePause)
{
    m_processThread->setHorizontalMirror(false);
    Stub_Function::resetSub(::get_capture_pause, ADDR(Stub_Function, get_capture_pause_true));
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    Stub_Function::resetSub(::get_capture_pause, ADDR(Stub_Function, get_capture_pause_false));
    m_processThread->setHorizontalMirror(true);
}

/**
 *  @brief
 */
TEST_F(MajorImagePThTest, scheduler)
{
    Stub_Function::resetSub(::encoder_buff_scheduler, ADDR(Stub_Function, encoder_buff_scheduler_one));
    m_processThread->setHorizontalMirror(false);
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    m_processThread->setHorizontalMirror(true);
    Stub_Function::resetSub(::encoder_buff_scheduler, ADDR(Stub_Function, encoder_buff_scheduler));
}

/**
 *  @brief 264
 */
TEST_F(MajorImagePThTest, Format264)
{
    Stub_Function::resetSub(::v4l2core_get_requested_frame_format, ADDR(Stub_Function, v4l2core_get_requested_frame_format_264));
    m_processThread->setHorizontalMirror(false);
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    m_processThread->setHorizontalMirror(true);
    Stub_Function::resetSub(::v4l2core_get_requested_frame_format, ADDR(Stub_Function, v4l2core_get_requested_frame_format_yuv));
}

/**
 *  @brief
 */
TEST_F(MajorImagePThTest, schedulerFormat264)
{
    Stub_Function::resetSub(::encoder_buff_scheduler, ADDR(Stub_Function, encoder_buff_scheduler_one));
    Stub_Function::resetSub(::v4l2core_get_requested_frame_format, ADDR(Stub_Function, v4l2core_get_requested_frame_format_264));
    m_processThread->setHorizontalMirror(false);
    m_processThread->init();
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    m_processThread->setHorizontalMirror(true);
    Stub_Function::resetSub(::encoder_buff_scheduler, ADDR(Stub_Function, encoder_buff_scheduler));
    Stub_Function::resetSub(::v4l2core_get_requested_frame_format, ADDR(Stub_Function, v4l2core_get_requested_frame_format_yuv));
}

/**
 *  @brief 拍照
 */
TEST_F(MajorImagePThTest, TakePic)
{
    m_processThread->init();
    m_processThread->m_bTake = true;
    m_processThread->m_strPath = "~/Pictures/1.jpg";
    m_processThread->setHorizontalMirror(false);
    call_private_fun::MajorImageProcessingThreadrun(*m_processThread);
    m_processThread->setHorizontalMirror(true);
}
