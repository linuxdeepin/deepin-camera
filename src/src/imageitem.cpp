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
#include <QProcess>
#include <QMouseEvent>
#include <QDebug>
#include <QMimeData>
#include <QClipboard>
#include <QDir>
#include <DDesktopServices>
#include <libffmpegthumbnailer/videothumbnailer.h>

using namespace ffmpegthumbnailer;

static int _indexNow = 0;
static QSet<int> m_index;
static bool m_bMulti = false; //是否多选
static QMap<int, ImageItem *> m_indexImage;

ImageItem::ImageItem(int index, QString path, QWidget *parent)
{
    Q_UNUSED(parent);
    _index = index;
    _path = path;
    setScaledContents(true);
    setFixedSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);

    QPixmap pix;
    QFileInfo fileInfo(_path);
    if (fileInfo.suffix() == "mkv" || fileInfo.suffix() == "mp4") {
        VideoThumbnailer thumber;
        std::vector<uint8_t> buf;

        //                    QTime d(0, 0, 0, 0);
        //                    thumber.setSeekTime(d.toString("hh:mm:ss:ms").toStdString());
        //                    QMediaPlayer *mediaPlayer = new QMediaPlayer(this);

        //                    mediaPlayer->setMedia(QUrl::fromLocalFile(strFile));
        //                    qDebug() << QString::number(mediaPlayer->error()) << "&&&&&&&";
        //                    if (mediaPlayer->isVideoAvailable()) {
        //                        thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //                        QImage img = QImage::fromData(buf.data(), buf.size(), "png");
        //                        pix = new QPixmap(QPixmap::fromImage(/**tmpimg*/img));
        //                    }
        //                    else {
        //                        QImage *tmpimg = nullptr;
        //                        tmpimg = new QImage(":/images/123.jpg");
        //                        pix = new QPixmap(QPixmap::fromImage(*tmpimg));
        //                    }

        //                    m_bThumbnailReadOK = false;
        //                    QThread *tmpTh = QThread::create([ = ]() {
        //                        VideoThumbnailer thumber;
        //                        QTime d(0, 0, 0, 0);
        //                        thumber.setSeekTime(d.toString("hh:mm:ss:ms").toStdString());
        //                        thumber.setThumbnailSize(480 * qApp->devicePixelRatio());
        //                        thumber.setMaintainAspectRatio(true);
        //                        std::vector<uint8_t> buf;
        //                        thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //                        m_bThumbnailReadOK = true;
        //                        QThread::currentThread()->quit();
        //                    });
        //                    tmpTh->start();
        //                    QThread::msleep(50);
        //                    if (m_bThumbnailReadOK) {
        //                        thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //                        QImage img = QImage::fromData(buf.data(), buf.size(), "png");
        //                        //pix = new QPixmap(QPixmap::fromImage(img));
        //                        pix = QPixmap::fromImage(QImage(img));
        //                    }
        //                    else
        {
            QImage tmpimg = QImage(":/images/123.jpg");
            pix = QPixmap::fromImage(QImage(":/images/123.jpg"));
        }

        //                    QFile f(strFile);
        //                    bool bValid = false;
        //                    if (f.open(QIODevice::ReadOnly)) {
        //                        QDataStream ds(&f);
        //                        ds >> bValid;
        //                        if (bValid) {
        //                            thumber.generateThumbnail(fileInfo.canonicalFilePath().toUtf8().toStdString(),ThumbnailerImageType::Png, buf);
        //                            QImage img = QImage::fromData(buf.data(), buf.size(), "png");
        //                            pix = new QPixmap(QPixmap::fromImage(/**tmpimg*/img));
        //                        }
        //                        else {
        //                            QImage *tmpimg = nullptr;
        //                            tmpimg = new QImage(":/images/123.jpg");
        //                            pix = new QPixmap(QPixmap::fromImage(*tmpimg));
        //                        }
        //                    }

    } else if (fileInfo.suffix() == "jpg") {
        pix = QPixmap::fromImage(QImage(path));
    } else {
        //continue; //其他格式不管
        return;
    }
    updatePic(pix);

    QMenu *menu = new QMenu();
    QAction *actCopy = new QAction(this);
    actCopy->setText("Copy");

    QAction *actDel = new QAction(this);
    actDel->setText("Delete");
    QAction *actOpenFolder = new QAction(this);
    actOpenFolder->setText("Open folder");
    //                menu->addAction(actOpen);
    menu->addAction(actCopy);
    menu->addAction(actDel);
    menu->addAction(actOpenFolder);

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &DLabel::customContextMenuRequested, this, [=](QPoint pos) {
        Q_UNUSED(pos);
        menu->exec(QCursor::pos());
    });
    //                connect(actOpen, &QAction::triggered, this, [ = ] {
    //                    //                    QString  cmd = QString("xdg-open ") + strFile; //在linux下，可以通过system来xdg-open命令调用默认程序打开文件；
    //                    //                    system(cmd.toStdString().c_str());

    //                    if (fileInfo.suffix() == "jpg")
    //                    {
    //                        QString program = "deepin-image-viewer"; //用看图打开
    //                        QStringList arguments;
    //                        arguments << strFile;
    //                        QProcess *myProcess = new QProcess(this);
    //                        myProcess->startDetached(program, arguments);
    //                    } else
    //                    {
    //                        QString program = "deepin-movie"; //用影院打开
    //                        QStringList arguments;
    //                        arguments << strFile;
    //                        QProcess *myProcess = new QProcess(this);
    //                        myProcess->startDetached(program, arguments);
    //                    }
    //                });
    connect(actCopy, &QAction::triggered, this, [=] {
        QStringList paths;
        if (m_index.isEmpty()) {
            paths = QStringList(path);
            qDebug() << "sigle way";
        } else {
            QSet<int>::iterator it;
            for (it = m_index.begin(); it != m_index.end(); ++it) {
                paths << m_indexImage.value(*it)->getPath();
                qDebug() << m_indexImage.value(*it)->getPath();
            }
        }

        QClipboard *cb = qApp->clipboard();
        QMimeData *newMimeData = new QMimeData();
        QByteArray gnomeFormat = QByteArray("copy\n");
        QString text;
        QList<QUrl> dataUrls;
        for (QString path : paths) //待添加复制和删除功能以及快捷键效果
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

        //                    QImage img(paths.first());//img特有，视频不需要
        //                    Q_ASSERT(!img.isNull());
        //                    newMimeData->setImageData(img);

        cb->setMimeData(newMimeData, QClipboard::Clipboard);
    });
    connect(actOpenFolder, &QAction::triggered, this, [=] {
        QString strtmp = fileInfo.path();
        if (strtmp.size() && strtmp[0] == '~') {
            //奇怪，这里不能直接使用strFolder调replace函数
            strtmp.replace(0, 1, QDir::homePath());
        }
        Dtk::Widget::DDesktopServices::showFolder(strtmp);
    });
    connect(actDel, &QAction::triggered, this, [=] {
        if (m_index.isEmpty()) {
            DDesktopServices::trash(path);
        } else {
            QSet<int>::iterator it;
            for (it = m_index.begin(); it != m_index.end(); ++it) {
                DDesktopServices::trash(m_indexImage.value(*it)->getPath());
            }
        }
    });

    pix.scaled(this->size(), Qt::KeepAspectRatio);
}

