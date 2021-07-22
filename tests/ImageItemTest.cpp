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


///**
// *  @brief 打印窗口
// */
//TEST_F(ImageItemTest, printdialog)
//{
//    QMap<int, ImageItem *> it = get_imageitem();

//    if (it.count() > 0) {
//        ImageItem* pItem = it.value(0);
//        QAction *print = mainwindow->findChild<QAction *>("PrinterAction");
//        if (print)
//            print->trigger();
//        QTest::qWait(1000);
//    }
//}



///**
// *  @brief 右键打开文件夹
// */
//TEST_F(ImageItemTest, OpenFolder)
//{
//    //右键菜单打开文件夹
//    QMap<int, ImageItem *> it = get_imageitem();
//    QAction *actOpenFolder = mainwindow->findChild<QAction *>("OpenFolderAction");
//    if (it.count() > 0
//        && actOpenFolder) {
//        actOpenFolder->trigger();
//    }
//}

///**
// *  @brief 双击缩略图
// */
//TEST_F(ImageItemTest, mouseDoubleClickEvent)
//{
//    //右键菜单打开文件夹
//    QMap<int, ImageItem *> it = get_imageitem();

//    if (it.count() > 0) {
//        QMap<int, ImageItem *> imgitem = get_imageitem();
//        QTest::mouseDClick(imgitem[1], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    }
//}

///**
// *  @brief 删除单张缩略图
// */
//TEST_F(ImageItemTest, ImageItemDel)
//{

////    QTest::qWait(1000);
//    QMap<int, ImageItem *> it = get_imageitem();
//    ImageItem *imgit = nullptr;

//    if (it.count() > 0) {
//        imgit = it.value(0);
//        if (!imgit)
//            return;

//        //点击鼠标右键
//        QTest::qWait(1000);
//        QTest::mouseMove(imgit, QPoint(0, 0), 500);
//        QTest::mousePress(imgit, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 500);
//        QTest::mouseRelease(imgit, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 0);

//        //点击鼠标左键
//        QTest::qWait(1000);
//        QTest::mouseMove(imgit, QPoint(0, 0), 500);
//        QTest::mousePress(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//        QTest::mouseRelease(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    }

//    if (!it.isEmpty()) {
//        imgit = it.value(0);
//        QTest::keyPress(imgit, Qt::Key_C, Qt::ControlModifier, 500);
//        QTest::keyRelease(imgit, Qt::Key_C, Qt::ControlModifier, 500);
//        QTest::keyClick(imgit, Qt::Key_Delete, Qt::NoModifier, 0);
//    }
//}

///**
// *  @brief 缩略图多选和复制，删除
// */
//TEST_F(ImageItemTest, ImageItemContinuousChoose)
//{
//    mainwindow->settingDialog();
//    QVariant picpath = dc::Settings::get().settings()->option("base.save.picdatapath")->defaultValue();
//    QVariant vdpath = dc::Settings::get().settings()->option("base.save.vddatapath")->defaultValue();
//    dc::Settings::get().setPathOption("picdatapath", QVariant(picpath));
//    dc::Settings::get().setPathOption("vddatapath", QVariant(vdpath));
//    dc::Settings::get().settings()->sync();
//    mainwindow->settingDialogDel();

//    QMap<int, ImageItem *> ImageMap = get_imageitem();
//    QList<ImageItem *> ImageList;
//    ImageList.clear();
//    int number = ImageMap.count();
//    if (number > 0) {
//        ImageList = ImageMap.values();

//        //Ctrl多选
//        for (int i = 0; i < ImageList.count() / 2; i++) {
//            QTest::mouseMove(ImageList[i], QPoint(0, 0), 100);
//            QTest::keyPress(ImageList[i], Qt::Key_Control, Qt::ControlModifier, 200);
//            QTest::mouseClick(ImageList[i], Qt::LeftButton, Qt::ControlModifier, QPoint(0, 0), 200);
//        }
//        QTest::keyRelease(mainwindow, Qt::Key_Control, Qt::NoModifier, 500);

