/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*
* Maintainer: hujianbo <hujianbo@uniontech.com>
*
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

#include "thumbnailsbar.h"
#include "camview.h"
#include "datamanager.h"
#include "Settings.h"
#include "ac-deepin-camera-define.h"

#include <DLabel>
#include <DDesktopServices>
#include <DGuiApplicationHelper>
#include <DSettingsDialog>
#include <DSettingsOption>
#include <DSettings>

#include <QCollator>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDateTime>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QKeyEvent>
#include <QShortcut>
#include <QThread>

#include <sys/time.h>

using namespace dc;

QMap<int, ImageItem *> g_indexImage;

QMap<int, ImageItem *> get_imageitem()
{
    return g_indexImage;
}

ThumbnailsBar::ThumbnailsBar(DWidget *parent)
    : DFloatingWidget(parent)
{
    m_lastButton = new DPushButton(this);
    m_thumbLeftWidget = new ThumbWidget(this);
    m_lastButton->setObjectName(BUTTON_PICTURE_VIDEO);
    m_lastButton->setAccessibleName(BUTTON_PICTURE_VIDEO);
    m_nDelTimes = 0;
    m_strFileName = "";
    setFocus(Qt::OtherFocusReason);
    setFocusPolicy(Qt::NoFocus);
    this->setFramRadius(18);
    initShortcut();
    m_nStatus = STATNULL;
    m_nActTpye = ActTakePic;
    m_nMaxWidth = 0;
    m_hBox = new QHBoxLayout();
    m_thumbLeftWidget->setLayout(m_hBox);
    m_thumbLeftWidget->setObjectName("thumbLeftWidget");
    m_thumbLeftWidget->setFocusPolicy(Qt::TabFocus);
    m_hBox->setContentsMargins(0, 0, 0, 0);
    m_hBox->setSpacing(ITEM_SPACE);
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(5, 0, 5, 0);

    setBlurBackgroundEnabled(true); //设置磨砂效果

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_5"));
    m_mainLayout->addWidget(m_thumbLeftWidget, Qt::AlignLeft);

    m_showVdTime = new DLabel(this);
    m_mainLayout->addWidget(m_showVdTime, Qt::AlignRight);
    m_showVdTime->setFixedWidth(VIDEO_TIME_WIDTH);
    QFont ft("SourceHanSansSC");
    ft.setPixelSize(12);
    ft.setWeight(QFont::Normal);
    m_showVdTime->setFont(ft);

    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
    [ = ](DGuiApplicationHelper::ColorType type) {
        QPalette pltLabel = m_showVdTime->palette();

        if (DGuiApplicationHelper::LightType == type)
            pltLabel.setColor(QPalette::WindowText, QColor(0, 26, 46));
        else
            pltLabel.setColor(QPalette::WindowText, QColor(255, 229, 209));

        m_showVdTime->setPalette(pltLabel);

    });

    m_showVdTime->setAlignment(Qt::AlignCenter);
    m_lastButton->setFixedWidth(LAST_BUTTON_WIDTH);
    m_lastButton->setFixedHeight(LAST_BUTTON_HEIGHT);
    QIcon iconPic(":/images/icons/light/button/photograph.svg");
    m_lastButton->setIcon(iconPic);
    m_lastButton->setIconSize(QSize(33, 33));
    m_lastButton->setFocusPolicy(Qt::TabFocus);

    DPalette pa = m_lastButton->palette();
    QColor clo("#0081FF");
    pa.setColor(DPalette::Dark, clo);
    pa.setColor(DPalette::Light, clo);

    m_lastButton->setPalette(pa);
    m_lastButton->setToolTip(tr("Take photo"));
    m_lastButton->setToolTipDuration(500);//0.5s消失

    connect(m_lastButton, SIGNAL(clicked()), this, SLOT(onBtnClick()));

    m_mainLayout->addWidget(m_lastButton, Qt::AlignRight);
    this->setLayout(m_mainLayout);

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_lastDelTime = QDateTime::currentDateTime();
}

