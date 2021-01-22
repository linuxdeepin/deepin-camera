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

#include <DGuiApplicationHelper>
#include <DDesktopServices>
#include <DPrintPreviewDialog>

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

#include <libffmpegthumbnailer/videothumbnailerc.h>

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/avutil.h>
#include "malloc.h"
#include "load_libs.h"
}

extern QMap<int, ImageItem *> g_indexImage;

ImageItem::ImageItem(int index, QString path, QWidget *parent)
{
    Q_UNUSED(parent);
    m_bVideo = false;
    m_actPrint = nullptr;
    m_index = index;
    m_path = path;

    setScaledContents(true);
    setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);

    QPixmap pix;
    QFileInfo fileInfo(m_path);

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
        m_strDuratuion = "";
        QImage img(path);
        img = img.scaled(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE);
        pix = QPixmap::fromImage(img);
        malloc_trim(0);
    } else {
        m_strDuratuion = "";
        return;
    }

    updatePic(pix);

    m_menu = new QMenu(this);
    QAction *actCopy = new QAction(this);
    actCopy->setObjectName("CopyAction");
    actCopy->setText(tr("Copy"));

    QAction *actDel = new QAction(this);
    actDel->setObjectName("DelAction");
    actDel->setText(tr("Delete"));

    QAction *actOpenFolder = new QAction(this);
    actOpenFolder->setObjectName("OpenFolderAction");
    actOpenFolder->setText(tr("Open folder"));

    m_menu->addAction(actCopy);
    m_menu->addAction(actDel);

    if (!m_bVideo) {
        m_actPrint = new QAction(this);
        m_actPrint->setText(tr("Print"));
        m_actPrint->setObjectName("PrinterAction");
        m_menu->addAction(m_actPrint);
        /**
         * @brief 右键菜单打印
         */
        connect(m_actPrint, &QAction::triggered, this, &ImageItem::onPrint);
    }

    m_menu->addAction(actOpenFolder);
    setContextMenuPolicy(Qt::CustomContextMenu);

    /**
    *缩略图右键打开菜单栏
    **/
    connect(this, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
        Q_UNUSED(pos);
        m_menu->exec(QCursor::pos());
        DataManager::instance()->setCtrlMulti(false);
        DataManager::instance()->setShiftMulti(false);
        qDebug() << "Click the right mouse to open the thumbnail menu bar";
    });

    /**
     *右键菜单复制
    **/
    connect(actCopy, &QAction::triggered, this, [ = ] {
        QStringList paths;

        if (DataManager::instance()->m_setIndex.isEmpty())
        {
            paths = QStringList(path);
            qDebug() << "sigle way";
        } else
        {
            QSet<int>::iterator it;

            for (it = DataManager::instance()->m_setIndex.begin(); it != DataManager::instance()->m_setIndex.end(); it++) {
                paths << g_indexImage.value(*it)->getPath();
                qInfo() << g_indexImage.value(*it)->getPath();
            }

        }

        QClipboard *cb = qApp->clipboard();
        QMimeData *newMimeData = new QMimeData();
        QByteArray gnomeFormat = QByteArray("copy\n");
        QString text;
        QList<QUrl> dataUrls;

        for (QString path : paths)
        {
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
    });

    /**
     *右键菜单打开文件夹
    **/
    connect(actOpenFolder, &QAction::triggered, this, [ = ] {
        QString strtmp = fileInfo.path();

        if (strtmp.size() && strtmp[0] == '~')
        {
            //这里不能直接使用strFolder调replace函数
            strtmp.replace(0, 1, QDir::homePath());
        }

        if (DataManager::instance()->m_setIndex.size() <= 1)
        {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            Dtk::Widget::DDesktopServices::showFileItem(url);
        } else
        {
            //多选待定义,先打开文件夹吧，可以用showFileItems都选中
            //https://pms.uniontech.com/zentao/bug-view-41745.html
            Dtk::Widget::DDesktopServices::showFolder(strtmp);
        }
        qDebug() << "Click the right mouse to open the folder";
    });

    /**
     *右键菜单删除
    **/
    connect(actDel, &QAction::triggered, this, [ = ] {
        emit trashFile();
    }, Qt::QueuedConnection);
    pix.scaled(this->size(), Qt::KeepAspectRatio);
}

ImageItem::~ImageItem()
{
    QFileInfo fileInfo(m_path);
    if (fileInfo.suffix() == "webm")
        DataManager::instance()->setvideoCount(DataManager::instance()->getvideoCount() - 1);

}

