/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
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
#include <sys/time.h>
#include <QCollator>
#include <DLabel>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <DDesktopServices>

bool compareByString(const DBImgInfo &str1, const DBImgInfo &str2)
{
    static QCollator sortCollator;

    sortCollator.setNumericMode(true);

    return sortCollator.compare(str1.fileName, str2.fileName) < 0;
}

ThumbnailsBar::ThumbnailsBar(DWidget *parent) : DFloatingWidget(parent)
{
    m_nActTpye = ActTakePic;
    m_bTakePicMode = true;
    m_nItemCount = 0;
    m_nMaxItem = 0;
    m_wgt = new DWidget(this);
    m_hBOx = new QHBoxLayout(m_wgt);
    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(8, 8, 0, 8);
    m_mainLayout->setSpacing(0);
    //setWidget(m_wgt);//放开后缩略图到左上角了

    setBlurBackgroundEnabled(true);

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_4"));
//    pushButton_8 = new DButtonBoxButton(QStyle::SP_ArrowLeft);
//    pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
//    pushButton_8->setFixedWidth(20);
//    horizontalLayout->addWidget(pushButton_8);
    //horizontalLayout->addSpacing(10);

    m_mainLayout->setObjectName(QStringLiteral("horizontalLayout_5"));

    m_mainLayout->addWidget(m_wgt);

//    pushButton_9 = new DButtonBoxButton(QStyle::SP_ArrowRight);
//    pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
//    pushButton_9->setFixedWidth(20);
//    horizontalLayout->addWidget(pushButton_9);


//    horizontalLayout->setStretch(0, 1);
//    horizontalLayout->setStretch(1, 12);
//    horizontalLayout->setStretch(2, 1);

    m_lastButton = new DPushButton(this);
    m_lastButton->setFixedWidth(50);
    QIcon iconPic(":/images/icons/button/photograph.svg");
    m_lastButton->setIcon(iconPic);
    m_lastButton->setIconSize(QSize(18,18));
    DPalette pa;
    QColor clo("#0081FF");
    pa.setColor(DPalette::Button, clo);
    m_lastButton->setPalette(pa);

    connect(m_lastButton,SIGNAL(clicked()),this,SLOT(OnBtnClick()));

    m_mainLayout->addWidget(m_lastButton,Qt::AlignRight);
    m_mainLayout->addSpacing(8);
    this->setLayout(m_mainLayout);
    m_strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Pictures/摄像头";


    this->setContextMenuPolicy(Qt::CustomContextMenu);
    m_hBOx->setMargin(5);

    //layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
    //this->setFixedWidth(300);

//    m_hBOx->setStretch()

//    QDir dir(strPath);
////    if(!dir.exists())
////    {
////        return;
////    }
////    imageList = new QListWidget;
////    //获取所选文件类型过滤器
//    QStringList filters;
////    filters<<QString("*.webm");


////    //定义迭代器并设置过滤器
//    QDirIterator dir_iterator(strPath,
//                              filters,
//                              QDir::Files | QDir::NoSymLinks,
//                              QDirIterator::Subdirectories);
//    DBImgInfo info;
////    while(dir_iterator.hasNext())
////    {
////        info.filePath = dir_iterator.next();
////        info.fileName = dir_iterator.fileInfo().fileName();
////        m_infos.append(info);
////        QListWidgetItem *imageItem = new QListWidgetItem;
////        //为单元项设置属性
////        imageItem->setIcon(QIcon(info.filePath));
////        //imageItem->setText(tr("Browse"));
////        //重新设置单元项图片的宽度和高度
////        imageItem->setSizeHint(QSize(100,120));
////        //将单元项添加到QListWidget中
////        imageList->addItem(imageItem);
////    }

//    strPath = "/home/hujianbo/Pictures/摄像头";
//    //strPath = QFileDialog::getExistingDirectory(this, "选择目录", "./", QFileDialog::ShowDirsOnly);
//    QDir dir1(strPath);
//    if(!dir.exists())
//    {
//        return;
//    }

//    //获取所选文件类型过滤器
//    filters.clear();
//    filters<<QString("*.jpg");

//    //定义迭代器并设置过滤器
//    QDirIterator dir_iterator1(strPath,
//                              filters,
//                              QDir::Files | QDir::NoSymLinks,
//                              QDirIterator::Subdirectories);

//    while(dir_iterator.hasNext())
//    {
//        info.filePath = dir_iterator.next();
//        info.fileName = dir_iterator.fileInfo().fileName();
//        m_infos.append(info);
//        QPixmap *pix = new QPixmap(info.fileName);
////        QListWidgetItem *imageItem = new QListWidgetItem;
////        //为单元项设置属性
////        imageItem->setIcon(QIcon(info.fileName));
////        //imageItem->setText(tr("Browse"));
////        //重新设置单元项图片的宽度和高度
////        imageItem->setSizeHint(QSize(100,120));
////        //将单元项添加到QListWidget中
////        imageList->addItem(imageItem);


//        QLabel *pLabel = new QLabel(m_widget);
//        pLabel->setPixmap(*pix);
//        pLabel->setScaledContents(true);
//        pix->scaled(pLabel->size(), Qt::KeepAspectRatio);
//    }
//    m_widget->show();
//    //imageList->show();
//    std::sort(m_infos.begin(), m_infos.end(), compareByString);
//    load();
}


