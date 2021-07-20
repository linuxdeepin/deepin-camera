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

#include "datamanager.h"
#include "imageitem.h"
#include "ac-deepin-camera-define.h"
#include "capplication.h"

#include <DGuiApplicationHelper>
#include <DDesktopServices>

#include <QFileInfo>
#include <QPainterPath>
#include <QProcess>
#include <QMouseEvent>
#include <QDebug>
#include <QMimeData>
#include <QClipboard>
#include <QDir>
#include <QTime>
#include <QThread>
#include <QShortcut>

#include <libffmpegthumbnailer/videothumbnailerc.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/avutil.h>
#include "malloc.h"
#include "load_libs.h"
}

//extern QMap<int, ImageItem *> g_indexImage;

ImageItem::ImageItem(QWidget *parent): DLabel(parent)
{
    m_bVideo = false;
    m_actPrint = nullptr;
    m_pAniWidget = new AnimationWidget(QPixmap(), this);

    setScaledContents(false);
    setMargin(0);

    initShortcut();

    m_menu = new QMenu(this);

    m_actCopy = new QAction(this);
    m_actCopy->setObjectName("CopyAction");
    m_actCopy->setText(tr("Copy"));

    m_actDel = new QAction(this);
    m_actDel->setObjectName("DelAction");
    m_actDel->setText(tr("Delete"));

    m_actOpenFolder = new QAction(this);
    m_actOpenFolder->setObjectName("OpenFolderAction");
    m_actOpenFolder->setText(tr("Open folder"));

    m_actPrint = new QAction(this);
    m_actPrint->setText(tr("Print"));
    m_actPrint->setObjectName("PrinterAction");

    m_menu->addAction(m_actCopy);
    m_menu->addAction(m_actDel);
    if (!m_bVideo)
        m_menu->addAction(m_actPrint);
    m_menu->addAction(m_actOpenFolder);
    setContextMenuPolicy(Qt::CustomContextMenu);

    //右键菜单
    connect(this, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
        Q_UNUSED(pos);
        if (m_bVideo){
            m_menu->removeAction(m_actPrint);
        } else {
            m_menu->insertAction(m_actOpenFolder, m_actPrint);
        }
        m_menu->exec(QCursor::pos());
        DataManager::instance()->setCtrlMulti(false);
        DataManager::instance()->setShiftMulti(false);
        qDebug() << "Click the right mouse to open the thumbnail menu bar";
    });
    connect(m_actCopy, &QAction::triggered, this, &ImageItem::onCopy);
    connect(m_actOpenFolder, &QAction::triggered, this, &ImageItem::onOpenFolder);
    connect(m_actPrint, &QAction::triggered, this, &ImageItem::onPrint);
    connect(m_actDel, &QAction::triggered, this, [ = ] {
        emit trashFile(m_path);
    }, Qt::QueuedConnection);

    m_lastDelTime = QDateTime::currentDateTime();
}

ImageItem::~ImageItem()
{
    QFileInfo fileInfo(m_path);
    if (fileInfo.suffix() == "webm")
        DataManager::instance()->setvideoCount(DataManager::instance()->getvideoCount() - 1);
  
    delete m_pAniWidget;
    m_pAniWidget = nullptr;
}

