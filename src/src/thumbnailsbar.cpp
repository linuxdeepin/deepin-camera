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
#include <sys/time.h>
#include <QCollator>
#include <DLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <DDesktopServices>
#include <QDateTime>
#include <QApplication>
#include <QMimeData>
#include <QClipboard>
#include <QKeyEvent>
#include <QShortcut>
#include <QThread>
#include <DGuiApplicationHelper>

//QMap<QString, QPixmap> m_imagemap;
QMap<int, ImageItem *> g_indexImage;
int g_indexNow = 0;
QSet<int> g_setIndex;
//extern QString g_strFileName;
extern int g_devStatus;

ThumbnailsBar::ThumbnailsBar(DWidget *parent) : DFloatingWidget(parent)
{
    //this->grabKeyboard(); //获取键盘事件的关键处理
    setFocus(Qt::OtherFocusReason);
    setFocusPolicy(Qt::StrongFocus);
    //this->setFramRadius(18);
    QShortcut *shortcut = new QShortcut(QKeySequence("ctrl+c"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onShortcutCopy()));
    //也可以用Qt::Key_Delete
    QShortcut *shortcutDel = new QShortcut(QKeySequence("delete"), this);
    connect(shortcutDel, SIGNAL(activated()), this, SLOT(onShortcutDel()));

    m_nStatus = STATNULL;
    m_nActTpye = ActTakePic;
    m_nItemCount = 0;
    m_nMaxWidth = 0;
    m_strFileName = "";
    m_hBOx = new QHBoxLayout();
    m_hBOx->setSpacing(ITEM_SPACE);
    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setBlurBackgroundEnabled(true); //设置磨砂效果

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_4"));

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_5"));

    m_mainLayout->addLayout(m_hBOx, Qt::AlignLeft);

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
        if (DGuiApplicationHelper::LightType == type) {
            pltLabel.setColor(QPalette::WindowText, QColor(0,26,46));
        } else {
            pltLabel.setColor(QPalette::WindowText, QColor(255,229,209));
        }

        m_showVdTime->setPalette(pltLabel);
    });

    m_showVdTime->setAlignment(Qt::AlignCenter);

    m_lastButton = new DPushButton(this);
    //m_lastButton->setStyleSheet("border:2px groove gray;border-radius:10px;padding:2px 4px;");
    //m_lastButton->setStyleSheet("border-radius:8px;");
    m_lastButton->setFixedWidth(LAST_BUTTON_WIDTH);
    m_lastButton->setFixedHeight(LAST_BUTTON_HEIGHT);
    QIcon iconPic(":/images/icons/light/button/photograph.svg");
    m_lastButton->setIcon(iconPic);
    m_lastButton->setIconSize(QSize(33, 33));

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
    m_lastItemCount = 0;
}

void ThumbnailsBar::setBtntooltip()
{
    m_lastButton->setToolTip(tr("Take photo"));
    m_lastButton->setToolTipDuration(500);
}