void ThumbnailsBar::initShortcut()
{
    QShortcut *shortcutCopy = new QShortcut(QKeySequence("ctrl+c"), this);
    shortcutCopy->setObjectName(SHORTCUT_COPY);
    connect(shortcutCopy, SIGNAL(activated()), this, SLOT(onShortcutCopy()));
    //也可以用Qt::Key_Delete
    QShortcut *shortcutDel = new QShortcut(QKeySequence("delete"), this);
    shortcutDel->setObjectName(SHORTCUT_DELETE);
    connect(shortcutDel, SIGNAL(activated()), this, SLOT(onShortcutDel()));
    //唤起右键菜单
    QShortcut *shortcutMenu = new QShortcut(QKeySequence("Alt+M"), this);
    shortcutMenu->setObjectName(SHORTCUT_CALLMENU);
    connect(shortcutMenu, SIGNAL(activated()), this, SLOT(onCallMenu()));
    //打开文件夹
    QShortcut *shortcutOpenFolder = new QShortcut(QKeySequence("Ctrl+O"), this);
    shortcutOpenFolder->setObjectName(SHORTCUT_OPENFOLDER);
    connect(shortcutOpenFolder, SIGNAL(activated()), this, SLOT(onOpenFolder()));
    //打印
    QShortcut *shortcutPrint = new QShortcut(QKeySequence("Ctrl+P"), this);
    shortcutPrint->setObjectName(SHORTCUT_PRINT);
    connect(shortcutPrint, SIGNAL(activated()), this, SLOT(OnPrint()));
}

void ThumbnailsBar::setBtntooltip()
{
    m_lastButton->setToolTip(tr("Take photo"));
    m_lastButton->setToolTipDuration(500);
}

