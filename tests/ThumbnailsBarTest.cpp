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

#include "ThumbnailsBarTest.h"
#include "../src/src/mainwindow.h"
#include "../src/src/capplication.h"
#include "../src/src/Settings.h"
#include "../src/src/videowidget.h"
#include "../src/src/imageitem.h"
#include "src/Settings.h"
#include "src/thumbnailsbar.h"
#include "v4l2_devices.h"
#include "src/LPF_V4L2.h"
#include "src/majorimageprocessingthread.h"
#include "stub/addr_pri.h"
#include "ac-deepin-camera-define.h"
#include "stub.h"
#include "stub_function.h"
#include "datamanager.h"

#include <QtTest/QTest>

ACCESS_PRIVATE_FIELD(CMainWindow, ActType, m_nActTpye);

ACCESS_PRIVATE_FIELD(ThumbnailsBar, int, m_nDelTimes);
ACCESS_PRIVATE_FIELD(ThumbnailsBar, QStringList, m_strlstFolders);
ACCESS_PRIVATE_FIELD(ThumbnailsBar, QFileInfoList, m_lstPicFolder);
ACCESS_PRIVATE_FIELD(ThumbnailsBar, QFileInfoList, m_lstVdFolder);
ACCESS_PRIVATE_FUN(ThumbnailsBar, bool(QFileInfoList &firstIn, QFileInfoList &secondIn, QFileInfoList &resultOut), sortFileInfoList);

ThumbnailsBarTest::ThumbnailsBarTest()
{
    CMainWindow* mainwindow = CamApp->getMainWindow();
    if (mainwindow) {
        m_thumbnails =  mainwindow->findChild<ThumbnailsBar *>(THUMBNAIL);
    }
}

ThumbnailsBarTest::~ThumbnailsBarTest()
{
    m_thumbnails = NULL;
}

void ThumbnailsBarTest::SetUp()
{

}

void ThumbnailsBarTest::TearDown()
{
    
}

/**
 *  @brief thumbnailsbar类打桩
 */
TEST_F(ThumbnailsBarTest, thumbonfolderchange)
{
    Stub stub;
    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    QVariant picpath = dc::Settings::get().settings()->option("base.save.picdatapath")->defaultValue();
    QVariant vdpath = dc::Settings::get().settings()->option("base.save.vddatapath")->defaultValue();
    dc::Settings::get().setPathOption("picdatapath", QVariant(picpath));
    dc::Settings::get().setPathOption("vddatapath", QVariant(vdpath));
    dc::Settings::get().settings()->sync();

    access_private_field::ThumbnailsBarm_nDelTimes(*m_thumbnails) = 1;
    m_thumbnails->onFoldersChanged("");

    QStringList strlstFolderstmp = access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails);
    access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails).clear();
    m_thumbnails->onFoldersChanged("");

    access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails) = strlstFolderstmp;
    m_thumbnails->onFoldersChanged("");

    QDir dir1(access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails)[0]);
    QStringList filters1;
    QFileInfoList fileinfolist1;
    filters1 << QString("*.jpg") << QString("*.webm");
    dir1.setNameFilters(filters1);
    dir1.setSorting(QDir::Time /*| QDir::Reversed*/);
    if (dir1.exists())
        fileinfolist1 += dir1.entryInfoList();
    for (int i = 0; i < fileinfolist1.size(); i++) {
        QFile file1(fileinfolist1.at(i).filePath());
        file1.open(QFile::Append);
        file1.setFileTime(QDateTime::currentDateTime().addDays(-2 * i), QFileDevice::FileModificationTime);
        file1.close();
    }

    QDir dir2(access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails)[1]);
    QStringList filters2;
    QFileInfoList fileinfolist2;
    filters2 << QString("*.jpg") << QString("*.webm");
    dir2.setNameFilters(filters2);
    dir2.setSorting(QDir::Time /*| QDir::Reversed*/);
    if (dir2.exists())
        fileinfolist2 += dir2.entryInfoList();
    for (int i = 0; i < fileinfolist2.size(); i++) {
        QFile file2(fileinfolist2.at(i).filePath());
        file2.open(QFile::Append);
        file2.setFileTime(QDateTime::currentDateTime().addDays(-2 * i + 1), QFileDevice::FileModificationTime);
        file2.close();
    }
    QFileInfoList result;
    QFileInfoList fileinfolist2tmp, fileinfolist1tmp;
    int nLetAddCount = 10;
    while (result.size() < nLetAddCount) {
        if (result.size() == 3) {
            fileinfolist2tmp = fileinfolist2;
            fileinfolist2.clear();
        }
        if (result.size() == 6) {
            fileinfolist1tmp = fileinfolist1;
            fileinfolist2 = fileinfolist2tmp;
            fileinfolist1.clear();
        }
        if (result.size() == 9) {
            fileinfolist2.clear();
            fileinfolist1.clear();
        }
        if (call_private_fun::ThumbnailsBarsortFileInfoList(*m_thumbnails, fileinfolist1, fileinfolist2, result) == false)
            break;
    }
    m_thumbnails->onFoldersChanged("");
    //调用onBtnClick（）设备可用状态分支
}


