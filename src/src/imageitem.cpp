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
#include "imageitem.h"
#include <DGuiApplicationHelper>
#include <QFileInfo>
#include <libffmpegthumbnailer/videothumbnailer.h>
#include <QProcess>
#include <QMouseEvent>
#include <QDebug>
#include <QMimeData>
#include <QClipboard>
#include <QDir>
#include <DDesktopServices>
#include <QTime>
#include <QThread>
extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <libavutil/avutil.h>
}
using namespace ffmpegthumbnailer;

extern QSet<int> g_setIndex;
extern QMap<int, ImageItem *> g_indexImage;
extern int g_indexNow;
bool g_bMultiSlt = false; //是否多选
int g_videoCount;//缩略图的视频个数

ImageItem::ImageItem(int index, QString path, QWidget *parent)
{
    Q_UNUSED(parent);
    m_bVideo = false;
    m_index = index;
    m_path = path;
    setScaledContents(true);
    setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
    QPixmap pix;
    QFileInfo fileInfo(m_path);
    if (fileInfo.suffix() == "mp4" || fileInfo.suffix() == "webm") {
        m_bVideo = true;
        VideoThumbnailer thumber;
        thumber.setThumbnailSize(100);
        std::vector<uint8_t> buf;
        if (parseFromFile(fileInfo)) {
            try {
                thumber.generateThumbnail(m_path.toUtf8().toStdString(), ThumbnailerImageType::Png, buf);//异常视频这里老崩，给上游提交bug的出处

                QImage img = QImage::fromData(buf.data(), int(buf.size()), "png");
                pix = QPixmap::fromImage(img);
            } catch (...) {
                qDebug() << "generateThumbnail failed";
            }
        }
        g_videoCount ++;
        QString strTime = "";
        if (m_nDuration < 0) {
            m_nDuration = 0;
        }
        int nDuration = static_cast<int>(m_nDuration / 1000000);
        int nHour = nDuration / 3600;
        if (nHour == 0) {
            strTime.append("00");
        } else if (nHour < 10) {
            strTime.append("0");
            strTime.append(QString::number(nHour));
        } else {
            strTime.append(QString::number(nHour));
        }
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


        //显示到缩略图上
//        QTime tm(0, 0, 0, 0);
//        QPainter painter1;
//        painter1.begin(&pix);
//        painter1.setPen(Qt::red);
//        QFont font("SourceHanSansSC", 16, QFont::ExtraLight);
//        font.setPixelSize(15);
//        painter1.setFont(font);
//        painter1.drawText(10, 0, pix.width() - 2 * 10, pix.height(), Qt::AlignBottom | Qt::AlignHCenter, tm.addSecs(static_cast<int>(m_nDuration / 1000000)).toString("hh:mm:ss"));
//        painter1.end();
    } else if (fileInfo.suffix() == "jpg") {
        m_strDuratuion = "";
        pix = QPixmap::fromImage(QImage(path));
    } else {
        m_strDuratuion = "";
        //continue; //其他格式不管
        return;
    }
    updatePic(pix);

    QMenu *menu = new QMenu();
    QAction *actCopy = new QAction(this);
    actCopy->setText(tr("Copy"));

    QAction *actDel = new QAction(this);
    actDel->setText(tr("Delete"));
    QAction *actOpenFolder = new QAction(this);
    actOpenFolder->setText(tr("Open folder"));
    menu->addAction(actCopy);
    menu->addAction(actDel);
    menu->addAction(actOpenFolder);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
        Q_UNUSED(pos);
        menu->exec(QCursor::pos());
        g_setIndex.clear();
        g_bMultiSlt = false;
    });
    connect(actCopy, &QAction::triggered, this, [ = ] {
        QStringList paths;
        if (g_setIndex.isEmpty())
        {
            paths = QStringList(path);
            qDebug() << "sigle way";
        } else
        {
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

        //        QImage img(paths.first());//img特有，视频不需要
        //        Q_ASSERT(!img.isNull());
        //        newMimeData->setImageData(img);

        cb->setMimeData(newMimeData, QClipboard::Clipboard);
    });
    connect(actOpenFolder, &QAction::triggered, this, [ = ] {
        QString strtmp = fileInfo.path();
        if (strtmp.size() && strtmp[0] == '~')
        {
            //奇怪，这里不能直接使用strFolder调replace函数
            strtmp.replace(0, 1, QDir::homePath());
        }

        if (g_setIndex.size() <= 1) {
            QUrl url = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
            Dtk::Widget::DDesktopServices::showFileItem(url);
        } else {
            //多选待定义,先打开文件夹吧，可以用showFileItems都选中
            //https://pms.uniontech.com/zentao/bug-view-41745.html
            Dtk::Widget::DDesktopServices::showFolder(strtmp);
        }
    });
    connect(actDel, &QAction::triggered, this, [ = ] {
        emit trashFile();
    });
    //右键菜单先进入aboutToHide再进入QAction::triggered，因此该功能挪到customContextMenuRequested
//    connect(menu,&QMenu::aboutToHide,this, [ = ] {
//        //1、判断shift是否按下，如果按下，不处理，如果未按下，将按下标志置false
//        g_setIndex.clear();
//        g_bMultiSlt = false;//此时shift键焦点已丢失，且暂时还无法获取到是否按下的信息，因此置false，需要多选重新按下即可
//    });

    pix.scaled(this->size(), Qt::KeepAspectRatio);
}

