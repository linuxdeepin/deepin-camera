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

//QMap<QString, QPixmap> m_imagemap;
QMap<int, ImageItem *> m_indexImage;
int m_indexNow = 0;
QSet<int> m_setIndex;
extern QString g_strFileName;

bool compareByString(const DBImgInfo &str1, const DBImgInfo &str2)
{
    static QCollator sortCollator;

    sortCollator.setNumericMode(true);

    return sortCollator.compare(str1.fileName, str2.fileName) < 0;
}

ThumbnailsBar::ThumbnailsBar(DWidget *parent) : DFloatingWidget(parent)
{
    this->grabKeyboard(); //获取键盘事件的关键处理
    QShortcut *shortcut = new QShortcut(QKeySequence("ctrl+c"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(onShortcutCopy()));
    QShortcut *shortcutDel = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    connect(shortcutDel, SIGNAL(activated()), this, SLOT(onShortcutDel()));

    m_nStatus = STATNULL;
    m_nActTpye = ActTakePic;
    m_nItemCount = 0;
    m_nMaxItem = 0;
    m_hBOx = new QHBoxLayout();
    m_hBOx->setSpacing(2);
    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(5, 0, 5, 0);

    setBlurBackgroundEnabled(true); //设置磨砂效果

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_4"));

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_5"));

    m_mainLayout->addLayout(m_hBOx, Qt::AlignLeft);
    //    QSpacerItem *horizontalSpacer;
    //    horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    //    m_mainLayout->addItem(horizontalSpacer);

    m_lastButton = new DPushButton(this);

    m_lastButton->setFixedWidth(64);
    m_lastButton->setFixedHeight(50);
    QIcon iconPic(":/images/icons/light/button/photograph.svg");
    m_lastButton->setIcon(iconPic);
    m_lastButton->setIconSize(QSize(24, 20));

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
}

//void ThumbnailsBar::load()
//{
//    QString path;
//    for (int i = 0; i < m_infos.size(); i ++) {
//        path = m_infos[i].filePath;
//        loadInterface(path);
//    }
//}

//void ThumbnailsBar::resizeEvent(QResizeEvent *size)
//{
//    int nWidth = this->width();

////    printf("resize w %d, h %d\n", size.w);

//}

//void ThumbnailsBar::loadInterface(QString path)
//{
//    QImage tImg;
//    tImg = QImage(path);
//    QPixmap pixmap = QPixmap::fromImage(tImg);

//    //m_writelock.lockForWrite();
//    m_imagemap.insert(path, pixmap.scaledToHeight(IMAGE_HEIGHT_DEFAULT,  Qt::FastTransformation));
//    //m_writelock.unlock();
//}

void ThumbnailsBar::onFoldersChanged(const QString &strDirectory)
{
    Q_UNUSED(strDirectory);
    m_nItemCount = 0;
    qDebug() << m_nMaxItem;
    int nLetAddCount = (m_nMaxItem - 64 - 20) / (THUMBNAIL_WIDTH + 2) - 1;

    QLayoutItem *child;
    while ((child = m_hBOx->takeAt(0)) != nullptr) {
        ImageItem *tmp = dynamic_cast<ImageItem *>(child->widget());
        tmp->deleteLater();
        delete tmp;
        tmp = nullptr;
        //setParent为NULL，防止删除之后界面不消失
        if (child->widget()) {
            child->widget()->setParent(nullptr);
        }
    }
    //    m_imagemap.clear();
    m_indexImage.clear();
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
            //使用dir就不要使用迭代器了，会让dir的设置失效
            //QDirIterator dir_iterator(dir);
            QFileInfoList list = dir.entryInfoList();
            for (int i = 0; i < list.size(); ++i) {
                if (nLetAddCount <= m_nItemCount) {
                    break;
                }

                QString strFile = list.at(i).filePath();
                QFileInfo fileInfo = list.at(i);
                if (fileInfo.suffix() == "mkv" || fileInfo.suffix() == "mp4" || fileInfo.suffix() == "webm") {
                    QString strFileName = fileInfo.fileName();
                    if (strFileName.compare(g_strFileName) == 0) {
                        continue; //mp4文件此时还不完整，读取generateThumbnail会崩溃
                    }
                }
                ImageItem *pLabel = new ImageItem(tIndex, fileInfo.filePath());
                m_indexImage.insert(tIndex, pLabel);
                tIndex++;

                m_hBOx->addWidget(pLabel);

                m_nItemCount++;
            }
        }
    }
    emit fitToolBar();
}