void ThumbnailsBar::onFoldersChanged(const QString &strDirectory)
{
    Q_UNUSED(strDirectory);
    if (m_nDelTimes > 0) {
        m_nDelTimes--;
        qDebug() << "FoldersChanged but return case del";
        return;
    }

    QString strShowTime = "";
    qDebug() << m_nMaxWidth;
    int nLetAddCount = (m_nMaxWidth - LAST_BUTTON_WIDTH - VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (SELECTED_WIDTH + 2) - 1;//+SELECTED_WIDTH是为了适配多选，避免多选后无法容纳的情况

    QLayoutItem *child;
    while ((child = m_hBox->takeAt(0)) != nullptr) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(child->widget());
        //tmp->deleteLater();
        delete tmp;
        tmp = nullptr;

        //setParent为NULL，防止删除之后界面不消失
        if (child->widget())
            child->widget()->setParent(nullptr);

    }

    g_indexImage.clear();
    m_fileInfoLst.clear();
    m_lstPicFolder.clear();
    m_lstVdFolder.clear();
    if (m_strlstFolders.size() <= 0) {
        emit fitToolBar();
        m_showVdTime->setText("");
        return;
    }
    //获取所选文件类型过滤器
    QStringList filters;
    filters << QString("*.jpg") << /*QString("*.mp4") << */QString("*.webm");
    int tIndex = 0;
    QString strFolder;

    strFolder = m_strlstFolders[0];
    QDir dir1(strFolder);
    //按时间逆序排序
    dir1.setNameFilters(filters);
    dir1.setSorting(QDir::Time /*| QDir::Reversed*/);
    if (dir1.exists())
        m_lstPicFolder += dir1.entryInfoList();

    if (m_strlstFolders.size() > 1) {
        strFolder = m_strlstFolders[1];
    } else {
        strFolder = "";
    }

    QDir dir2(strFolder);
    //按时间逆序排序
    dir2.setNameFilters(filters);
    dir2.setSorting(QDir::Time /*| QDir::Reversed*/);
    if (dir2.exists())
        m_lstVdFolder += dir2.entryInfoList();

    //将两个tmpLst合并排序，赋值给m_fileInfoLst
    while (m_fileInfoLst.size() < nLetAddCount) {
        if (m_lstPicFolder.size() > 0) {
            if (m_lstVdFolder.size() > 0) {
                QFileInfo fileInfo1 = m_lstPicFolder.at(0);
                QFileInfo fileInfo2 = m_lstVdFolder.at(0);

                if (fileInfo1.fileTime(QFileDevice::FileBirthTime).secsTo(fileInfo2.fileTime(QFileDevice::FileBirthTime)) > 0) {
                    m_fileInfoLst += fileInfo2;
                    m_lstVdFolder.removeAt(0);
                } else {
                    m_fileInfoLst += fileInfo1;
                    m_lstPicFolder.removeAt(0);
                }
            } else {
                QFileInfo fileInfo1 = m_lstPicFolder.at(0);
                m_fileInfoLst += fileInfo1;
                m_lstPicFolder.removeAt(0);
            }
        } else {
            //两个都为空，结束while循环
            if (m_lstVdFolder.size() <= 0) {
                break;
            }
            QFileInfo fileInfo2 = m_lstVdFolder.at(0);
            m_fileInfoLst += fileInfo2;
            m_lstVdFolder.removeAt(0);
        }
    }

    QString strFileName = "";
    while (m_fileInfoLst.size() > 0) {

        if (nLetAddCount <= m_hBox->count())
            break;

        QFileInfo fileInfo = m_fileInfoLst.at(0);
        m_fileInfoLst.removeAt(0);
        strFileName = fileInfo.fileName();

        if (!m_strFileName.isEmpty() && fileInfo.fileName().compare(m_strFileName) == 0)
            continue;

        ImageItem *pLabel = new ImageItem(tIndex, fileInfo.filePath());
        connect(pLabel, SIGNAL(needFit()), this, SIGNAL(fitToolBar()));
        connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
        connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
        connect(pLabel, SIGNAL(shiftMulti()), this, SLOT(onShiftMulti()));
        g_indexImage.insert(tIndex, pLabel);
        //g_indexImage.insert(tIndex, pLabel);
        tIndex++;

        m_hBox->addWidget(pLabel);

        if (m_hBox->count() == 1)
            strShowTime = pLabel->getDuration();
    }

    if (!g_indexImage.isEmpty())
        DataManager::instance()->setindexNow(g_indexImage.begin().value()->getIndex());

    emit fitToolBar();
    m_showVdTime->setText(strShowTime);
}

void ThumbnailsBar::onBtnClick()
{
    if (DataManager::instance()->getdevStatus() != 2)
        return;

    if (m_nActTpye == ActTakePic) {
        if (m_nStatus == STATPicIng) {
            m_nStatus = STATNULL;
            m_lastButton->setToolTip(tr("Take photo"));
            m_lastButton->setToolTipDuration(500);
            emit enableTitleBar(3);
            emit takePic(false);
            emit enableSettings(true);
            qDebug() << "****Stop Taking photo";//tooltip与正在处理的动作是相反的
        } else {
            m_nStatus = STATPicIng;
            m_lastButton->setToolTip(tr("Stop taking photos"));
            m_lastButton->setToolTipDuration(500);

            //标题栏视频按钮置灰不可选
            emit enableTitleBar(1);

            emit takePic(true);
            emit enableSettings(false);
            qDebug() << "***Take photo";
        }

    } else if (m_nActTpye == ActTakeVideo) {
        if (m_nStatus == STATVdIng) {
            m_nStatus = STATNULL;
            emit enableTitleBar(4);
            emit enableSettings(true);
            emit takeVd();
        } else {
            m_nStatus = STATVdIng;
            this->hide();
            //1、标题栏拍照按钮置灰不可选
            emit enableTitleBar(2);
            //2、禁用设置功能
            emit enableSettings(false);
            //3、录制
            emit takeVd();
        }

    } else
        return;

}