void ThumbnailsBar::load()
{
    QString path;
    for (int i = 0; i < m_infos.size(); i ++) {
        path = m_infos[i].filePath;
        loadInterface(path);
    }
}

//void ThumbnailsBar::resizeEvent(QResizeEvent *size)
//{
//    int nWidth = this->width();

////    printf("resize w %d, h %d\n", size.w);

//}

void ThumbnailsBar::loadInterface(QString path)
{
    QImage tImg;
    tImg = QImage(path);
    QPixmap pixmap = QPixmap::fromImage(tImg);

    //m_writelock.lockForWrite();
    m_imagemap.insert(path, pixmap.scaledToHeight(IMAGE_HEIGHT_DEFAULT,  Qt::FastTransformation));
    //m_writelock.unlock();
}

//待完善内容：1、先获取路径并排序再加载;2、视频获取第一帧作为缩略图，或者直接贴图。
void ThumbnailsBar::onFileChanged(const QString &strDirectory)
{
    m_nItemCount = 1;
    qDebug() << m_nMaxItem;
    int nLetAddCount = (m_nMaxItem - 58)/(THUMBNAIL_WIDTH+8);

    //获取所选文件类型过滤器
    QStringList filters;
    filters << QString("*.jpg");
    QDir dir(m_strPath);
    if (!dir.exists()) {
        return;
    }

    QLayoutItem *child;
    while ((child = m_hBOx->takeAt(0)) != 0) {
        //setParent为NULL，防止删除之后界面不消失
        if (child->widget()) {
            child->widget()->setParent(NULL);
        }

        delete child;
    }
    //定义迭代器并设置过滤器
    QDirIterator dir_iterator(m_strPath,
                              filters,
                              QDir::Files | QDir::NoSymLinks,
                              QDirIterator::Subdirectories);
    while (dir_iterator.hasNext()) {
        if(nLetAddCount == m_nItemCount){
            break;
        }
        QString strFile = dir_iterator.next();
        QPixmap *pix = new QPixmap(/*dir_iterator.next()*/strFile);
        DLabel *pLabel = new DLabel(this);
        QMenu *menu = new QMenu();
        QAction *actOpen = new QAction(this);
        actOpen->setText("打开");
        QAction *actSave = new QAction(this);
        actSave->setText("另存为");
        QAction *actMove = new QAction(this);
        actMove->setText("移入回收站");
        QAction *actDel = new QAction(this);
        actDel->setText("删除");
        menu->addAction(actOpen);
        menu->addAction(actSave);
        menu->addAction(actMove);
        menu->addAction(actDel);
        pLabel->setContextMenuPolicy(Qt::CustomContextMenu);
        //connect(pLabel, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(showListWidgetMenuSlot(QPoint)));

        connect(pLabel, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
            menu->exec(QCursor::pos());
        });
        connect(actOpen, &QAction::triggered, this, [ = ] {
            QString  cmd = QString("xdg-open ") + strFile; //在linux下，可以通过system来xdg-open命令调用默认程序打开文件；
            system(cmd.toStdString().c_str());
        });
        connect(actSave, &QAction::triggered, this, [ = ] {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                            strFile,
                                                            tr("Images (*.jpg)"));
        });
        connect(actMove, &QAction::triggered, this, [ = ] {
            DDesktopServices::trash(strFile);
        });
        connect(actDel, &QAction::triggered, this, [ = ] {
            QFile filetmp(strFile);
            filetmp.remove();
        });
        pLabel->setPixmap(*pix);
        pLabel->setScaledContents(true);
        pLabel->setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
        pix->scaled(pLabel->size(), Qt::KeepAspectRatio);
        m_hBOx->addWidget(pLabel);
        m_hBOx->addSpacing(5);
        m_nItemCount ++;
    }

    QString strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Videos/摄像头";
    QStringList filters1;
    filters1 << QString("*.webm") << QString("*.avi");
    QDirIterator dir_iterator1(strPath,
                               filters1,
                               QDir::Files | QDir::NoSymLinks,
                               QDirIterator::Subdirectories);
    while (dir_iterator1.hasNext()) {
        if(nLetAddCount == m_nItemCount){
            break;
        }
        QString strFile = dir_iterator1.next();
        QImage *tmp = new QImage(":/images/123.jpg");
        QPixmap *pix = new QPixmap(/*dir_iterator1.next()*/strFile);
        DLabel *pLabel = new DLabel();
        QMenu *menu = new QMenu();
        QAction *actOpen = new QAction(this);
        actOpen->setText("打开");
        QAction *actSave = new QAction(this);
        actSave->setText("另存为");
        QAction *actMove = new QAction(this);
        actMove->setText("移入回收站");
        QAction *actDel = new QAction(this);
        actDel->setText("删除");
        menu->addAction(actOpen);
        menu->addAction(actSave);
        menu->addAction(actMove);
        menu->addAction(actDel);
        pLabel->setContextMenuPolicy(Qt::CustomContextMenu);
        //connect(pLabel, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(showListWidgetMenuSlot(QPoint)));

        connect(pLabel, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
            menu->exec(QCursor::pos());
        });
        connect(actOpen, &QAction::triggered, this, [ = ] {
            QString  cmd = QString("xdg-open ") + strFile; //在linux下，可以通过system来xdg-open命令调用默认程序打开文件；
            system(cmd.toStdString().c_str());
        });
        connect(actSave, &QAction::triggered, this, [ = ] {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                            strFile,
                                                            tr("Videos (*.avi *.webm)"));
        });
        connect(actMove, &QAction::triggered, this, [ = ] {
            DDesktopServices::trash(strFile);
        });
        connect(actDel, &QAction::triggered, this, [ = ] {
            QFile filetmp(strFile);
            filetmp.remove();
        });
        pLabel->setPixmap(QPixmap::fromImage(*tmp));
        pLabel->setScaledContents(true);
        pLabel->setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
        pix->scaled(pLabel->size(), Qt::KeepAspectRatio);
        m_hBOx->addWidget(pLabel);
        m_hBOx->addSpacing(5);
        m_nItemCount ++;
    }
    emit fitToolBar();
}

void ThumbnailsBar::onBtnClick()
{
    if(m_bTakePicMode){

    }
}

void ThumbnailsBar::ChangeActType(int nType)
{
    if(m_nActTpye == nType){
        return;
    }
    m_nActTpye = nType;
    if(nType == ActTakePic)
    {
        QIcon iconPic(":/images/icons/button/photograph.svg");
        m_lastButton->setIcon(iconPic);
        m_lastButton->setIconSize(QSize(18,18));
        DPalette pa;
        QColor clo("#0081FF");
        pa.setColor(DPalette::Button, clo);
        m_lastButton->setPalette(pa);
    }
    else {
        QIcon iconPic(":/images/icons/button/transcribe.svg");
        m_lastButton->setIcon(iconPic);
        m_lastButton->setIconSize(QSize(18,18));
        DPalette pa;
        QColor clo("#0081FF");
        pa.setColor(DPalette::Button, clo);
        m_lastButton->setPalette(pa);
    }

}