void ThumbnailsBar::onFoldersChanged(const QString &strDirectory)
{
    Q_UNUSED(strDirectory);
    m_nItemCount = 0;
    QString strShowTime = "";
    qInfo() << m_nMaxWidth;
    int nLetAddCount = (m_nMaxWidth - LAST_BUTTON_WIDTH - VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (THUMBNAIL_WIDTH + 2) - 1;

    QLayoutItem *child;
    while ((child = m_hBOx->takeAt(0)) != nullptr) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(child->widget());
        //tmp->deleteLater();
        delete tmp;
        tmp = nullptr;
        //setParent为NULL，防止删除之后界面不消失
        if (child->widget()) {
            child->widget()->setParent(nullptr);
        }
    }
    g_indexImage.clear();
    m_fileInfoLst.clear();
    m_lstPicFolder.clear();
    m_lstVdFolder.clear();
    m_curFileIndex = 0;

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
        if (sortFileInfoList(m_lstPicFolder, m_lstVdFolder, m_fileInfoLst) == false)
            break;
    }

    QString strFileName = "";
    while (m_fileInfoLst.size() > 0) {
        if (nLetAddCount <= m_nItemCount) {
            m_curFileIndex = m_nItemCount;
            break;
        }

        QFileInfo fileInfo = m_fileInfoLst.first();
        m_fileInfoLst.removeFirst();
        strFileName = fileInfo.fileName();

        if (!m_strFileName.isEmpty() && fileInfo.fileName().compare(m_strFileName) == 0)
            continue;

        ImageItem *pLabel = new ImageItem(tIndex, fileInfo.filePath());
        connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
        connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
        g_indexImage.insert(tIndex, pLabel);
        tIndex++;

        m_hBOx->addWidget(pLabel);
        if (m_nItemCount == 0) {
            strShowTime = pLabel->getDuration();
        }
        m_nItemCount++;
    }
    if (m_lastItemCount != m_nItemCount) {
        emit fitToolBar();
        m_lastItemCount = m_nItemCount;
    }
    m_showVdTime->setText(strShowTime);
}