void ThumbnailsBar::onShortcutCopy()
{
    QStringList paths;
    //缩略图个数为零
    if (g_indexImage.size() <= 0)
        return;

    if (DataManager::instance()->m_setIndex.isEmpty()) {
        paths = QStringList(g_indexImage.value(DataManager::instance()->getindexNow())->getPath());
        qDebug() << "sigle way";
    } else {
        QSet<int>::iterator it;
        for (it = DataManager::instance()->m_setIndex.begin(); it != DataManager::instance()->m_setIndex.end(); ++it) {
            paths << g_indexImage.value(*it)->getPath();
            qDebug() << g_indexImage.value(*it)->getPath();
        }

    }

    QClipboard *cb = qApp->clipboard();
    QMimeData *newMimeData = new QMimeData();
    QByteArray gnomeFormat = QByteArray("copy\n");
    QString text;
    QList<QUrl> dataUrls;

    for (QString path : paths) {
        if (!path.isEmpty())
            text += path + '\n';
        dataUrls << QUrl::fromLocalFile(path);

        gnomeFormat.append(QUrl::fromLocalFile(path).toEncoded()).append("\n");
    }

    newMimeData->setText(text.endsWith('\n') ? text.left(text.length() - 1) : text);
    newMimeData->setUrls(dataUrls);

    gnomeFormat.remove(gnomeFormat.length() - 1, 1);
    //本系统(UOS)特有
    newMimeData->setData("x-special/gnome-copied-files", gnomeFormat);
    cb->setMimeData(newMimeData, QClipboard::Clipboard);
}

void ThumbnailsBar::onShortcutDel()
{
    qDebug() << "onShortcutDel";
    //改用datetime，避免跨天之后判断错误
    QDateTime timeNow = QDateTime::currentDateTime();

    if (m_lastDelTime.msecsTo(timeNow) < 100) {
        qDebug() << "del too fast";
        qDebug() << timeNow;
        qDebug() << m_lastDelTime;
        return;
    }

    m_lastDelTime = timeNow;
    onTrashFile();

}