void ImageItem::updatePicPath(const QString &filePath)
{
    m_path = filePath;
    QPixmap pix;
    QFileInfo fileInfo(filePath);

    if (fileInfo.suffix() == "webm") {
        m_bVideo = true;
        video_thumbnailer *m_video_thumbnailer = getLoadLibsInstance()->m_video_thumbnailer();
        image_data *m_image_data = getLoadLibsInstance()->m_video_thumbnailer_create_image_data();

        if (parseFromFile(fileInfo)) {
            try {
                //thumber.generateThumbnail(m_path.toUtf8().toStdString(), ThumbnailerImageType::Png, buf);//异常视频这里老崩，给上游提交bug的出处
                getLoadLibsInstance()->m_video_thumbnailer_generate_thumbnail_to_buffer(m_video_thumbnailer, m_path.toUtf8().data(), m_image_data);
                auto img = QImage::fromData(m_image_data->image_data_ptr, static_cast<int>(m_image_data->image_data_size), "png");
                img.scaled(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE);
                pix = QPixmap::fromImage(img);
                malloc_trim(0);
            } catch (...) {
                qWarning() << "generateThumbnail failed";
            }

        }

        getLoadLibsInstance()->m_video_thumbnailer_destroy_image_data(m_image_data);
        getLoadLibsInstance()->m_video_thumbnailer_destroy(m_video_thumbnailer);
        m_image_data = nullptr;
        m_video_thumbnailer = nullptr;

        DataManager::instance()->setvideoCount(DataManager::instance()->getvideoCount() + 1);
        QString strTime = "";

        if (m_nDuration < 0) {
            m_nDuration = 0;
        }

        int nDuration = static_cast<int>(m_nDuration / 1000000);
        int nHour = nDuration / 3600;

        if (nHour == 0)
            strTime.append("00");
        else if (nHour < 10) {
            strTime.append("0");
            strTime.append(QString::number(nHour));
        } else
            strTime.append(QString::number(nHour));

        strTime.append(":");
        int nOutHour = nDuration % 3600;
        int nMins = nOutHour / 60;

        if (nMins == 0) {
            strTime.append("00");
        } else if (nMins < 10) {
            strTime.append("0");
            strTime.append(QString::number(nMins));
        } else {
            strTime.append(QString::number(nMins));
        }

        strTime.append(":");
        int nSecs = nOutHour % 60;

        if (nSecs == 0) {
            strTime.append("00");
        } else if (nSecs < 10) {
            strTime.append("0");
            strTime.append(QString::number(nSecs));
        } else {
            strTime.append(QString::number(nSecs));
        }

        m_strDuratuion = strTime;
    } else if (fileInfo.suffix() == "jpg") {
        m_bVideo = false;
        m_strDuratuion = "";
        QImage img(filePath);
        img = img.scaled(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE);
        pix = QPixmap::fromImage(img);
        malloc_trim(0);
    } else {
        m_strDuratuion = "";
        pix = QPixmap();
//        return;
    }
    updatePic(pix);
    pix.scaled(this->size(), Qt::KeepAspectRatio);
}

void ImageItem::mouseDoubleClickEvent(QMouseEvent *ev)
{
//    if (!CamApp->isPanelEnvironment()) {
//        if (ev->button() == Qt::RightButton)
//            return;

//        QFileInfo fileInfo(m_path);
//        QString program;
//        if (fileInfo.suffix() == "jpg") {
//            program = "deepin-image-viewer"; //用看图打开
//            qDebug() << "Open it with deepin-image-viewer";
//        } else {
//            program = "deepin-movie"; //用影院打开
//            qDebug() << "Open it with deepin-movie";
//        }

//        QStringList arguments;
//        //表示本地文件
//        arguments << QUrl::fromLocalFile(m_path).toString();
//        qInfo() << QUrl::fromLocalFile(m_path).toString();
//        QProcess *myProcess = new QProcess(this);
//        bool bOK = myProcess->startDetached(program, arguments);

//        if (!bOK)
//            qWarning() << "QProcess startDetached error";
//    }
}