void ThumbnailsBar::onBtnClick()
{
    if (g_devStatus != 2) {
        return;
    }
    if (m_nActTpye == ActTakePic) {
        if (m_nStatus == STATPicIng) {
            m_nStatus = STATNULL;
            m_lastButton->setToolTip(tr("Take photo"));
            m_lastButton->setToolTipDuration(500);
            emit enableTitleBar(3);
            emit takePic(false);
            emit enableSettings(true);
            qInfo() << "****Stop Taking photo";//tooltip与正在处理的动作是相反的
        } else {
            m_nStatus = STATPicIng;
            m_lastButton->setToolTip(tr("Stop taking photos"));
            m_lastButton->setToolTipDuration(500);
            //1、标题栏视频按钮置灰不可选
            emit enableTitleBar(1);
            emit takePic(true);
            emit enableSettings(false);
            qInfo() << "***Take photo";
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

    } else {
        return;
    }
}

void ThumbnailsBar::onShortcutCopy()
{
    QStringList paths;
    if (g_setIndex.isEmpty()) {
        paths = QStringList(g_indexImage.value(g_indexNow)->getPath());
        qInfo() << "sigle way";
    } else {
        QSet<int>::iterator it;
        for (it = g_setIndex.begin(); it != g_setIndex.end(); ++it) {
            paths << g_indexImage.value(*it)->getPath();
            qInfo() << g_indexImage.value(*it)->getPath();
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
    qInfo() << "onShortcutDel";
    //改用datetime，避免跨天之后判断错误
    QDateTime timeNow = QDateTime::currentDateTime();
    if (m_lastDelTime.msecsTo(timeNow) < 100) {
        qInfo() << "del too fast";
        qInfo() << timeNow;
        qInfo() << m_lastDelTime;
        return;
    }
    m_lastDelTime = timeNow;
    onTrashFile();

}

void ThumbnailsBar::onTrashFile()
{
    qInfo() << "onTrashFile";
    if (g_setIndex.isEmpty()) { //删除
        if (g_indexImage.size() <= 0) {
            return;
        }
        ImageItem *tmp = g_indexImage.value(g_indexNow);
        if (tmp == nullptr) {
            qInfo() << "ImageItem not exist !";
            qInfo() << "g_indexNow=" << g_indexNow;

            ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
            g_indexNow = itemNow->getIndex();
            return;
        }
        QString strPath = tmp->getPath();
        QFile file(strPath);
        if (!file.exists()) {
            qInfo() << "file not exist !";
            qInfo() << g_indexNow << " " << strPath;
        }

        bool bTrashed = DDesktopServices::trash(strPath);
        if (!bTrashed) {
            qInfo() << "trash failed!";
            qInfo() << "path is " << strPath;
        }
        delFile(strPath);
    } else {//边删边加会乱掉，先删完再加
        //获取最大的set值
        if (g_indexImage.isEmpty()) {
            return;
        }
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
        int nIndex0 = itemNow->getIndex();
        itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(m_hBOx->count() - 1)->widget());
        int nIndex1 = itemNow->getIndex();
        int nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;

        //1、通过路径找到界面图元，区分imageitem删除还是dele键删除
        //2、删除对应图元
        //3、找到set里边对应数据，删除
        //4、读取下一个fileinfolist数据，写到set和图元
        int nCount = g_setIndex.size();
        QSet<int>::iterator it;
        for (it = g_setIndex.begin(); it != g_setIndex.end(); ++it) {
            bool bTrashed = DDesktopServices::trash(g_indexImage.value(*it)->getPath());
            if (!bTrashed) {
                qInfo() << "trash failed!";
                qInfo() << "path is " << g_indexImage.value(*it)->getPath();
            }
            for (int i = 0; i < m_hBOx->count(); i ++) {
                ImageItem *itemDel = dynamic_cast<ImageItem *>(m_hBOx->itemAt(i)->widget());
                if (itemDel->getPath().compare(g_indexImage.value(*it)->getPath()) == 0) {
                    m_hBOx->removeWidget(itemDel);
                    //itemNow->deleteLater();
                    delete itemDel;
                    itemDel = nullptr;
                    break;
                }
            }
            g_indexImage.remove(*it);
        }

        for (int i = 0; i < nCount; i ++) {
            if (m_fileInfoLst.isEmpty()) {
                m_nItemCount = m_hBOx->count();
                emit fitToolBar();
                if (g_indexImage.isEmpty()) {
                    g_indexNow = 0;
                    m_showVdTime->setText("");
                } else {
                    ImageItem *itemExist = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
                    g_indexNow = itemExist->getIndex();
                    m_showVdTime->setText(itemExist->getDuration());
                    QFile file1(itemExist->getPath());
                    if (!file1.exists()) {
                        qInfo() << "file not exist,delete error";//说明g_indexNow还有问题
                    }
                }
                g_setIndex.clear();
                return;
            }
            QFileInfo fileInfo = m_fileInfoLst.at(0);
            int nIndexSupply = nIndexMax + 1 + i;
            m_fileInfoLst.removeAt(0);
            ImageItem *pLabel = new ImageItem(nIndexSupply, fileInfo.filePath());
            qInfo() << "supply:" << nIndexSupply << " filename " << fileInfo.fileName();
            if (pLabel == nullptr) {
                qInfo() << "error! imageitem is null!!";
            }
            connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
            connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
            g_indexImage.insert(nIndexMax + 1 + i, pLabel);
            m_hBOx->insertWidget(m_hBOx->count(), pLabel);
        }
        emit fitToolBar();
        //g_indexImage里边的数据是已经删掉了的
    }
    g_setIndex.clear();
    if (g_indexImage.isEmpty()) {//这里判断Layout是否为空是不正确的，可能删的太快导致Layout为空
        g_indexNow = 0;
        m_showVdTime->setText("");
        return;
    } else {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
        g_indexNow = itemNow->getIndex();
        m_showVdTime->setText(itemNow->getDuration());
        QFile file2(itemNow->getPath());
        if (!file2.exists()) {
            qInfo() << "file not exist,delete error";//说明g_indexNow还有问题
            qInfo() << "path : " << itemNow->getPath();
        }
    }
    //qInfo() << "g_indexNow=" << g_indexNow;
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

void ThumbnailsBar::onShowVdTime(QString str)
{
    m_showVdTime->setText(str);
}

void ThumbnailsBar::onFileName(const QString &strfilename)
{
    m_strFileName = strfilename;
}

void ThumbnailsBar::ChangeActType(int nType)
{
    if (m_nActTpye == nType) {
        return;
    }
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
    } else {
        return;
    }

}

void ThumbnailsBar::addPath(QString strPath)
{
    if (!m_strlstFolders.contains(strPath)) {
        m_strlstFolders.push_back(strPath);
        onFoldersChanged("");
    }
}

void ThumbnailsBar::addFile(QString strFile)
{
    /*********方式1,永远指向某一个房间
    //获取当前选中图片在ui中的位置，insert之后，将g_indexNow指向该位置
    int nIndex = -1;
    for (int i = 0; i < m_hBOx->count(); i ++) {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(i)->widget());
        if (itemNow->getIndex() == g_indexNow) {
            nIndex = i;
            break;
        }
    }
    ************/
    //避免出现空白图片
    QFileInfo fileinfo(strFile);
    if (!fileinfo.exists()) {
        return;
    }
    int nIndexMax = -1;
    //bool bSelectedFirst = false;
    if (!m_hBOx->isEmpty()) {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
        int nIndex0 = itemNow->getIndex();
//        if (nIndex0 == g_indexNow) {
//            bSelectedFirst = true;
//        }
        itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(m_hBOx->count() - 1)->widget());
        int nIndex1 = itemNow->getIndex();
        nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;
    }
    ImageItem *pLabel = new ImageItem(nIndexMax + 1, strFile);
    connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
    qInfo() << "supply:" << nIndexMax + 1 << " filename " << strFile;
    connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
    g_indexImage.insert(nIndexMax + 1, pLabel);

    int nIndex = -1;
    int nLetAddCount = (m_nMaxWidth - LAST_BUTTON_WIDTH- VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (THUMBNAIL_WIDTH + 2) - 1;
    if (m_hBOx->count() >= nLetAddCount) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(m_nItemCount - 1)->widget());
        nIndex = tmp->getIndex();
        QString strPath = tmp->getPath();
        g_indexImage.remove(nIndex);
        m_hBOx->removeWidget(tmp);
        //tmp->deleteLater();
        delete tmp;
        tmp = nullptr;
        //ui上删掉的得加回来，否则删除文件的时候，总会剩一个文件
        QFileInfo fileinfotmp(strPath);
        m_fileInfoLst += fileinfotmp;
        if (g_indexNow == nIndex) {
            g_indexNow = nIndexMax + 1;
        }
        QSet<int>::iterator it;
        for (it = g_setIndex.begin(); it != g_setIndex.end(); ++it) {
            if (*it == nIndex) {
                g_setIndex.clear();
                g_setIndex.insert(nIndexMax + 1);
                break;
            }
        }
    }

    if (pLabel == nullptr) {
        qInfo() << "error! imageitem is null!!";
    }
    m_hBOx->insertWidget(0, pLabel);

    m_nItemCount = m_hBOx->count();

    /**************方式1
    //找到对应的widget，然后把选中的item改到对应的g_indexNow
    if (nIndex >= 0) {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(nIndex)->widget());
        g_indexNow = itemNow->getIndex();
    }
    **************/

    if (m_nItemCount <= 1) {//除非最开始没有图元，否则后续根据最开始的图像移动，图像在哪，选中的框在哪儿
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
        g_indexNow = itemNow->getIndex();
        m_showVdTime->setText(itemNow->getDuration());
    } else {
//        if (bSelectedFirst) {//当选中的是第一个，就永远是第一个，如果不是，则一直框住前面那个
//            ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
//            g_indexNow = tmp->getIndex();
//            m_showVdTime->setText(tmp->getDuration());
//            QFile file(tmp->getPath());
//            if (!file.exists()) {
//                qInfo() << "file not exist,delete error";//说明g_indexNow还有问题
//            }
//        }
    }
    qInfo() << "m_nItemCount " << m_nItemCount;
    emit fitToolBar();
    m_strFileName = "";
}

