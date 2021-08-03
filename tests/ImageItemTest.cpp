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


extern QMap<int, ImageItem *> g_indexImage;

ACCESS_PRIVATE_FIELD(CMainWindow, ActType, m_nActTpye);
ACCESS_PRIVATE_FIELD(ImageItem, QAction*, m_actPrint);

ImageItemTest::ImageItemTest()
{
    mainwindow = CamApp->getMainWindow();
}

ImageItemTest::~ImageItemTest()
{
    mainwindow = NULL;
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
    ImageItem* pItem = mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);
    QAction *print = mainwindow->findChild<QAction *>("PrinterAction");
    if (print)
        print->trigger();
    QTest::qWait(1000);
}



/**
 *  @brief 右键打开文件夹
 */
TEST_F(ImageItemTest, OpenFolder)
{
    ImageItem* pItem = mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);
    QAction *actOpenFolder = mainwindow->findChild<QAction *>("OpenFolderAction");
    if (actOpenFolder) {
        actOpenFolder->trigger();
    }
}

/**
 *  @brief 单击缩略图
 */
TEST_F(ImageItemTest, mouseClickEvent)
{
    ImageItem* pItem = mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);

    if (pItem) {
        QTest::mouseClick(pItem, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
}

/**
 *  @brief 删除单张缩略图
 */
TEST_F(ImageItemTest, ImageItemDel)
{
    ImageItem* pItem = mainwindow->findChild<ImageItem*>(THUMBNAIL_PREVIEW);

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
    QAction *copyact = mainwindow->findChild<QAction *>("CopyAction");
    if (copyact)
        copyact->trigger();

    QAction *delact = mainwindow->findChild<QAction *>("DelAction");
    if (delact)
        delact->trigger();
}