void ThumbnailsBar::onTrashFile()
{
    qDebug() << "onTrashFile";

    if (DataManager::instance()->m_setIndex.isEmpty()) { //删除
        if (g_indexImage.size() <= 0)
            return;

        ImageItem *tmp = g_indexImage.value(DataManager::instance()->getindexNow());

        if (tmp == nullptr) {
            qDebug() << "ImageItem not exist !";
            qDebug() << "DataManager::instance()->getindexNow()=" << DataManager::instance()->getindexNow();

            ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
            DataManager::instance()->setindexNow(itemNow->getIndex());
            return;
        }

        QString strPath = tmp->getPath();
        QFile file(strPath);

        if (!file.exists()) {
            qDebug() << "file not exist !";
            qDebug() << DataManager::instance()->getindexNow() << " " << strPath;
        }

        bool bTrashed = DDesktopServices::trash(strPath);

        if (!bTrashed) {
            qDebug() << "trash failed!";
            qDebug() << "path is " << strPath;
        }

        delFile(strPath);
    } else {//边删边加会乱掉，先删完再加
        //获取最大的set值
        if (g_indexImage.isEmpty())
            return;

        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
        int nIndex0 = itemNow->getIndex();
        itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(m_hBox->count() - 1)->widget());
        int nIndex1 = itemNow->getIndex();
        int nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;

        //1、通过路径找到界面图元，区分imageitem删除还是dele键删除
        //2、删除对应图元
        //3、找到set里边对应数据，删除
        //4、读取下一个fileinfolist数据，写到set和图元
        int nCount = DataManager::instance()->m_setIndex.size();
        QSet<int>::iterator it;
        for (it = DataManager::instance()->m_setIndex.begin(); it != DataManager::instance()->m_setIndex.end(); ++it) {
            bool bTrashed = DDesktopServices::trash(g_indexImage.value(*it)->getPath());

            if (!bTrashed) {
                qDebug() << "trash failed!";
                qDebug() << "path is " << g_indexImage.value(*it)->getPath();
            }

            for (int i = 0; i < m_hBox->count(); i ++) {
                ImageItem *itemDel = dynamic_cast<ImageItem *>(m_hBox->itemAt(i)->widget());

                if (itemDel->getPath().compare(g_indexImage.value(*it)->getPath()) == 0) {
                    m_hBox->removeWidget(itemDel);
                    //itemNow->deleteLater();
                    delete itemDel;
                    itemDel = nullptr;
                    break;
                }

            }
            g_indexImage.remove(*it);

        }

        for (int i = 0; i < nCount; i ++) {
            if (m_fileInfoLst.isEmpty() && m_lstPicFolder.isEmpty() && m_lstVdFolder.isEmpty()) {
                emit fitToolBar();
                if (g_indexImage.isEmpty()) {
                    DataManager::instance()->setindexNow(0);
                    m_showVdTime->setText("");
                } else {
                    ImageItem *itemExist = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
                    DataManager::instance()->setindexNow(itemExist->getIndex());
                    m_showVdTime->setText(itemExist->getDuration());
                    QFile file1(itemExist->getPath());
                    if (!file1.exists())
                        qDebug() << "file not exist,delete error";//说明DataManager::instance()->getindexNow()还有问题

                }

                DataManager::instance()->m_setIndex.clear();
                return;
            }

            QFileInfo fileInfo;
            if (m_fileInfoLst.size() > 0) {
                fileInfo = m_fileInfoLst.at(0);
                m_fileInfoLst.removeAt(0);
            } else {
                if (m_lstPicFolder.size() > 0) {
                    if (m_lstVdFolder.size() > 0) {
                        QFileInfo fileInfo1 = m_lstPicFolder.at(0);
                        QFileInfo fileInfo2 = m_lstVdFolder.at(0);

                        if (fileInfo1.fileTime(QFileDevice::FileBirthTime).secsTo(fileInfo2.fileTime(QFileDevice::FileBirthTime)) > 0) {
                            fileInfo = fileInfo2;
                            m_lstVdFolder.removeAt(0);
                        } else {
                            fileInfo = fileInfo1;
                            m_lstPicFolder.removeAt(0);
                        }
                    } else {
                        QFileInfo fileInfo1 = m_lstPicFolder.at(0);
                        fileInfo = fileInfo1;
                        m_lstPicFolder.removeAt(0);
                    }
                } else {
                    QFileInfo fileInfo2 = m_lstVdFolder.at(0);
                    fileInfo = fileInfo2;
                    m_lstVdFolder.removeAt(0);
                }
            }

            int nIndexSupply = nIndexMax + 1 + i;

            ImageItem *pLabel = new ImageItem(nIndexSupply, fileInfo.filePath());
            qDebug() << "supply:" << nIndexSupply << " filename " << fileInfo.fileName();

            if (pLabel == nullptr)
                qDebug() << "error! imageitem is null!!";

            connect(pLabel, SIGNAL(needFit()), this, SIGNAL(fitToolBar()));
            connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
            connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
            connect(pLabel, SIGNAL(shiftMulti()), this, SLOT(onShiftMulti()));
            g_indexImage.insert(nIndexMax + 1 + i, pLabel);
            m_hBox->insertWidget(m_hBox->count(), pLabel);
        }
        emit fitToolBar();
        //g_indexImage里边的数据是已经删掉了的
    }

    DataManager::instance()->m_setIndex.clear();

    if (g_indexImage.isEmpty()) {//判断Layout是否为空是不正确的，可能删的太快导致Layout为空
        DataManager::instance()->setindexNow(0);
        m_showVdTime->setText("");
        return;
    } else {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
        DataManager::instance()->setindexNow(itemNow->getIndex());
        m_showVdTime->setText(itemNow->getDuration());
        QFile file2(itemNow->getPath());

        if (!file2.exists()) {
            qDebug() << "file not exist,delete error";//说明DataManager::instance()->getindexNow()还有问题
            qDebug() << "path : " << itemNow->getPath();
        }

    }

}