void ImageItem::mouseDoubleClickEvent(QMouseEvent *ev)
{
//    Q_UNUSED(ev);
    if (ev->button() == Qt::RightButton)
        return;

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

    if (!bOK)
        qWarning() << "QProcess startDetached error";

}

void ImageItem::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
}

void ImageItem::mousePressEvent(QMouseEvent *ev)
{
    if (DataManager::instance()->getindexNow() != -1) {
        //当图片铺满缩略图，选中最后一张，拍照；此时缩略图选中的框消失，选择缩略图的某一个图元，程序崩溃
        //根本问题在于缩略图铺满是，继续添加图元，没有正确维护g_indexNow
        if (!g_indexImage.contains(DataManager::instance()->getindexNow())) {
            DataManager::instance()->m_setIndex.clear();
            DataManager::instance()->setindexNow(g_indexImage.begin().value()->getIndex());
        }

        g_indexImage.value(DataManager::instance()->getindexNow())->update();
    }

    //当按下Ctrl键，多选，鼠标右键弹出右键菜单后松开Ctrl键，此时mainwindow的keyReleaseEvent无法检测到按键时间，因此
    //此处补充获取Ctrl键盘状态，以避免继续选择图元，应用处于多选状态的问题
    if (DataManager::instance()->getMultiType() == Ctrl && QGuiApplication::keyboardModifiers() == Qt::ControlModifier) {
        if (DataManager::instance()->m_setIndex.contains(m_index)) {
            if (ev->button() == Qt::LeftButton) {
                DataManager::instance()->m_setIndex.remove(m_index);
                if (DataManager::instance()->m_setIndex.size() > 0) {
                    DataManager::instance()->setindexNow(g_indexImage.value(*DataManager::instance()->m_setIndex.begin())->getIndex());
                }
            }
        } else {
            DataManager::instance()->setindexNow(m_index);
            DataManager::instance()->m_setIndex.insert(m_index);
        }

    } else if (DataManager::instance()->getMultiType() == Shift && QGuiApplication::keyboardModifiers() == Qt::ShiftModifier) {
        if (ev->button() == Qt::LeftButton) {
            if (DataManager::instance()->getLastIndex() == -1) {//没有上一次的按键记录就添加
                DataManager::instance()->setLastIndex(DataManager::instance()->getindexNow());
            } else {
                static int nCount = 0;//确保不会本次和上次重复填导致ui选中效果错误
                if (0 == nCount) {
                    DataManager::instance()->setindexNow(m_index);
                    nCount ++;
                } else {
                    DataManager::instance()->setLastIndex(m_index);
                    nCount = 0;
                }
                DataManager::instance()->m_setIndex.clear();
                int nLast = DataManager::instance()->getLastIndex();
                int nNow = DataManager::instance()->getindexNow();
                if (nLast == nNow) {
                    DataManager::instance()->setindexNow(m_index);
                    DataManager::instance()->m_setIndex.clear();
                    update();
                    return;
                }
                emit shiftMulti();
            }
        }
    } else {
        if (ev->button() == Qt::LeftButton) {
            //左键点击，清空之前的多选和单选，重新开始选择
            DataManager::instance()->setindexNow(m_index);
            DataManager::instance()->m_setIndex.clear();
            DataManager::instance()->m_setIndex.insert(m_index);
        }
    }

    update();

    if (DataManager::instance()->m_setIndex.size() <= 1) {
        emit showDuration(m_strDuratuion);

        if (!m_bVideo) {
            emit showDuration("");
            m_actPrint->setVisible(true);
        }

    } else {
        QSet<int>::iterator it;
        bool bHaveVideo = false;

        for (it = DataManager::instance()->m_setIndex.begin(); it != DataManager::instance()->m_setIndex.end(); it++) {
            if (g_indexImage.value(*it)->getIsVideo()) {
                bHaveVideo = true;
                break;
            }
        }

        if (bHaveVideo) {
            emit showDuration("... ...");

            if (!m_bVideo) //当前为视频是不会new actPrint的
                m_actPrint->setVisible(false);

        } else {
            emit showDuration("");
            //多选有视频存在,M_bVideo为true
            if (!m_bVideo)
                m_actPrint->setVisible(true);
        }

    }
    emit needFit();
}

void ImageItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    QPainter painter(this);

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    QRect backgroundRect = rect();
    QRect pixmapRect;
    QFileInfo fileinfo(m_path);
    QString str = fileinfo.suffix();

    if (DataManager::instance()->m_setIndex.contains(m_index)
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
        pixmapRect.setX(backgroundRect.x() + 1);
        pixmapRect.setY(backgroundRect.y() + 0);
        pixmapRect.setWidth(backgroundRect.width() - 2);
        pixmapRect.setHeight(backgroundRect.height() - 0);

        QPainterPath bg0;
        bg0.addRoundedRect(pixmapRect, 4, 4);
        painter.setClipPath(bg0);

        QPainterPath bg;
        bg.addRoundedRect(pixmapRect, 4, 4);

        if (m_pixmap.isNull()) {
            painter.setClipPath(bg);
            QIcon icon(m_pixmapstring);
            icon.paint(&painter, pixmapRect);
        }

        this->setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
    }

    QPainterPath bg1;
    bg1.addRoundedRect(pixmapRect, 4, 4);
    painter.setClipPath(bg1);

    painter.drawPixmap(pixmapRect, m_pixmap);

    //加入图标
    if (m_bVideo) {
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
    painter.restore();
}

void ImageItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void ImageItem::showMenu()
{
    QPoint centerpos(width() / 2, height() / 2);
    QPoint screen_centerpos = mapToGlobal(centerpos);
#ifndef UNITTEST
    m_menu->exec(screen_centerpos);
#endif
}

void ImageItem::onPrint()
{

    if (!m_bVideo) {
        QStringList paths;

        if (DataManager::instance()->m_setIndex.isEmpty()) {
            paths = QStringList(m_path);
            qDebug() << "sigle print";
        } else {
            QSet<int>::iterator it;

            for (it = DataManager::instance()->m_setIndex.begin(); it != DataManager::instance()->m_setIndex.end(); it++) {
                paths << g_indexImage.value(*it)->getPath();
                qInfo() << g_indexImage.value(*it)->getPath();
            }
        }
        showPrintDialog(QStringList(paths), this);
    }

}

void ImageItem::showPrintDialog(const QStringList &paths, QWidget *parent)
{
    QList<QImage> imgs;

    for (const QString &path : paths) {
        QImage img(path);

        if (!img.isNull())
            imgs << img;
    }

    DPrintPreviewDialog printDialog(parent);
    printDialog.setObjectName(PRINT_DIALOG);
    printDialog.setAccessibleName(PRINT_DIALOG);

#ifndef UNITTEST
    QObject::connect(&printDialog, &DPrintPreviewDialog::paintRequested, parent, [ = ](DPrinter * _printer) {
        QPainter painter(_printer);
        for (QImage img : imgs) {
            if (!img.isNull()) {
                painter.setRenderHint(QPainter::Antialiasing);
                painter.setRenderHint(QPainter::SmoothPixmapTransform);
                QRect wRect  = _printer->pageRect();
                QImage tmpMap;

                if (img.width() > wRect.width() || img.height() > wRect.height()) {
                    tmpMap = img.scaled(wRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                } else {
                    tmpMap = img;
                }

                QRectF drawRectF = QRectF(qreal(wRect.width() - tmpMap.width()) / 2,
                                          qreal(wRect.height() - tmpMap.height()) / 2,
                                          tmpMap.width(), tmpMap.height());
                painter.drawImage(QRectF(drawRectF.x(), drawRectF.y(), tmpMap.width(),
                                         tmpMap.height()), tmpMap);
            }

            if (img != imgs.last()) {
                _printer->newPage();
                qDebug() << "painter newPage!    File:" << __FILE__ << "    Line:" << __LINE__;
            }

        }

        painter.end();
    });

    printDialog.exec();
#else
    printDialog.show();
#endif
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
                av_get_media_type_string(type));
        return AVERROR(EINVAL);
    }

    /* Allocate a codec context for the decoder */
    AVCodecContext *dec_ctx = getLoadLibsInstance()->m_avcodec_alloc_context3(dec);

    //*dec_par = getLoadLibsInstance()->m_avcodec_parameters_alloc();
    if (!dec_ctx) {
        fprintf(stderr, "Failed to allocate the %s codec context\n",
                av_get_media_type_string(type));
        return AVERROR(ENOMEM);
    }

    if (getLoadLibsInstance()->m_avcodec_open2(dec_ctx, dec, nullptr) < 0)
        fprintf(stderr, "Could not open the codec\n");

    /* Copy codec parameters from input stream to output codec context */
    if ((ret = getLoadLibsInstance()->m_avcodec_parameters_to_context(dec_ctx, st->codecpar)) < 0) {
        fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                av_get_media_type_string(type));
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