ImageItem::~ImageItem()
{
    QFileInfo fileInfo(m_path);
    if (fileInfo.suffix() == "mp4" || fileInfo.suffix() == "webm") {
        g_videoCount --;
    }
    //    m_menu->deleteLater();
    //    m_actCopy->deleteLater();
    //    m_actDel->deleteLater();
    //    m_actOpenFolder->deleteLater();
}

void ImageItem::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    QFileInfo fileInfo(m_path);
    if (fileInfo.suffix() == "jpg") {
        QString program = "deepin-image-viewer"; //用看图打开
        QStringList arguments;
        arguments << m_path;
        QProcess *myProcess = new QProcess(this);
        myProcess->startDetached(program, arguments);
    } else {
        QString program = "deepin-movie"; //用影院打开
        QStringList arguments;
        arguments << m_path;
        QProcess *myProcess = new QProcess(this);
        myProcess->startDetached(program, arguments);
    }
}

void ImageItem::mouseReleaseEvent(QMouseEvent *ev) //改到缩略图里边重载，然后set到indexnow，现在的方法只是重绘了这一个item
{
    Q_UNUSED(ev);
}

void ImageItem::mousePressEvent(QMouseEvent *ev)
{
    g_indexNow = m_index;
    if (g_bMultiSlt) {
        if (g_setIndex.contains(m_index)) {
            if (ev->button() == Qt::LeftButton) {
                g_setIndex.remove(m_index);
            }
        } else {
            g_setIndex.insert(m_index);
        }
    } else {
        if (ev->button() == Qt::RightButton) {
            return;
        }
        g_setIndex.clear();
    }
    update();
    if (g_setIndex.size() <= 1) {
        emit showDuration(m_strDuratuion);
    } else {
        emit showDuration("... ...");
    }
}
void ImageItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    //qDebug() << "paint this index" << m_index << "indexnow " << m_indexNow  << "setsize " << m_setIndex.size();
    QPainter painter(this);

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    QRect backgroundRect = rect();
    QRect pixmapRect;
    QFileInfo fileinfo(m_path);
    QString str = fileinfo.suffix();
    if (g_setIndex.contains(m_index) || (g_setIndex.isEmpty() && m_index == g_indexNow)) {
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
        if (themeType == DGuiApplicationHelper::LightType) {
            painter.fillRect(pixmapRect, QBrush(Qt::white));
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            painter.fillRect(pixmapRect, QBrush(Qt::black));
        }

        QPainterPath bg;
        bg.addRoundedRect(pixmapRect, 4, 4);
        if (m_pixmap.isNull()) {
            painter.setClipPath(bg);
            QIcon icon(m_pixmapstring);
            icon.paint(&painter, pixmapRect);
        }
        this->setFixedSize(58, 58);
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
            //            painter.drawPixmap(pixmapRect, m_pixmapstring);

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
//        int n,m;
//        n = width();
//        m = height();
        iconRect.setX((width() - 14) / 2);
        iconRect.setY((height() - 14) / 2);
        iconRect.setWidth(14);
        iconRect.setHeight(14);
        if (themeType == DGuiApplicationHelper::LightType) {
            //QIcon icon(":/images/icons/light/play.svg");
            //icon.paint(&painter,iconRect);
            QPixmap pix = QPixmap::fromImage(QImage(":/images/icons/light/play.svg"));
            painter.drawPixmap(iconRect, pix);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            //QIcon icon(":/images/icons/dark/play_dark.svg");
            QPixmap pix = QPixmap::fromImage(QImage(":/images/icons/dark/play_dark.svg"));
            //icon.paint(&painter,iconRect);
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

static int open_codec_context(int *stream_idx,
                              AVCodecParameters **dec_par, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
    int ret, stream_index;
    AVStream *st;


    //AVDictionary *opts = nullptr;
    ret = av_find_best_stream(fmt_ctx, type, -1, -1, nullptr, 0);
    if (ret < 0) {
        qWarning() << "Could not find " << av_get_media_type_string(type)
                   << " stream in input file";
        return ret;
    }

    stream_index = ret;
    st = fmt_ctx->streams[stream_index];
    *dec_par = st->codecpar;
#if LIBAVFORMAT_VERSION_MAJOR >= 57 && LIBAVFORMAT_VERSION_MINOR <= 25
    avcodec_find_decoder((*dec_par)->codec_id);
#else
    /* find decoder for the stream */
    //*dec_ctx = st->codecpar;
    AVCodec *dec = nullptr;
    dec = avcodec_find_decoder(st->codecpar->codec_id);

    if (!dec) {
        fprintf(stderr, "Failed to find %s codec\n",
                av_get_media_type_string(type));
        return AVERROR(EINVAL);
    }
    /* Allocate a codec context for the decoder */
    AVCodecContext *dec_ctx = avcodec_alloc_context3(dec);
    //*dec_par = avcodec_parameters_alloc();
    if (!dec_ctx) {
        fprintf(stderr, "Failed to allocate the %s codec context\n",
                av_get_media_type_string(type));
        return AVERROR(ENOMEM);
    }
    if (avcodec_open2(dec_ctx, dec, nullptr) < 0) {
        fprintf(stderr, "Could not open the codec\n");
    }
    /* Copy codec parameters from input stream to output codec context */
    if ((ret = avcodec_parameters_to_context(dec_ctx, st->codecpar)) < 0) {
        fprintf(stderr, "Failed to copy %s codec parameters to decoder context\n",
                av_get_media_type_string(type));
        return ret;
    }
    ret = avcodec_parameters_from_context(*dec_par, dec_ctx);
    if (ret < 0) {
        fprintf(stderr, "Could not copy the stream parameters\n");
        avcodec_free_context(&dec_ctx);
        exit(1);
    }
    avcodec_free_context(&dec_ctx);
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

    auto ret = avformat_open_input(&av_ctx, fi.filePath().toUtf8().constData(), nullptr, nullptr);
    if (ret < 0) {
        qWarning() << "avformat: could not open input";
        return mi;
    }

    if (avformat_find_stream_info(av_ctx, nullptr) < 0) {
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
    avformat_close_input(&av_ctx);

    return true;
}
