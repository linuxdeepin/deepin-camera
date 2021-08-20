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

MajorImagePThTest::MajorImagePThTest()
{

}

MajorImagePThTest::~MajorImagePThTest()
{

}

void MajorImagePThTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
}

void MajorImagePThTest::TearDown()
{
    m_mainwindow = NULL;
}

/**
 *  @brief 最大延迟帧
 */
TEST_F(MajorImagePThTest, reachMaxDelayedFrames)
{
    MajorImageProcessingThread *processThread = m_mainwindow->findChild<MajorImageProcessingThread *>("MajorThread");
    processThread->getStatus();
    emit processThread->reachMaxDelayedFrames();
}