void ThumbnailsBar::delFile(QString strFile)
{
    //获取最大的set值
    if (m_hBOx->isEmpty()) {
        return;
    }

    //最值始终出现在第一个或者最后一个
    ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
    int nIndex0 = itemNow->getIndex();
    itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(m_hBOx->count() - 1)->widget());
    int nIndex1 = itemNow->getIndex();
    int nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;

    //1、通过路径找到界面图元，区分imageitem删除还是dele键删除
    //2、删除对应图元
    //3、找到set里边对应数据，删除
    //4、读取下一个fileinfolist数据，写到set和图元
    bool bRemoved = false;
    for (int i = 0; i < m_hBOx->count(); i ++) {
        ImageItem *itemDel = dynamic_cast<ImageItem *>(m_hBOx->itemAt(i)->widget());
        if (itemDel->getPath().compare(strFile) == 0) {
            g_indexImage.remove(g_indexNow);
            m_hBOx->removeWidget(itemDel);
            //itemNow->deleteLater();
            delete itemDel;
            itemDel = nullptr;
            bRemoved = true;
            break;
        }
    }
    if (!bRemoved) {
        qInfo() << "warning: item didn't removed!!!";
    }

    //g_indexImage里边的数据是已经删掉了的
    if (m_fileInfoLst.isEmpty()) {
        m_nItemCount = m_hBOx->count();
        emit fitToolBar();
        return;
    }

    QFileInfo fileInfo = m_fileInfoLst.at(0);
    m_fileInfoLst.removeAt(0);

    ImageItem *pLabel = new ImageItem(nIndexMax + 1, fileInfo.filePath());
    qInfo() << "supply:" << nIndexMax + 1 << " filename " << fileInfo.fileName();
    if (pLabel == nullptr) {
        qInfo() << "error! imageitem is null!!";
    }
    connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
    connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
    g_indexImage.insert(nIndexMax + 1, pLabel);
    m_hBOx->insertWidget(m_hBOx->count(), pLabel);

    //emit fitToolBar();
}