void ImageItem::mousePressEvent(QMouseEvent *ev)
{
//    if (DataManager::instance()->getindexNow() != -1) {
//        //当图片铺满缩略图，选中最后一张，拍照；此时缩略图选中的框消失，选择缩略图的某一个图元，程序崩溃
//        //根本问题在于缩略图铺满是，继续添加图元，没有正确维护g_indexNow
//        if (!g_indexImage.contains(DataManager::instance()->getindexNow())) {
//            DataManager::instance()->m_setIndex.clear();
//            DataManager::instance()->setindexNow(g_indexImage.begin().value()->getIndex());
//        }

//        g_indexImage.value(DataManager::instance()->getindexNow())->update();
//    }


//    if (!CamApp->isPanelEnvironment()) {
//        //当按下Ctrl键，多选，鼠标右键弹出右键菜单后松开Ctrl键，此时mainwindow的keyReleaseEvent无法检测到按键时间，因此
//        //此处补充获取Ctrl键盘状态，以避免继续选择图元，应用处于多选状态的问题
//        if (DataManager::instance()->getMultiType() == Ctrl && QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
//            if (DataManager::instance()->m_setIndex.contains(m_index)) {
//                if (ev->button() == Qt::LeftButton) {
//                    DataManager::instance()->m_setIndex.remove(m_index);
//                    if (DataManager::instance()->m_setIndex.size() > 0)
//                        DataManager::instance()->setindexNow(g_indexImage.value(*DataManager::instance()->m_setIndex.begin())->getIndex());
//                }
//            } else {
//                DataManager::instance()->setindexNow(m_index);
//                DataManager::instance()->m_setIndex.insert(m_index);
//            }

//        } else if (DataManager::instance()->getMultiType() == Shift && QGuiApplication::keyboardModifiers() == Qt::ShiftModifier) {
//            if (ev->button() == Qt::LeftButton) {
//                if (DataManager::instance()->getLastIndex() == -1) {//没有上一次的按键记录就添加
//                    DataManager::instance()->setLastIndex(DataManager::instance()->getindexNow());
//                } else {
//                    static int nCount = 0;//确保不会本次和上次重复填导致ui选中效果错误
//                    if (0 == nCount) {
//                        DataManager::instance()->setindexNow(m_index);
//                        nCount ++;
//                    } else {
//                        DataManager::instance()->setLastIndex(m_index);
//                        nCount = 0;
//                    }
//                    DataManager::instance()->m_setIndex.clear();
//                    int nLast = DataManager::instance()->getLastIndex();
//                    int nNow = DataManager::instance()->getindexNow();
//                    if (nLast == nNow) {
//                        DataManager::instance()->setindexNow(m_index);
//                        DataManager::instance()->m_setIndex.clear();
//                        update();
//                        return;
//                    }
//                    emit shiftMulti();
//                }
//            }
//        } else {
//            if (ev->button() == Qt::LeftButton) {
//                //左键点击，清空之前的多选和单选，重新开始选择
//                DataManager::instance()->setindexNow(m_index);
//                DataManager::instance()->m_setIndex.clear();
//                DataManager::instance()->m_setIndex.insert(m_index);
//            }
//        }
//    } else {
//        if (ev->button() == Qt::LeftButton) {
//            //左键点击，清空之前的多选和单选，重新开始选择
//            DataManager::instance()->setindexNow(m_index);
//            DataManager::instance()->m_setIndex.clear();
//            DataManager::instance()->m_setIndex.insert(m_index);
//            m_bMousePress = true;
//        }
//    }
//    update();
//    if (DataManager::instance()->m_setIndex.size() <= 1) {
//        emit showDuration(m_strDuratuion);

//        if (!m_bVideo) {
//            emit showDuration("");
//            if (m_actPrint)
//                m_actPrint->setVisible(true);
//        }

//    } else {
//        QSet<int>::iterator it;
//        bool bHaveVideo = false;

//        for (it = DataManager::instance()->m_setIndex.begin(); it != DataManager::instance()->m_setIndex.end(); ++it) {
//            if (g_indexImage.value(*it)->getIsVideo()) {
//                bHaveVideo = true;
//                break;
//            }
//        }

//        if (bHaveVideo) {
//            emit showDuration("... ...");

//            if (!m_bVideo) //当前为视频是不会new actPrint的
//                m_actPrint->setVisible(false);

//        } else {
//            emit showDuration("");
//            //多选有视频存在,M_bVideo为true
//            if (!m_bVideo)
//                m_actPrint->setVisible(true);
//        }

//    }
//    emit needFit();
}

void ImageItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QRect pixmapRect = rect();
    QRect foregroundRect;
    QFileInfo fileinfo(m_path);
    QString str = fileinfo.suffix();

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    /*if (DataManager::instance()->m_setIndex.contains(m_index)
            || (DataManager::instance()->m_setIndex.isEmpty()
                && m_index == DataManager::instance()->getindexNow())) {
        QPainterPath backgroundBp;
        QRect reduceRect = QRect(backgroundRect.x() + 1, backgroundRect.y() + 1,
                                 backgroundRect.width() - 2, backgroundRect.height() - 2);
        backgroundBp.addRoundedRect(reduceRect, 8, 8);
        painter.setClipPath(backgroundBp);
        painter.fillRect(
            reduceRect,
            QBrush(DGuiApplicationHelper::instance()->applicationPalette().highlight().color()));

        if (m_pixmap.width() > m_pixmap.height()) {
            m_pixmap = m_pixmap.copy((m_pixmap.width() - m_pixmap.height()) / 2, 0, m_pixmap.height(),
                                     m_pixmap.height());
        } else if (m_pixmap.width() < m_pixmap.height()) {
            m_pixmap = m_pixmap.copy(0, (m_pixmap.height() - m_pixmap.width()) / 2, m_pixmap.width(),
                                     m_pixmap.width());
        }

        pixmapRect.setX(backgroundRect.x() + 5);
        pixmapRect.setY(backgroundRect.y() + 5);
        pixmapRect.setWidth(backgroundRect.width() - 10);
        pixmapRect.setHeight(backgroundRect.height() - 10);

        //修复透明图片被选中后透明地方变成绿色
        QPainterPath bg0;
        bg0.addRoundedRect(pixmapRect, 4, 4);
        painter.setClipPath(bg0);

        if (themeType == DGuiApplicationHelper::LightType)
            painter.fillRect(pixmapRect, QBrush(Qt::white));
        else if (themeType == DGuiApplicationHelper::DarkType)
            painter.fillRect(pixmapRect, QBrush(Qt::black));

        QPainterPath bg;
        bg.addRoundedRect(pixmapRect, 4, 4);

        if (m_pixmap.isNull()) {
            painter.setClipPath(bg);
            QIcon icon(m_pixmapstring);
            icon.paint(&painter, pixmapRect);
        }

        this->setFixedSize(SELECTED_WIDTH, SELECTED_WIDTH);
    } else {

    }*/

    pixmapRect.setX(rect().x() + 1);
    pixmapRect.setY(rect().y() + 1);
    pixmapRect.setWidth(rect().width() - 2);
    pixmapRect.setHeight(rect().height() - 2);

    foregroundRect.setX(rect().x() + 2);
    foregroundRect.setY(rect().y() + 2);
    foregroundRect.setWidth(rect().width() - 4);
    foregroundRect.setHeight(rect().height() - 4);

    QPainterPath bg;
    bg.addRoundedRect(rect(), 26, 26);
    painter.fillPath(bg, QBrush(QColor(0, 0, 0, 0.1 * 255)));

    QPainterPath fg;
    fg.addRoundedRect(pixmapRect, 25, 25);
    if (m_pixmap.isNull()) {
        painter.setClipPath(fg);
        QIcon icon(m_pixmapstring);
        icon.paint(&painter, pixmapRect);
    }
    painter.setBrush(QBrush(m_pixmap));
    painter.setPen(Qt::NoPen);
    painter.drawPath(fg);

    QPainterPath inside;
    inside.addRoundedRect(foregroundRect, 24, 24);
    painter.setBrush(Qt::transparent);
    painter.setPen(QColor(255, 255, 255, 0.2 * 255));
    painter.drawPath(inside);

    //加入图标
    /*if (m_bVideo) {
        QRect iconRect;
        iconRect.setX((width() - 14) / 2);
        iconRect.setY((height() - 14) / 2);
        iconRect.setWidth(14);
        iconRect.setHeight(14);

        if (themeType == DGuiApplicationHelper::LightType) {
            QPixmap pix = QPixmap::fromImage(QImage(":/images/icons/light/play.svg"));
            painter.drawPixmap(iconRect, pix);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            QPixmap pix = QPixmap::fromImage(QImage(":/images/icons/dark/play_dark.svg"));
            painter.drawPixmap(iconRect, pix);
        }

    }

    painter.save();
    painter.setPen(
        QPen(DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color(), 1));
    painter.drawRoundedRect(pixmapRect, 4, 4);
    painter.restore();*/
}