ImageItem::~ImageItem()
{
    m_menu->deleteLater();
    m_actCopy->deleteLater();
    m_actDel->deleteLater();
    m_actOpenFolder->deleteLater();
}

void ImageItem::mouseDoubleClickEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    QFileInfo fileInfo(_path);
    if (fileInfo.suffix() == "jpg") {
        QString program = "deepin-image-viewer"; //用看图打开
        QStringList arguments;
        arguments << _path;
        QProcess *myProcess = new QProcess(this);
        myProcess->startDetached(program, arguments);
    } else {
        QString program = "deepin-movie"; //用影院打开
        QStringList arguments;
        arguments << _path;
        QProcess *myProcess = new QProcess(this);
        myProcess->startDetached(program, arguments);
    }
}

void ImageItem::mouseReleaseEvent(QMouseEvent *ev) //改到缩略图里边重载，然后set到indexnow，现在的方法只是重绘了这一个item
{
    if (ev->button() == Qt::LeftButton) {
        if (_index != _indexNow) {
            //ImageItem *tItem = m_indexImage.value(_indexNow);
            //tItem->update();
            _indexNow = _index;
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
        if (_index != _indexNow) {
            _indexNow = _index;
            update();
        }
    }
    if (m_bMulti && ev->button() == Qt::LeftButton) { //左键选择，右键腾出来用于选择菜单
        if (m_index.contains(_index)) {
            m_index.remove(_index);
        } else {
            m_index.insert(_index);
        }
    }
}
void ImageItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    //qDebug() << "paint" << _index;
    QPainter painter(this);

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
    QRect backgroundRect = rect();
    QRect pixmapRect;
    QFileInfo fileinfo(_path);
    QString str = fileinfo.suffix();
    if (m_index.contains(_index) || (m_index.isEmpty() && _index == _indexNow)) {
        QPainterPath backgroundBp;
        QRect reduceRect = QRect(backgroundRect.x() + 1, backgroundRect.y() + 1,
                                 backgroundRect.width() - 2, backgroundRect.height() - 2);
        backgroundBp.addRoundedRect(reduceRect, 8, 8);
        painter.setClipPath(backgroundBp);
        painter.fillRect(
            reduceRect,
            QBrush(DGuiApplicationHelper::instance()->applicationPalette().highlight().color()));

        if (_pixmap.width() > _pixmap.height()) {
            _pixmap = _pixmap.copy((_pixmap.width() - _pixmap.height()) / 2, 0, _pixmap.height(),
                                   _pixmap.height());
        } else if (_pixmap.width() < _pixmap.height()) {
            _pixmap = _pixmap.copy(0, (_pixmap.height() - _pixmap.width()) / 2, _pixmap.width(),
                                   _pixmap.width());
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
        if (!_pixmap.isNull()) {
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
        if (_pixmap.isNull()) {
            painter.setClipPath(bg);
            //            painter.drawPixmap(pixmapRect, m_pixmapstring);
            QIcon icon(m_pixmapstring);
            icon.paint(&painter, pixmapRect);
        }
        this->setFixedSize(48, 58);
    } else {
        pixmapRect.setX(backgroundRect.x() + 1);
        pixmapRect.setY(backgroundRect.y() + 0);
        pixmapRect.setWidth(backgroundRect.width() - 2);
        pixmapRect.setHeight(backgroundRect.height() - 0);

        QPainterPath bg0;
        bg0.addRoundedRect(pixmapRect, 4, 4);
        painter.setClipPath(bg0);

        if (!_pixmap.isNull()) {
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
        if (_pixmap.isNull()) {
            painter.setClipPath(bg);
            //            painter.drawPixmap(pixmapRect, m_pixmapstring);

            QIcon icon(m_pixmapstring);
            icon.paint(&painter, pixmapRect);
        }
        this->setFixedSize(30, 40);
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
    painter.drawPixmap(pixmapRect, _pixmap);

    painter.save();
    painter.setPen(
        QPen(DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color(), 1));
    painter.drawRoundedRect(pixmapRect, 4, 4);
    painter.restore();
}