void ThumbnailsBar::onShowVdTime(QString str)
{
    m_showVdTime->setText(str);
}

void ThumbnailsBar::onFileName(QString strfilename)
{
    m_strFileName = strfilename;
}

void ThumbnailsBar::onCallMenu()
{
    ImageItem *tmp = g_indexImage.value(DataManager::instance()->getindexNow());
    //判断指针不为空
    if (tmp)
        tmp->showMenu();
}

void ThumbnailsBar::onOpenFolder()
{
    //保证和缩略图打开的一致
    ImageItem *tmp = g_indexImage.value(DataManager::instance()->getindexNow());
    QString save_path = tmp->getPath();

    if (save_path.isEmpty())
        save_path = Settings::get().getOption("base.save.picdatapath").toString();

    if (save_path.size() && save_path[0] == '~')
        save_path.replace(0, 1, QDir::homePath());

    if (!QFileInfo(save_path).exists()) {
        QDir d;
        d.mkpath(save_path);
    }
#ifndef UNITTEST
    Dtk::Widget::DDesktopServices::showFolder(save_path);
#endif
}

void ThumbnailsBar::OnPrint()
{
    ImageItem *tmp = g_indexImage.value(DataManager::instance()->getindexNow());
    //判断指针不为空
    if (tmp)
        tmp->onPrint();
}

void ThumbnailsBar::onShiftMulti()
{
    int n1, n2;
    n1 = n2 = -1;
    //按ui顺序读取出两个索引值的位置
    for (int i = 0; i < m_hBox->count(); i ++) {
        ImageItem *item = dynamic_cast<ImageItem *>(m_hBox->itemAt(i)->widget());
        if (item->getIndex() == DataManager::instance()->getLastIndex() ||
                item->getIndex() == DataManager::instance()->getindexNow()) {
            if (-1 == n1) {
                n1 = i;
            } else {
                n2 = i;
                break;
            }
        }
    }
    //取两个ui索引中间的值并添加到m_setIndex
    for (int i = n1; i <= n2; i ++) {
        ImageItem *item = dynamic_cast<ImageItem *>(m_hBox->itemAt(i)->widget());
        DataManager::instance()->m_setIndex.insert(item->getIndex());
    }
}


void ThumbnailsBar::ChangeActType(enum ActType nType)
{
    if (m_nActTpye == nType)
        return;

    m_nActTpye = nType;

    if (nType == ActTakePic) {
        QIcon iconPic(":/images/icons/light/button/photograph.svg");
        m_lastButton->setIcon(iconPic);
        m_lastButton->setIconSize(QSize(33, 33));

        DPalette pa = m_lastButton->palette();
        QColor clo("#0081FF");
        pa.setColor(DPalette::Dark, clo);
        pa.setColor(DPalette::Light, clo);
        m_lastButton->setPalette(pa);
        m_lastButton->setToolTip(tr("Take photo"));
    } else if (nType == ActTakeVideo) {
        QIcon iconPic(":/images/icons/light/button/transcribe.svg");
        m_lastButton->setIcon(iconPic);
        m_lastButton->setIconSize(QSize(33, 33));

        DPalette pa = m_lastButton->palette();
        QColor clo("#FF0000");
        pa.setColor(DPalette::Dark, clo);
        pa.setColor(DPalette::Light, clo);
        m_lastButton->setPalette(pa);
        m_lastButton->setToolTip(tr("Record video"));
    } else
        return;

}