void ImageItem::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "mapToParent(event->pos()).x():" << mapToParent(event->pos()).x() << endl;
    qDebug() << "x():" << x() << endl;
    qDebug() << "x()+ width():" << x() + width() << endl;
    qDebug() << "mapToParent(event->pos()).y():" << mapToParent(event->pos()).y() << endl;
    qDebug() << "y():" << y() << endl;
    qDebug() << "y()+ height():" << y() + height() << endl;
    if (mapToParent(event->pos()).x() < x() || mapToParent(event->pos()).x() > (x() + width()))
        m_bMousePress = false;

    if (mapToParent(event->pos()).y() < y() || mapToParent(event->pos()).y() > (y() + height()))
        m_bMousePress = false;
}

void ImageItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QFileInfo fileInfo(m_path);
        QString program;
        if (fileInfo.suffix() == "jpg") {
            program = "deepin-image-viewer"; //用看图打开
            qDebug() << "Open it with deepin-image-viewer";
        } else {
            program = "deepin-movie"; //用影院打开
            qDebug() << "Open it with deepin-movie";
        }

        QStringList arguments;
        //表示本地文件
        arguments << QUrl::fromLocalFile(m_path).toString();
        qInfo() << QUrl::fromLocalFile(m_path).toString();
        QProcess *myProcess = new QProcess(this);
        bool bOK = myProcess->startDetached(program, arguments);
        if (CamApp->isPanelEnvironment())
            CamApp->getMainWindow()->showMinimized();

        if (!bOK)
            qWarning() << "QProcess startDetached error";
    }
}

void ImageItem::showMenu()
{
    QPoint centerpos(width() / 2, height() / 2);
    QPoint screen_centerpos = mapToGlobal(centerpos);
    if (m_bVideo){
        m_menu->removeAction(m_actPrint);
    }
    else {
        m_menu->insertAction(m_actOpenFolder, m_actPrint);
    }
#ifndef UNITTEST
    m_menu->exec(screen_centerpos);
#endif
}

void ImageItem::onOpenFolder()
{
    if (!m_path.isEmpty() && m_path == '~') {
        //这里不能直接使用strFolder调replace函数
        m_path.replace(0, 1, QDir::homePath());
    }
    Dtk::Widget::DDesktopServices::showFolder(m_path);
    qDebug() << "Click the right mouse or press the shortcut to open the folder";
}

void ImageItem::initShortcut()
{
    QShortcut *shortcutCopy = new QShortcut(QKeySequence("ctrl+c"), this);
    shortcutCopy->setObjectName(SHORTCUT_COPY);
    connect(shortcutCopy, SIGNAL(activated()), this, SLOT(onCopy()));
    //也可以用Qt::Key_Delete
    QShortcut *shortcutDel = new QShortcut(QKeySequence("delete"), this);
    shortcutDel->setObjectName(SHORTCUT_DELETE);
    connect(shortcutDel, SIGNAL(activated()), this, SLOT(onShortcutDel()));
    //唤起右键菜单
    QShortcut *shortcutMenu = new QShortcut(QKeySequence("Alt+M"), this);
    shortcutMenu->setObjectName(SHORTCUT_CALLMENU);
    connect(shortcutMenu, SIGNAL(activated()), this, SLOT(showMenu()));
    //打开文件夹
    QShortcut *shortcutOpenFolder = new QShortcut(QKeySequence("Ctrl+O"), this);
    shortcutOpenFolder->setObjectName(SHORTCUT_OPENFOLDER);
    connect(shortcutOpenFolder, &QShortcut::activated, this, &ImageItem::onOpenFolder);
    //打印
    QShortcut *shortcutPrint = new QShortcut(QKeySequence("Ctrl+P"), this);
    shortcutPrint->setObjectName(SHORTCUT_PRINT);
    connect(shortcutPrint, SIGNAL(activated()), this, SLOT(onPrint()));
}