void ThumbnailsBar::fitSize(int nWidth)
{
    m_nMaxWidth = nWidth;
    int nLetAddCount = (m_nMaxWidth - LAST_BUTTON_WIDTH- VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (THUMBNAIL_WIDTH + 2) - 1;
    int nIndex = -1;
    if (m_hBOx->count() > nLetAddCount) {//当前item个数大于允许显示的个数
        int nDelNum = m_hBOx->count() - nLetAddCount;
        for (int i = 0; i < nDelNum; i ++) {
            ImageItem *tmp = g_indexImage.begin().value();
            nIndex = tmp->getIndex();
            QString strPath = tmp->getPath();
            g_indexImage.remove(nIndex);
            m_hBOx->removeWidget(tmp);
            //tmp->deleteLater();
            delete tmp;
            tmp = nullptr;
            //ui上删掉的得加回来，否则删除文件的时候，总会剩一个文件
            QFileInfo fileinfotmp(strPath);
            m_fileInfoLst += fileinfotmp;
        }
        g_indexNow = g_indexImage.last()->getIndex();
        g_setIndex.clear();
        g_setIndex.insert(g_indexNow);
        m_nItemCount = nLetAddCount;
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

bool ThumbnailsBar::sortFileInfoList(QFileInfoList &firstIn, QFileInfoList &secondIn, QFileInfoList &resultOut)
{
    if (firstIn.size() > 0) {
        if (secondIn.size() > 0) {
            QFileInfo fileInfo1 = firstIn.first();
            QFileInfo fileInfo2 = secondIn.first();

            if (fileInfo1.fileTime(QFileDevice::FileModificationTime).secsTo(fileInfo2.fileTime(QFileDevice::FileModificationTime)) > 0) {
                resultOut += fileInfo2;
                secondIn.removeFirst();
            } else {
                resultOut += fileInfo1;
                firstIn.removeFirst();
            }
        } else {
            QFileInfo fileInfo1 = firstIn.first();
            resultOut += fileInfo1;
            firstIn.removeFirst();
        }
    } else {
        //两个都为空，结束while循环
        if (secondIn.size() <= 0) {
            return false;
        }
        QFileInfo fileInfo2 = secondIn.first();
        resultOut += fileInfo2;
        secondIn.removeFirst();
    }
    return true;
}