//        //Shift多选
//        if (ImageList.count() > 0) {
//            QTest::mouseMove(ImageList[0], QPoint(0, 0), 200);
//            QTest::keyPress(ImageList[0], Qt::Key_Shift, Qt::ShiftModifier, 300);
//            QTest::mousePress(ImageList[0], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
//            QTest::mouseRelease(ImageList[0], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
//            for (int i = 0; i < ImageList.count() / 2; i++) {
//                QTest::mouseMove(ImageList[i], QPoint(0, 0), 200);
//                QTest::keyPress(ImageList[i], Qt::Key_Shift, Qt::ShiftModifier, 300);
//                QTest::mousePress(ImageList[i], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
//                QTest::mouseRelease(ImageList[i], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
//            }
//            QTest::qWait(1000);
//            QTest::mouseMove(ImageList[0], QPoint(0, 0), 200);
//            QTest::keyPress(ImageList[0], Qt::Key_Shift, Qt::ShiftModifier, 300);
//            QTest::mousePress(ImageList[0], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
//            QTest::mouseRelease(ImageList[0], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
//        }

//        QAction *copyact = mainwindow->findChild<QAction *>("CopyAction");
//        if (copyact)
//            copyact->trigger();

//        QAction *delact = mainwindow->findChild<QAction *>("DelAction");
//        if (delact)
//            delact->trigger();
//    }
//}

// /**
//  * @brief imageitem与thumbnails类未覆盖部分打桩（构造函数存在connect函数，这些未覆盖部分需要尽量往后面放，以免影响其他case）
//  */
// TEST_F(ImageItemTest, imageitem_thumbnails)
// {
//     Stub stub;
//     stub.set(ADDR(QFileInfo, suffix), ADDR(Stub_Function, suffix));
//     ImageItem *imageitem1 = new ImageItem(nullptr, 0, "/a");
//     stub.set(ADDR(ImageItem, parseFromFile), ADDR(Stub_Function, parseFromFile));
//     ImageItem *imageitem2 = new ImageItem(nullptr, 0, QString(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)));
//     delete imageitem1;
//     delete imageitem2;
//     stub.reset(ADDR(QFileInfo, suffix));
//     stub.reset(ADDR(ImageItem, parseFromFile));

//     ThumbnailsBar *thumbnailsBarNew = new ThumbnailsBar();
//     //调用setBtntooltip()
//     thumbnailsBarNew->setBtntooltip();
//     //调用设备不可用状态分支
//     thumbnailsBarNew->onBtnClick();
//     //调用onBtnClick（）设备可用状态分支
//     stub.set(ADDR(DataManager, getdevStatus), ADDR(Stub_Function, getdevStatus));
//     //调用拍照分支的非正在拍照状态
//     thumbnailsBarNew->onBtnClick();
//     //调用拍照分支的正在拍照状态
//     CMainWindow *mw = new CMainWindow();
//     thumbnailsBarNew->onBtnClick();
//     //调用录像分支但不处于正在录像分支
//     access_private_field::CMainWindowm_nActTpye(*mw) = ActTakeVideo;
//     thumbnailsBarNew->ChangeActType(access_private_field::CMainWindowm_nActTpye(*mw));
//     thumbnailsBarNew->onBtnClick();
//     //调用录像分支但处于正在录像分支
//     thumbnailsBarNew->onBtnClick();
//     //还原
//     stub.reset(ADDR(DataManager, getdevStatus));
//     //调用onShortcutDel删除太快分支
//     stub.set(ADDR(QDateTime, msecsTo), ADDR(Stub_Function, msecsTo));
//     thumbnailsBarNew->onShortcutDel();
//     delete thumbnailsBarNew;
//     //还原
//     stub.reset(ADDR(QDateTime, msecsTo));
// }



