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

#include "ImageItemTest.h"
#include "src/mainwindow.h"
#include "src/capplication.h"
#include "ac-deepin-camera-define.h"
#include "stub.h"
#include "stub_function.h"
#include "datamanager.h"
#include "stub/addr_pri.h"

#include <QtTest/QTest>



ImageItemTest::ImageItemTest()
{
    m_mainwindow = CamApp->getMainWindow();
}

ImageItemTest::~ImageItemTest()
{
    m_mainwindow = NULL;
}

void ImageItemTest::SetUp()
{
    
}

void ImageItemTest::TearDown()
{
    
}


/**
 *  @brief 打印窗口
 */
TEST_F(ImageItemTest, printdialog)
{
    ImageItem* pItem = m_mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);
    QAction *print = m_mainwindow->findChild<QAction *>("PrinterAction");
    if (print)
        print->trigger();
    QTest::qWait(1000);
}



/**
 *  @brief 右键打开文件夹
 */
TEST_F(ImageItemTest, OpenFolder)
{
    ImageItem* pItem = m_mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);
    QAction *actOpenFolder = m_mainwindow->findChild<QAction *>("OpenFolderAction");
    if (actOpenFolder) {
        actOpenFolder->trigger();
    }
    //TODO  找到打开的文管窗口，然后关闭
}

/**
 *  @brief 单击缩略图
 */
TEST_F(ImageItemTest, mouseClickEvent)
{
    ImageItem* pItem = m_mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);

    if (pItem) {
        QTest::mouseClick(pItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    QTest::qWait(1000);
    //TODO 找到打开的看图或者影院窗口，关闭
}

/**
 *  @brief 删除单张缩略图
 */
TEST_F(ImageItemTest, ImageItemDel)
{
    ImageItem* pItem = m_mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);

    if (pItem) {
        //点击鼠标右键
        QTest::qWait(1000);
        QTest::mouseMove(pItem, QPoint(0, 0), 500);
        QTest::mousePress(pItem, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(pItem, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 0);

        //点击鼠标左键
        QTest::qWait(1000);
        QTest::mouseMove(pItem, QPoint(0, 0), 500);
        QTest::mousePress(pItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(pItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
}

/**
 *  @brief 缩略图复制，删除
 */
TEST_F(ImageItemTest, ImageItemCopyDel)
{
    QAction *copyact = m_mainwindow->findChild<QAction *>("CopyAction");
    if (copyact)
        copyact->trigger();

    QAction *delact = m_mainwindow->findChild<QAction *>("DelAction");
    if (delact)
        delact->trigger();
}