void ThumbnailsBar::onBtnClick() //没有相机录像崩溃，待处理
{
    if (m_nActTpye == ActTakePic) {
        if (m_nStatus == STATPicIng) {
            m_nStatus = STATNULL;
            emit enableTitleBar(3);
            emit takePic();
            emit enableSettings(true);
        } else {
            m_nStatus = STATPicIng;
            //1、标题栏视频按钮置灰不可选
            emit enableTitleBar(1);
            emit takePic();
            emit enableSettings(false);
        }

    } else if (m_nActTpye == ActTakeVideo) {
        if (m_nStatus == STATVdIng) {
            m_nStatus = STATNULL;
            emit enableTitleBar(4);
            emit enableSettings(true);
            emit takeVd();
        } else {
            m_nStatus = STATVdIng;
            //1、标题栏拍照按钮置灰不可选
            emit enableTitleBar(2);
            //2、禁用设置功能
            emit enableSettings(false);
            //3、录制
            emit takeVd();

            this->hide();

            m_nItemCount = 0;
            emit fitToolBar();
        }

    } else {
        return;
    }
}

void ThumbnailsBar::onShortcutCopy()
{
    QStringList paths;
    if (m_setIndex.isEmpty()) {
        paths = QStringList(m_indexImage.value(m_indexNow)->getPath());
        qDebug() << "sigle way";
    } else {
        QSet<int>::iterator it;
        for (it = m_setIndex.begin(); it != m_setIndex.end(); ++it) {
            paths << m_indexImage.value(*it)->getPath();
            qDebug() << m_indexImage.value(*it)->getPath();
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
    if (m_setIndex.isEmpty()) {
        DDesktopServices::trash(m_indexImage.value(m_indexNow)->getPath());
    } else {
        QSet<int>::iterator it;
        for (it = m_setIndex.begin(); it != m_setIndex.end(); ++it) {
            DDesktopServices::trash(m_indexImage.value(*it)->getPath());
        }
    }
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
        m_lastButton->setIconSize(QSize(24, 20));

        DPalette pa = m_lastButton->palette();
        QColor clo("#0081FF");
        pa.setColor(DPalette::Dark, clo);
        pa.setColor(DPalette::Light, clo);
        m_lastButton->setPalette(pa);

        m_lastButton->setToolTip(tr("Take photo"));
    } else if (nType == ActTakeVideo) {
        QIcon iconPic(":/images/icons/light/button/transcribe.svg");
        m_lastButton->setIcon(iconPic);
        m_lastButton->setIconSize(QSize(26, 16));

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

void ThumbnailsBar::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) {
        m_bShiftPressed = true;
        m_bMultiSltFlag = true;
        for (int i = 0; i < m_hBOx->count(); i++) {
            ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(i)->widget());
            tmp->SetMulti(true);
        }
        m_setIndex.insert(m_indexNow);
    }
}

void ThumbnailsBar::keyReleaseEvent(QKeyEvent *e)
{
    //预留接口，关于shift按键取消后的操作
    if (e->key() == Qt::Key_Shift) {
        m_bShiftPressed = false;
        m_bMultiSltFlag = false;
        for (int i = 0; i < m_hBOx->count(); i++) {
            ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(i)->widget());
            tmp->SetMulti(false);
        }
    }

    //    if (e->key() == Qt::Key_Shift) {
    //        m_bMulti = false;
    //        m_index.clear();
    //        //不填0就是默认当前选项
    //        //m_index.insert(0);//始终选择第一个，选择当前的话，有可能正好是取消当前选项，此时需要定义选中哪个选项
    //    }
}

void ThumbnailsBar::mousePressEvent(QMouseEvent *ev) //点击空白处的处理
{
    Q_UNUSED(ev);
    //    if (!m_bShiftPressed && m_bMultiSltFlag) {
    //        for (int i = 0; i < m_hBOx->count(); i++) {
    //            ImageItem *tmp = dynamic_cast<ImageItem *>(m_hBOx->itemAt(i)->widget());
    //            tmp->SetMulti(false);
    //            tmp->update();
    //        }
    //        //m_index.insert(_indexNow);
    //    }
}
