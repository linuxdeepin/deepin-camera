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

#include <DLabel>
#include <DDesktopServices>
#include <DGuiApplicationHelper>

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


//QMap<QString, QPixmap> m_imagemap;
QMap<int, ImageItem *> g_indexImage;
int g_indexNow = 0;
QSet<int> g_setIndex;
//extern QString g_strFileName;
extern int g_devStatus;

QMap<int, ImageItem *> get_imageitem()
{
    return g_indexImage;
}

ThumbnailsBar::ThumbnailsBar(DWidget *parent) : DFloatingWidget(parent)
{
    m_nDelTimes = 0;
    m_strFileName = "";
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
    m_nMaxItem = 0;
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
    m_lastButton->setFocusPolicy(Qt::NoFocus);

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
    if (m_nDelTimes > 0) {
        m_nDelTimes--;
        qDebug() << "FoldersChanged but return case del";
        return;
    }
    Q_UNUSED(strDirectory);
    m_nItemCount = 0;
    QString strShowTime = "";
    qDebug() << m_nMaxItem;
    int nLetAddCount = (m_nMaxItem - LAST_BUTTON_WIDTH - VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (THUMBNAIL_WIDTH + 2) - 1;

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
    m_curFileIndex = 0;
    //获取所选文件类型过滤器
    QStringList filters;
    filters << QString("*.jpg") << QString("*.mp4") << QString("*.webm");
    int tIndex = 0;
    QString strFolder;
    for (int i = m_strlstFolders.size(); i >= 1; i--) {
        strFolder = m_strlstFolders[i - 1];
        QDir dir(strFolder);
        //按时间逆序排序
        dir.setNameFilters(filters);
        dir.setSorting(QDir::Time /*| QDir::Reversed*/);
        if (dir.exists()) {
            m_fileInfoLst += dir.entryInfoList();
        }
    }
    QString strFileName = "";
    while (m_fileInfoLst.size() > 0) {
        if (nLetAddCount <= m_nItemCount) {
            m_curFileIndex = m_nItemCount;
            break;
        }

        QFileInfo fileInfo = m_fileInfoLst.at(0);
        m_fileInfoLst.removeAt(0);
        strFileName = fileInfo.fileName();
        if (!m_strFileName.isEmpty() &&fileInfo.fileName().compare(m_strFileName) == 0) {
            continue;
        }
//        if (fileInfo.suffix() == "mp4" || fileInfo.suffix() == "webm") {
//            QString strFileName = fileInfo.fileName();
//        }
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
    if (!g_indexImage.isEmpty()) {
        g_indexNow = g_indexImage.begin().value()->getIndex();
    }

    if (m_lastItemCount != m_nItemCount) {

        m_lastItemCount = m_nItemCount;
    }
    emit fitToolBar();
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
            qDebug() << "****Stop Taking photo";//tooltip与正在处理的动作是相反的
        } else {
            m_nStatus = STATPicIng;
            m_lastButton->setToolTip(tr("Stop taking photos"));
            m_lastButton->setToolTipDuration(500);
            //1、标题栏视频按钮置灰不可选
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

    } else {
        return;
    }
}

void ThumbnailsBar::onShortcutCopy()
{
    QStringList paths;
    if (g_setIndex.isEmpty()) {
        paths = QStringList(g_indexImage.value(g_indexNow)->getPath());
        qDebug() << "sigle way";
    } else {
        QSet<int>::iterator it;
        for (it = g_setIndex.begin(); it != g_setIndex.end(); ++it) {
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
    if (g_setIndex.isEmpty()) { //删除
        if (g_indexImage.size() <= 0) {
            return;
        }
        ImageItem *tmp = g_indexImage.value(g_indexNow);
        if (tmp == nullptr) {
            qDebug() << "ImageItem not exist !";
            qDebug() << "g_indexNow=" << g_indexNow;

            ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
            g_indexNow = itemNow->getIndex();
            return;
        }
        QString strPath = tmp->getPath();
        QFile file(strPath);
        if (!file.exists()) {
            qDebug() << "file not exist !";
            qDebug() << g_indexNow << " " << strPath;
        }

        bool bTrashed = DDesktopServices::trash(strPath);
        if (!bTrashed) {
            qDebug() << "trash failed!";
            qDebug() << "path is " << strPath;
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
                qDebug() << "trash failed!";
                qDebug() << "path is " << g_indexImage.value(*it)->getPath();
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
                        qDebug() << "file not exist,delete error";//说明g_indexNow还有问题
                    }
                }
                g_setIndex.clear();
                return;
            }
            QFileInfo fileInfo = m_fileInfoLst.at(0);
            int nIndexSupply = nIndexMax + 1 + i;
            m_fileInfoLst.removeAt(0);
            ImageItem *pLabel = new ImageItem(nIndexSupply, fileInfo.filePath());
            qDebug() << "supply:" << nIndexSupply << " filename " << fileInfo.fileName();
            if (pLabel == nullptr) {
                qDebug() << "error! imageitem is null!!";
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
            qDebug() << "file not exist,delete error";//说明g_indexNow还有问题
            qDebug() << "path : " << itemNow->getPath();
        }
    }
    //qDebug() << "g_indexNow=" << g_indexNow;
}

void ThumbnailsBar::onShowVdTime(QString str)
{
    m_showVdTime->setText(str);
}

void ThumbnailsBar::onFileName(QString strfilename)
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
    int nIndexMax = -1;
    bool bSelectedFirst = false;
    if (!m_hBOx->isEmpty()) {
        ImageItem *itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
        int nIndex0 = itemNow->getIndex();
        if (nIndex0 == g_indexNow) {
            bSelectedFirst = true;
        }
        itemNow = dynamic_cast<ImageItem *>(m_hBOx->itemAt(m_hBOx->count() - 1)->widget());
        int nIndex1 = itemNow->getIndex();
        nIndexMax = nIndex0 > nIndex1 ? nIndex0 : nIndex1;
    }
    ImageItem *pLabel = new ImageItem(nIndexMax + 1, strFile);
    connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
    qDebug() << "supply:" << nIndexMax + 1 << " filename " << strFile;
    connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
    g_indexImage.insert(nIndexMax + 1, pLabel);

    int nLetAddCount = (m_nMaxItem - LAST_BUTTON_WIDTH- VIDEO_TIME_WIDTH - LAST_BUTTON_SPACE * 3) / (THUMBNAIL_WIDTH + 2) - 1;
    if (m_hBOx->count() >= nLetAddCount) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(m_nItemCount - 1)->widget());
        g_indexImage.remove(tmp->getIndex());
        m_hBOx->removeWidget(tmp);
        //tmp->deleteLater();
        delete tmp;
        tmp = nullptr;
    }
    if (pLabel == nullptr) {
        qDebug() << "error! imageitem is null!!";
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
        if (bSelectedFirst) {//当选中的是第一个，就永远是第一个，如果不是，则一直框住前面那个
            ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(0)->widget());
            g_indexNow = tmp->getIndex();
            m_showVdTime->setText(tmp->getDuration());
            QFile file(tmp->getPath());
            if (!file.exists()) {
                qDebug() << "file not exist,delete error";//说明g_indexNow还有问题
            }
        }

    }
    qDebug() << "m_nItemCount " << m_nItemCount;
    emit fitToolBar();
    m_strFileName = "";
}

void ThumbnailsBar::delFile(QString strFile)
{
    m_nDelTimes ++;
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
        qDebug() << "warning: item didn't removed!!!";
    }

    if (!g_indexImage.isEmpty()) {
        g_indexNow = g_indexImage.begin().value()->getIndex();
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
    qDebug() << "supply:" << nIndexMax + 1 << " filename " << fileInfo.fileName();
    if (pLabel == nullptr) {
        qDebug() << "error! imageitem is null!!";
    }
    connect(pLabel, SIGNAL(trashFile()), this, SLOT(onTrashFile()));
    connect(pLabel, SIGNAL(showDuration(QString)), this, SLOT(onShowVdTime(QString)));
    g_indexImage.insert(nIndexMax + 1, pLabel);
    m_hBOx->insertWidget(m_hBOx->count(), pLabel);

    //emit fitToolBar();
}

void ThumbnailsBar::mousePressEvent(QMouseEvent *ev) //点击空白处的处理
{
    Q_UNUSED(ev);
}

void ThumbnailsBar::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}