void ImageItem::onCopy()
{
    QStringList paths;
    paths << m_path;

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

void ImageItem::onShortcutDel()
{
    qDebug() << "onShortcutDel";
    //改用datetime，避免跨天之后判断错误
    QDateTime timeNow = QDateTime::currentDateTime();

    if (m_lastDelTime.msecsTo(timeNow) < 100) {
        qDebug() << "del too fast";
        qInfo() << timeNow;
        qInfo() << m_lastDelTime;
        return;
    }

    m_lastDelTime = timeNow;
    emit trashFile(m_path);
}

void ImageItem::onPrint()
{
    if (!m_bVideo) {
        showPrintDialog(QStringList(m_path), this);
    }
}

void ImageItem::showPrintDialog(const QStringList &paths, QWidget *parent)
{
    m_imgs.clear();
    QStringList tempExsitPaths;//保存存在的图片路径
    tempExsitPaths.clear();

    for (const QString &path : paths) {
        QImage img(path);
        if (!img.isNull())
            m_imgs << img;
        tempExsitPaths << paths;
    }

    DPrintPreviewDialog printDialog(parent);
    printDialog.setObjectName(PRINT_DIALOG);
    printDialog.setAccessibleName(PRINT_DIALOG);
#ifndef UNITTEST
    //适配打印接口2.0，dtk大于 5.4.7 版才合入最新的2.0打印控件接口
#if (DTK_VERSION_MAJOR > 5 \
    || (DTK_VERSION_MAJOR >=5 && DTK_VERSION_MINOR > 4) \
    || (DTK_VERSION_MAJOR >= 5 && DTK_VERSION_MINOR >= 4 && DTK_VERSION_PATCH >= 10))//5.4.7暂时没有合入

    if (DApplication::runtimeDtkVersion() >= DTK_VERSION_CHECK(5, 4, 10, 0)) {
        bool suc = printDialog.setAsynPreview(m_imgs.size());//设置总页数，异步方式
        //单张照片设置名称,可能多选照片，但能成功加载的可能只有一张，或从相册中选中的原图片不存在
        if (tempExsitPaths.size() == 1) {
            // 提供包含后缀的文件全名，由打印模块自己处理后缀
            QString docName = QString(QFileInfo(tempExsitPaths.at(0)).completeBaseName());
            docName += ".pdf";
            printDialog.setDocName(docName);
        }//else 多张照片不设置名称，默认使用print模块的print.pdf

        if (suc) //异步
            connect(&printDialog, SIGNAL(paintRequested(DPrinter *, const QVector<int> &)),
                    this, SLOT(paintRequestedAsyn(DPrinter *, const QVector<int> &)));
        else //同步
            connect(&printDialog, SIGNAL(paintRequested(DPrinter *)),
                    this, SLOT(paintRequestSync(DPrinter *)));
    } else {
        connect(&printDialog, SIGNAL(paintRequested(DPrinter *)),
                this, SLOT(paintRequestSync(DPrinter *)));
    }
#else
    connect(&printDialog, SIGNAL(paintRequested(DPrinter *)),
            this, SLOT(paintRequestSync(DPrinter *)));
#endif
    printDialog.exec();
#else
    //printDialog.show();
#endif
}

#if (DTK_VERSION_MAJOR > 5 \
    || (DTK_VERSION_MAJOR >=5 && DTK_VERSION_MINOR > 4) \
    || (DTK_VERSION_MAJOR >= 5 && DTK_VERSION_MINOR >= 4 && DTK_VERSION_PATCH >= 10))//5.4.7暂时没有合入
void ImageItem::paintRequestedAsyn(DPrinter *_printer, const QVector<int> &pageRange)
{
    QPainter painter(_printer);
    qDebug() << pageRange.count();
    int index = 0;
    for (int page : pageRange) {
        if (page < m_imgs.count() + 1) {
            QImage img = m_imgs.at(page - 1);
            if (!img.isNull()) {
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                QRect wRect  = _printer->pageRect();
                QImage tmpMap;

                if (img.width() > wRect.width() || img.height() > wRect.height())
                    tmpMap = img.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                else
                    tmpMap = img;

                QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
                                          qreal(wRect.height() - tmpMap.height()) / 2,
                                          tmpMap.width(), tmpMap.height());

                painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
                                         tmpMap.height()), tmpMap);
            }
            if (index < pageRange.size() - 1) {
                _printer->newPage();
                index++;
            }
        }
    }
    painter.end();
}
#endif