/**
 *  @brief thumbnailsbar类打桩
 */
TEST_F(ThumbnailsBarTest, thumbarnail)
{
    Stub stub;
    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    //ThumbnailsBar *thumbnailsBar = mainwindow->findChild<ThumbnailsBar *>(THUMBNAIL);
    //调用setBtntooltip()
    m_thumbnails->setBtntooltip();
    //调用设备不可用状态分支
    m_thumbnails->onBtnClick();
    //onFolderChanged()

    access_private_field::ThumbnailsBarm_nDelTimes(*m_thumbnails) = 1;
    m_thumbnails->onFoldersChanged("");

    QStringList strlstFolderstmp = access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails);
    access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails).clear();
    m_thumbnails->onFoldersChanged("");

    access_private_field::ThumbnailsBarm_strlstFolders(*m_thumbnails) = strlstFolderstmp;
    m_thumbnails->onFoldersChanged("");

    QFileInfoList lstPicFoldertmp = access_private_field::ThumbnailsBarm_lstPicFolder(*m_thumbnails);
    access_private_field::ThumbnailsBarm_lstPicFolder(*m_thumbnails) = access_private_field::ThumbnailsBarm_lstVdFolder(*m_thumbnails);

    access_private_field::ThumbnailsBarm_lstPicFolder(*m_thumbnails) = lstPicFoldertmp;
    m_thumbnails->onFoldersChanged("");

    //调用onBtnClick（）设备可用状态分支
    stub.set(ADDR(DataManager, getdevStatus), ADDR(Stub_Function, getdevStatus));
    //调用拍照分支的非正在拍照状态
    m_thumbnails->onBtnClick();
    //调用拍照分支的正在拍照状态
    //CMainWindow *mw = new CMainWindow();
    m_thumbnails->onBtnClick();
    //调用录像分支但不处于正在录像分支
    //access_private_field::CMainWindowm_nActTpye(*mw) = ActTakeVideo;
    //m_thumbnails->ChangeActType(access_private_field::CMainWindowm_nActTpye(*mw));
    m_thumbnails->ChangeActType(ActTakeVideo);
    m_thumbnails->onBtnClick();
    //调用录像分支但处于正在录像分支
    m_thumbnails->onBtnClick();
    //还原
    stub.reset(ADDR(DataManager, getdevStatus));
    //调用onShortcutDel删除太快分支
    stub.set(ADDR(QDateTime, msecsTo), ADDR(Stub_Function, msecsTo));
    m_thumbnails->onShortcutDel();

    //还原
    stub.reset(ADDR(QDateTime, msecsTo));

    //调用addFile()
    QString str = QString(QDir::homePath() + "/Pictures/" + QObject::tr("Camera"));
    qWarning() << QDir(str).exists();
    m_thumbnails->addFile(str);
    //调用onTrashFile()
    QMap<int, ImageItem *> it = get_imageitem();
    ImageItem *imgit = nullptr;

    bool removeImage = false;
    bool removeVideo = false;
    for(int i=0; i< it.count(); i++){
        imgit = it.value(i);
        if(NULL == imgit) {
            continue;
        }
        if(false == removeImage && false == imgit->getIsVideo()){
            QTest::mouseMove(imgit, QPoint(0, 0), 1000);
            QTest::mousePress(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
            QTest::mouseRelease(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
            QFileInfo filestr(imgit->getPath());
            QString filename = filestr->fileName();

            m_thumbnails->onTrashFile();
            //调用delFile
            m_thumbnails->delFile(filename);
            //调用widthChanged
            m_thumbnails->widthChanged();
            removeImage = true;
        }
        else if(false == removeVideo && imgit->getIsVideo()) {
            QTest::mouseMove(imgit, QPoint(0, 0), 1000);
            QTest::mousePress(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
            QTest::mouseRelease(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
            QFileInfo filestr(imgit->getPath());
            QString filename = filestr->fileName();
            m_thumbnails->onTrashFile();
            //调用delFile
            m_thumbnails->delFile(filename);
            //调用widthChanged
            m_thumbnails->widthChanged();
            removeVideo = true;
        }
        else if(removeVideo && removeImage) {
            break;
        }
    }

    // if (it.count() > 0) {
    //     imgit = it.value(0);
    //     if (!imgit)
    //         return;
    //     QTest::mouseMove(imgit, QPoint(0, 0), 1000);
    //     QTest::mousePress(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    //     QTest::mouseRelease(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    //     QFileInfo filestr(imgit->getPath());

    //     m_thumbnails->onTrashFile();
    //     //调用delFile
    //     m_thumbnails->delFile(str);
    //     //调用widthChanged
    //     m_thumbnails->widthChanged();
    //     // if (filestr.isReadable() && filestr.isWritable() && filestr.isExecutable()) {

    //     // }
    // }
}