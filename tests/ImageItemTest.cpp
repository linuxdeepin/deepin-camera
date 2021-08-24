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
#include "src/imageitem.h"
#include <QtTest/QTest>



ImageItemTest::ImageItemTest()
{

}

ImageItemTest::~ImageItemTest()
{

}

void ImageItemTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
    if (m_mainwindow){
         m_imageItem = m_mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);
    }
}

void ImageItemTest::TearDown()
{
    m_mainwindow = NULL;
}


/**
 *  @brief 打印窗口
 */
TEST_F(ImageItemTest, printdialog)
{
    QAction *print = m_mainwindow->findChild<QAction *>("PrinterAction");
    if (print)
        print->trigger();
    QTest::qWait(1000);
}


/**
 *  @brief 获取焦点
 */
TEST_F(ImageItemTest, Focus)
{
    m_imageItem->setFocus();
    QTest::qWait(100);
}


/**
 *  @brief 打开右键菜单
 */
TEST_F(ImageItemTest, RightMenu)
{
    //emit m_imageItem->customContextMenuRequested(QPoint(0,0));
    //QTest::qWait(100);
    //QTest::mouseClick(m_mainwindow, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 100);
}

/**
 *  @brief 右键打开文件夹
 */
TEST_F(ImageItemTest, OpenFolder)
{
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
    if (m_imageItem) {
        QTest::mouseClick(m_imageItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    QTest::qWait(1000);
    //TODO 找到打开的看图或者影院窗口，关闭
}

/**
 *  @brief 删除单张缩略图
 */
TEST_F(ImageItemTest, ImageItemDel)
{
    if (m_imageItem) {
        //点击鼠标右键
        QTest::qWait(1000);
        QTest::mouseMove(m_imageItem, QPoint(0, 0), 500);
        QTest::mousePress(m_imageItem, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(m_imageItem, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 0);

        //点击鼠标左键
        QTest::qWait(1000);
        QTest::mouseMove(m_imageItem, QPoint(0, 0), 500);
        QTest::mousePress(m_imageItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(m_imageItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
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
