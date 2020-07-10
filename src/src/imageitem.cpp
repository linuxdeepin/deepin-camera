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

extern QSet<int> m_setIndex;
extern QMap<int, ImageItem *> m_indexImage;
extern int m_indexNow;

ImageItem::ImageItem(int index, QString path, QWidget *parent)
{
    Q_UNUSED(parent);
    m_index = index;
    m_path = path;
    setScaledContents(true);
    setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
    QPixmap pix;
    QFileInfo fileInfo(m_path);
    if (fileInfo.suffix() == "mp4" || fileInfo.suffix() == "webm") {
        VideoThumbnailer thumber;
        thumber.setThumbnailSize(100);
        std::vector<uint8_t> buf;
        if (parseFromFile(fileInfo)) {
            thumber.generateThumbnail(m_path.toUtf8().toStdString(), ThumbnailerImageType::Png, buf);
            QImage img = QImage::fromData(buf.data(), int(buf.size()), "png");
            pix = QPixmap::fromImage(img);
        } else {
            pix = QPixmap::fromImage(QImage(":/images/123.jpg"));
        }

        QTime tm(0, 0, 0, 0);
        QPainter painter1;
        painter1.begin(&pix);
        painter1.setPen(Qt::red);
        painter1.setFont(QFont("SourceHanSansSC", 16, QFont::ExtraLight));
        painter1.drawText(10, 0, pix.width() - 2 * 10, pix.height(), Qt::AlignBottom | Qt::AlignHCenter, tm.addSecs(int(m_nDuration) / 1000000).toString("mm:ss"));
        //qDebug() << pix.width() << " " << pix.height();
        //qDebug() << tm.addSecs(int(m_nDuration) / 1000000).toString("mm:ss");
        painter1.end();

        //方式2：先使用线程尝试加载，加载失败只是线程出问题
        //        m_bThumbnailReadOK = false;
        //        QThread *tmpTh = QThread::create([ = ]() {
        //            VideoThumbnailer thumber;
        //            QTime d(0, 0, 0, 0);
        //            thumber.setSeekTime(d.toString("hh:mm:ss:ms").toStdString());
        //            thumber.setThumbnailSize(480 * qApp->devicePixelRatio());
        //            thumber.setMaintainAspectRatio(true);
        //            std::vector<uint8_t> buf;
        //            thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //            m_bThumbnailReadOK = true;
        //            QThread::currentThread()->quit();
        //        });
        //        tmpTh->start();
        //        QThread::msleep(50);
        //        if (m_bThumbnailReadOK) {
        //            thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //            QImage img = QImage::fromData(buf.data(), buf.size(), "png");
        //            //pix = new QPixmap(QPixmap::fromImage(img));
        //            pix = QPixmap::fromImage(QImage(img));
        //        }
        //        else
        //        {
        //            pix = QPixmap::fromImage(QImage(":/images/123.jpg"));
        //        }

        //方式3：数据流加载文件，判断加载的文件是否正确，再读取视频缩略图
        //        QFile f(m_path);//webm生效
        //        bool bValid = false;
        //        if (f.open(QIODevice::ReadOnly)) {
        //            QDataStream ds(&f);
        //            ds >> bValid;
        //            if (bValid) {
        //                thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //                QImage img = QImage::fromData(buf.data(), buf.size(), "png");
        //                pix = QPixmap::fromImage(img);
        //            }
        //            else {
        //                pix = QPixmap::fromImage(QImage(":/images/123.jpg"));
        //            }
        //        }

    } else if (fileInfo.suffix() == "jpg") {
        pix = QPixmap::fromImage(QImage(path));
    } else {
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
    });
    connect(actCopy, &QAction::triggered, this, [ = ] {
        QStringList paths;
        if (m_setIndex.isEmpty())
        {
            paths = QStringList(path);
            qDebug() << "sigle way";
        } else
        {
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
        Dtk::Widget::DDesktopServices::showFolder(strtmp);
    });
    connect(actDel, &QAction::triggered, this, [ = ] {
        if (m_setIndex.isEmpty())
        {
            DDesktopServices::trash(path);
        } else
        {
            QSet<int>::iterator it;
            for (it = m_setIndex.begin(); it != m_setIndex.end(); ++it) {
                DDesktopServices::trash(m_indexImage.value(*it)->getPath());
            }
        }
    });

    pix.scaled(this->size(), Qt::KeepAspectRatio);
}