void ThumbnailsBar::addPaths(QString strPicPath, QString strVdPath)
{
    if (m_strlstFolders.contains(strPicPath) && m_strlstFolders.contains(strVdPath)) {
        return;
    }

    m_strlstFolders.clear();

    if (!strPicPath.isEmpty()) {
        m_strlstFolders.push_back(strPicPath);
    }
    //两个路径不相同并且都不为空才添加
    if (strPicPath.compare(strVdPath) != 0 && !strVdPath.isEmpty()) {
        m_strlstFolders.push_back(strVdPath);
    }

    //这里不能根据m_strlstFolders.size()来判断是否做该动作，如果两个路径都为空，需要在ui上清空缩略图
    onFoldersChanged("");
}

void ThumbnailsBar::addFile(QString strFile)
{
    //避免出现空白图片
    QFileInfo fileinfo(strFile);

    if (!fileinfo.exists())
        return;

    //先删除多余的，保证全选情况下缩略图大小正确
    int nLetAddCount = (m_nMaxWidth - LAST_BUTTON_WIDTH - VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (SELECTED_WIDTH + 2) - 1;
    int nCount = m_hBox->count();

    if (nCount >= nLetAddCount) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBox->itemAt(nCount - 1)->widget());
        g_indexImage.remove(tmp->getIndex());
        QString strPath = tmp->getPath();
        m_hBox->removeWidget(tmp);
        delete tmp;
        tmp = nullptr;
        //ui上删掉的得加回来，否则删除文件的时候，总会剩一个文件
        QFileInfo fileinfo(strPath);
        m_fileInfoLst += fileinfo;
    }

    int nIndexMax = -1;
    bool bSelectedFirst = false;

    if (!m_hBox->isEmpty()) {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
        int nIndex0 = itemNow->getIndex();

        if (nIndex0 == DataManager::instance()->getindexNow())
            bSelectedFirst = true;

        itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(m_hBox->count() - 1)->widget());
        int nIndex1 = itemNow->getIndex();
        nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;
    }

    ImageItem *pLabel = new ImageItem(nIndexMax + 1, strFile);
    connect(pLabel, SIGNAL(needFit()), this, SIGNAL(fitToolBar()));
    connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
    qDebug() << "supply:" << nIndexMax + 1 << " filename " << strFile;
    connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
    connect(pLabel, SIGNAL(shiftMulti()), this, SLOT(onShiftMulti()));
    g_indexImage.insert(nIndexMax + 1, pLabel);

    if (pLabel == nullptr)
        qDebug() << "error! imageitem is null!!";

    m_hBox->insertWidget(0, pLabel);

    //找到对应的widget，然后把选中的item改到对应的DataManager::instance()->getindexNow()
    if (m_hBox->count() <= 1) {//除非最开始没有图元，否则后续根据最开始的图像移动，图像在哪，选中的框在哪儿
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
        DataManager::instance()->setindexNow(itemNow->getIndex());
        m_showVdTime->setText(itemNow->getDuration());
    } else {
        if (bSelectedFirst) {//当选中的是第一个，就永远是第一个，如果不是，则一直框住前面那个
            ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
            DataManager::instance()->setindexNow(tmp->getIndex());
            m_showVdTime->setText(tmp->getDuration());
            QFile file(tmp->getPath());

            if (!file.exists())
                qDebug() << "file not exist,delete error";//说明DataManager::instance()->getindexNow()还有问题

        }

    }

    emit fitToolBar();
    m_strFileName = "";
}