void ImageItem::paintRequestSync(DPrinter *_printer)
{
    int currentIndex = 0;
    QPainter painter(_printer);
    for (QImage img : m_imgs) {

        if (!img.isNull()) {
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setRenderHint(QPainter::SmoothPixmapTransform);
            QRect wRect  = _printer->pageRect();
            QImage tmpMap;

            if (img.width() > wRect.width() || img.height() > wRect.height())
                tmpMap = img.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            else
                tmpMap = img;

            QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
                                      qreal(wRect.height() - tmpMap.height()) / 2,
                                      tmpMap.width(), tmpMap.height());
            painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
                                     tmpMap.height()), tmpMap);
        }

//        if (img != m_imgs.last())
        //不应该将多个相同的图片过滤掉
        if (currentIndex != m_imgs.count() - 1) {
            _printer->newPage();
            currentIndex++;
        }
    }

    painter.end();
}

static int open_codec_context(int *stream_idx, AVCodecParameters **dec_par, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;
    ret = getAvformat()->m_av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
    if (ret < 0) {
        qWarning() << "Could not find " << getAvutil()->m_av_get_media_type_string(type)
                   << " stream in input file";
        return ret;
    }
    stream_index = ret;
    st = fmt_ctx->streams[stream_index];
    *dec_par = st->codecpar;
#if LIBAVFORMAT_VERSION_MAJOR >= 57 && LIBAVFORMAT_VERSION_MINOR <= 25
    getLoadLibsInstance()->m_avcodec_find_decoder((*dec_par)->codec_id);
#else
    /* find decoder for the stream */
    //*dec_ctx = st->codecpar;
    AVCodec *dec = nullptr;
    dec = getLoadLibsInstance()->m_avcodec_find_decoder(st->codecpar->codec_id);

    if (!dec) {
        fprintf(stderr, "Failed to find %s codec\n",
                getAvutil()->m_av_get_media_type_string(type));
        return AVERROR(EINVAL);
    }

    /* Allocate a codec context for the decoder */
    AVCodecContext *dec_ctx = getLoadLibsInstance()->m_avcodec_alloc_context3(dec);

    //*dec_par = getLoadLibsInstance()->m_avcodec_parameters_alloc();
    if (!dec_ctx) {
        fprintf(stderr, "Failed to allocate the %s codec context\n",
                getAvutil()->m_av_get_media_type_string(type));
        return AVERROR(ENOMEM);
    }

    if (getLoadLibsInstance()->m_avcodec_open2(dec_ctx, dec, nullptr) < 0)
        fprintf(stderr, "Could not open the codec\n");

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = getLoadLibsInstance()->m_avcodec_parameters_to_context(dec_ctx, st->codecpar)) < 0) {
        fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                getAvutil()->m_av_get_media_type_string(type));
        return ret;
    }

    ret = getLoadLibsInstance()->m_avcodec_parameters_from_context(*dec_par, dec_ctx);

    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        getLoadLibsInstance()->m_avcodec_free_context(&dec_ctx);
        exit(1);
    }

    getLoadLibsInstance()->m_avcodec_free_context(&dec_ctx);
#endif

    *stream_idx = stream_index;
    return 0;
}

bool ImageItem::parseFromFile(const QFileInfo &fi)
{
    bool mi = false;
    AVFormatContext *av_ctx = nullptr;
    int stream_id = -1;
    AVCodecParameters *dec_ctx = nullptr;

    if (!fi.exists()) {
        return mi;
    }

    auto ret = getAvformat()->m_avformat_open_input(&av_ctx, fi.filePath().toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) {
        qWarning() << "avformat: could not open input";
        return mi;
    }

    if (getAvformat()->m_avformat_find_stream_info(av_ctx, nullptr) < 0) {
        qWarning() << "av_find_stream_info failed";
        return mi;
    }

    if (av_ctx->nb_streams == 0) {
        return mi;
    }

    if (open_codec_context(&stream_id, &dec_ctx, av_ctx, AVMEDIA_TYPE_VIDEO) < 0) {
        return mi;
    }

    m_nDuration = av_ctx->duration;

    if (m_nDuration < 0) {
        return mi;
    }

    getAvformat()->m_avformat_close_input(&av_ctx);
    return true;
}