ImageItem::~ImageItem()
{
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
    if (ev->button() == Qt::LeftButton) {
        if (m_index != m_indexNow) {
            //ImageItem *tItem = m_indexImage.value(_indexNow);
            //tItem->update();
            m_indexNow = m_index;
            update();
        }
    }
    //    if (m_bMulti) {
    //        if (m_index.contains(_index)) {
    //            m_index.remove(_index);
    //        }
    //        else {
    //            m_index.insert(_index);
    //        }
    //    }
}

void ImageItem::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::RightButton) {
        //        if (m_index != m_indexNow) {
        //            m_indexNow = m_index;
        //            //update();
        //        }
        if (m_bMultiSlt) {
            if (m_setIndex.contains(m_index)) {
                m_setIndex.remove(m_index);
            } else {
                m_setIndex.insert(m_index);
            }
        } else {
            m_setIndex.clear();
            m_indexNow = m_index;
        }
    }
    if (ev->button() == Qt::LeftButton) { //左键选择，右键腾出来用于选择菜单
        if (m_bMultiSlt) {
            if (m_setIndex.contains(m_index)) {
                m_setIndex.remove(m_index);
            } else {
                m_setIndex.insert(m_index);
            }
        } else {
            m_setIndex.clear();
            m_indexNow = m_index;
        }
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
    if (m_setIndex.contains(m_index) || (m_setIndex.isEmpty() && m_index == m_indexNow)) {
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
        if (!m_pixmap.isNull()) {
            //            painter.fillRect(pixmapRect,
            //            QBrush(DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color()));
        }

        //        if (themeType == DGuiApplicationHelper::DarkType) {
        //            if(bFirstUpdate)
        //                m_pixmapstring = LOCMAP_SELECTED_DARK;
        //            else
        //                m_pixmapstring = LOCMAP_SELECTED_DAMAGED_DARK;
        //        } else {
        //            if(bFirstUpdate)
        //                m_pixmapstring = LOCMAP_SELECTED_LIGHT;
        //            else
        //                m_pixmapstring = LOCMAP_SELECTED_DAMAGED_LIGHT;
        //        }

        //        QPixmap pixmap = utils::base::renderSVG(m_pixmapstring, QSize(60, 60));
        QPainterPath bg;
        bg.addRoundedRect(pixmapRect, 4, 4);
        if (m_pixmap.isNull()) {
            painter.setClipPath(bg);
            //            painter.drawPixmap(pixmapRect, m_pixmapstring);
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

        if (!m_pixmap.isNull()) {
            //            painter.fillRect(pixmapRect,
            //            QBrush(DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color()));
        }

        //        if (themeType == DGuiApplicationHelper::DarkType) {
        //            if(bFirstUpdate)
        //                m_pixmapstring = LOCMAP_NOT_SELECTED_DARK;
        //            else
        //                m_pixmapstring = LOCMAP_NOT_SELECTED_DAMAGED_DARK;
        //        } else {
        //            if(bFirstUpdate)
        //                m_pixmapstring = LOCMAP_NOT_SELECTED_LIGHT;
        //            else
        //                m_pixmapstring = LOCMAP_NOT_SELECTED_DAMAGED_LIGHT;
        //        }

        //        QPixmap pixmap = utils::base::renderSVG(m_pixmapstring, QSize(30, 40));
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
    //    QPixmap blankPix = _pixmap;
    //    blankPix.fill(Qt::white);

    //    QRect whiteRect;
    //    whiteRect.setX(pixmapRect.x() + 1);
    //    whiteRect.setY(pixmapRect.y() + 1);
    //    whiteRect.setWidth(pixmapRect.width() - 2);
    //    whiteRect.setHeight(pixmapRect.height() - 2);

    QPainterPath bg1;
    bg1.addRoundedRect(pixmapRect, 4, 4);
    painter.setClipPath(bg1);

    //    painter.drawPixmap(pixmapRect, blankPix);
    painter.drawPixmap(pixmapRect, m_pixmap);

    painter.save();
    painter.setPen(
        QPen(DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color(), 1));
    painter.drawRoundedRect(pixmapRect, 4, 4);
    painter.restore();

    //    QTime tm(0,0,0,0);
    //    QPainter painter1;
    //    painter1.begin(&m_pixmap);
    //    painter1.setPen(Qt::red);
    //    painter1.setFont(QFont("SourceHanSansSC", 10, QFont::Black));
    //    painter1.drawText(0,10,this->width(),this->height(), Qt::AlignBottom, tm.addSecs(m_nDuration/1000000).toString("mm:ss"));
    ////    qDebug() << QString::number(m_nDuration);
    ////    qDebug() << tm.addSecs(m_nDuration/1000000).toString("mm:ss");
    //    painter1.end();
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
    avformat_close_input(&av_ctx);

    return true;
}