void ThumbnailsBar::delFile(QString strFile)
{
    m_nDelTimes ++;
    //获取最大的set值
    if (m_hBox->isEmpty())
        return;

    //最值始终出现在第一个或者最后一个
    ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(0)->widget());
    int nIndex0 = itemNow->getIndex();
    itemNow = dynamic_cast<ImageItem *>(m_hBox->itemAt(m_hBox->count() - 1)->widget());
    int nIndex1 = itemNow->getIndex();
    int nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;

    //1、通过路径找到界面图元，区分imageitem删除还是dele键删除
    //2、删除对应图元
    //3、找到set里边对应数据，删除
    //4、读取下一个fileinfolist数据，写到set和图元
    bool bRemoved = false;
    for (int i = 0; i < m_hBox->count(); i ++) {
        ImageItem *itemDel = dynamic_cast<ImageItem *>(m_hBox->itemAt(i)->widget());

        if (itemDel->getPath().compare(strFile) == 0) {
            g_indexImage.remove(DataManager::instance()->getindexNow());
            m_hBox->removeWidget(itemDel);
            delete itemDel;
            itemDel = nullptr;
            bRemoved = true;
            break;
        }

    }

    if (!bRemoved)
        qDebug() << "warning: item didn't removed!!!";

    if (!g_indexImage.isEmpty())
        DataManager::instance()->setindexNow(g_indexImage.begin().value()->getIndex());

    //m_fileInfoLst可能添加了数据，并且一定
    if (m_fileInfoLst.isEmpty() && m_lstPicFolder.isEmpty() && m_lstVdFolder.isEmpty()) {
        emit fitToolBar();
        return;
    }

    QFileInfo fileInfo;
    if (m_fileInfoLst.size() > 0) {
        fileInfo = m_fileInfoLst.at(0);
        m_fileInfoLst.removeAt(0);
    } else {
        if (m_lstPicFolder.size() > 0) {
            if (m_lstVdFolder.size() > 0) {
                QFileInfo fileInfo1 = m_lstPicFolder.at(0);
                QFileInfo fileInfo2 = m_lstVdFolder.at(0);

                if (fileInfo1.fileTime(QFileDevice::FileBirthTime).secsTo(fileInfo2.fileTime(QFileDevice::FileBirthTime)) > 0) {
                    fileInfo = fileInfo2;
                    m_lstVdFolder.removeAt(0);
                } else {
                    fileInfo = fileInfo1;
                    m_lstPicFolder.removeAt(0);
                }
            } else {
                QFileInfo fileInfo1 = m_lstPicFolder.at(0);
                fileInfo = fileInfo1;
                m_lstPicFolder.removeAt(0);
            }
        } else {
            QFileInfo fileInfo2 = m_lstVdFolder.at(0);
            fileInfo = fileInfo2;
            m_lstVdFolder.removeAt(0);
        }
    }



    ImageItem *pLabel = new ImageItem(nIndexMax + 1, fileInfo.filePath());
    qDebug() << "supply:" << nIndexMax + 1 << " filename " << fileInfo.fileName();

    if (pLabel == nullptr)
        qDebug() << "error! imageitem is null!!";

    connect(pLabel, SIGNAL(needFit()), this, SIGNAL(fitToolBar()));
    connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
    connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
    connect(pLabel, SIGNAL(shiftMulti()), this, SLOT(onShiftMulti()));
    g_indexImage.insert(nIndexMax + 1, pLabel);
    m_hBox->insertWidget(m_hBox->count(), pLabel);
}

void ThumbnailsBar::widthChanged()
{
    int nLetAddCount = (m_nMaxWidth - LAST_BUTTON_WIDTH - VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (SELECTED_WIDTH + 2) - 1;
    while (m_hBox->count() > nLetAddCount) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBox->itemAt(m_hBox->count() - 1)->widget());
        QString strPath = tmp->getPath();
        g_indexImage.remove(tmp->getIndex());
        m_hBox->removeWidget(tmp);
        //tmp->deleteLater();
        delete tmp;
        tmp = nullptr;

        QFileInfo fileinfo(strPath);
        m_fileInfoLst += fileinfo;
    }

}

void ThumbnailsBar::mousePressEvent(QMouseEvent *ev) //点击空白处的处理
{
    Q_UNUSED(ev);
}

void ThumbnailsBar::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
