// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "datamanager.h"
#include "imageitem.h"
#include "ac-deepin-camera-define.h"
#include "capplication.h"

#include <DGuiApplicationHelper>
#include <DDesktopServices>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#endif
#include <DSysInfo>

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

DCORE_USE_NAMESPACE

QShortcutEx::QShortcutEx(const QKeySequence& key, QWidget *parent,
                         const char *member, const char *ambiguousMember,
                         Qt::ShortcutContext context):QShortcut (key, parent, member, ambiguousMember, context)
{
    setAutoRepeat(false);
}

ImageItem::ImageItem(QWidget *parent)
    : DLabel(parent),
      m_bFocus(false)
{
    m_bVideo = false;
    m_actPrint = nullptr;
    m_pAniWidget = new AnimationWidget(QPixmap(), this);

    m_pixmapstring = ":/images/camera/snapshot.png";

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
        if (m_bVideo) {
            m_menu->removeAction(m_actPrint);
        } else {
            m_menu->insertAction(m_actOpenFolder, m_actPrint);
        }
#ifndef UNITTEST
        if (!m_path.isEmpty()) {
            m_menu->exec(QCursor::pos());
        }
#endif
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

void ImageItem::updatePicPath(const QString &filePath)
{
    m_path = filePath;
    QPixmap pix;
    QFileInfo fileInfo(filePath);

    if (fileInfo.suffix() == "webm" || fileInfo.suffix() == "mp4") {
        m_bVideo = true;
        if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
            video_thumbnailer *m_video_thumbnailer = getLoadLibsInstance()->m_video_thumbnailer();
            image_data *m_image_data = getLoadLibsInstance()->m_video_thumbnailer_create_image_data();

            try {
                //thumber.generateThumbnail(m_path.toUtf8().toStdString(), ThumbnailerImageType::Png, buf);//异常视频这里老崩，给上游提交bug的出处
                getLoadLibsInstance()->m_video_thumbnailer_generate_thumbnail_to_buffer(m_video_thumbnailer, m_path.toUtf8().data(), m_image_data);
                QImage img = QImage::fromData(m_image_data->image_data_ptr, static_cast<int>(m_image_data->image_data_size), "png");
                img.scaled(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE, Qt::KeepAspectRatioByExpanding);
                pix = QPixmap::fromImage(img);
                malloc_trim(0);
            } catch (...) {
                qWarning() << "generateThumbnail failed";
            }

            getLoadLibsInstance()->m_video_thumbnailer_destroy_image_data(m_image_data);
            getLoadLibsInstance()->m_video_thumbnailer_destroy(m_video_thumbnailer);

            m_image_data = nullptr;
            m_video_thumbnailer = nullptr;
        }
    } else if (fileInfo.suffix() == "jpg") {
        m_bVideo = false;
        QImage img(filePath);
        img = img.scaled(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE, Qt::KeepAspectRatioByExpanding);
        pix = QPixmap::fromImage(img);
        malloc_trim(0);
    } else {
        pix = QPixmap();
    }
    QTimer::singleShot(500, this, [ = ]() {
        updatePic(pix);
        pix.scaled(this->size(), Qt::KeepAspectRatio);
    });
}

void ImageItem::updatePic(QPixmap pixmap)
{
    m_pAniWidget->setVisible(true);
    m_pAniWidget->setPixmap(pixmap);


    m_pAnimation = new QPropertyAnimation(m_pAniWidget, "geometry", this);
    m_pAnimation->setEasingCurve(QEasingCurve::OutQuad);
    m_pAnimation->setDuration(ANIMATION_DURATION);
    m_pAnimation->setKeyValueAt(0, QRect(width() / 2, height() / 2, 0, 0));
    m_pAnimation->setKeyValueAt(0.3, QRect((width() - 40) / 2, (height() - 40) / 2, 40, 40));
    m_pAnimation->setKeyValueAt(1, QRect(1, 1, width() - 1, height() - 1)); //去除1像素外边框
    m_pAnimation->start(/*QAbstractAnimation::DeleteWhenStopped*/);
    connect(m_pAnimation, &QPropertyAnimation::finished, [ & ]() {
        m_pAnimation->deleteLater();
        m_pAnimation = nullptr;
        m_pixmap = m_pAniWidget->getPixmap();
        update();
        m_pAniWidget->setVisible(false);
    });
}

void ImageItem::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QRect pixmapRect = rect();
    QRect foregroundRect;
    QFileInfo fileinfo(m_path);
    QString str = fileinfo.suffix();

#if QT_VERSION_MAJOR > 5
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#else
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#endif

    pixmapRect.setX(rect().x() + 1);
    pixmapRect.setY(rect().y() + 1);
    pixmapRect.setWidth(rect().width() - 2);
    pixmapRect.setHeight(rect().height() - 2);

    foregroundRect.setX(rect().x() + 2);
    foregroundRect.setY(rect().y() + 2);
    foregroundRect.setWidth(rect().width() - 4);
    foregroundRect.setHeight(rect().height() - 4);

    QColor bgColor = m_bFocus ? Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color() : QColor(0, 0, 0, 25);
    QPainterPath bg;
    bg.addRoundedRect(rect(), 26, 26);
    painter.fillPath(bg, QBrush(bgColor));

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
}

static bool mouseDblClick = false;
void ImageItem::mousePressEvent(QMouseEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        mouseDblClick = true;
        return; //不响应双击事件
    }

    mouseDblClick = false;
    DLabel::mousePressEvent(event);
}

void ImageItem::mouseMoveEvent(QMouseEvent *event)
{
    //解决bug 在缩略图中可拖动相机界面，bug 100647
    Q_UNUSED(event);
    return;
}

void ImageItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick || mouseDblClick) {
        return; //不响应双击事件
    }

    if (!rect().contains(event->pos()))
        return;

    if (event->button() == Qt::LeftButton) {
        openFile();
    }

    return;
}

void ImageItem::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}

void ImageItem::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void ImageItem::onShowMenu()
{
    QPoint centerpos(width() / 2, height() / 2);
    QPoint screen_centerpos = mapToGlobal(centerpos);
    if (m_bVideo) {
        m_menu->removeAction(m_actPrint);
    } else {
        m_menu->insertAction(m_actOpenFolder, m_actPrint);
    }
#ifndef UNITTEST
    if (!m_path.isEmpty()) {
        m_menu->exec(screen_centerpos);
    }
#endif
}

void ImageItem::onOpenFolder()
{
    if (!m_path.isEmpty() && m_path == '~') {
        //这里不能直接使用strFolder调replace函数
        m_path.replace(0, 1, QDir::homePath());
    }
    QString path = QFileInfo(m_path).absolutePath();
    Dtk::Widget::DDesktopServices::showFolder(path);
    qDebug() << "Click the right mouse or press the shortcut to open the folder";
}

void ImageItem::initShortcut()
{
    QShortcutEx *shortcutCopy = new QShortcutEx(QKeySequence("ctrl+c"), this);
    shortcutCopy->setObjectName(SHORTCUT_COPY);
    connect(shortcutCopy, SIGNAL(activated()), this, SLOT(onCopy()));
    //也可以用Qt::Key_Delete
    QShortcutEx *shortcutDel = new QShortcutEx(QKeySequence("delete"), this);
    shortcutDel->setObjectName(SHORTCUT_DELETE);
    connect(shortcutDel, SIGNAL(activated()), this, SLOT(onShortcutDel()));
    //唤起右键菜单
    QShortcutEx *shortcutMenu = new QShortcutEx(QKeySequence("Alt+M"), this);
    shortcutMenu->setObjectName(SHORTCUT_CALLMENU);
    connect(shortcutMenu, SIGNAL(activated()), this, SLOT(onShowMenu()));
    //打开文件夹
    QShortcutEx *shortcutOpenFolder = new QShortcutEx(QKeySequence("Ctrl+O"), this);
    shortcutOpenFolder->setObjectName(SHORTCUT_OPENFOLDER);
    connect(shortcutOpenFolder, &QShortcut::activated, this, &ImageItem::onOpenFolder);
    //打印
    QShortcutEx *shortcutPrint = new QShortcutEx(QKeySequence("Ctrl+P"), this);
    shortcutPrint->setObjectName(SHORTCUT_PRINT);
    connect(shortcutPrint, SIGNAL(activated()), this, SLOT(onPrint()));
}

void ImageItem::openFile()
{
    QFileInfo fileInfo(m_path);
    QString program("dde-file-manager");  // dbus调用失败，通过文管打开
    QStringList arguments;
    if (m_path.isEmpty()) {
        return;
    }

    bool ret = false;
    QProcess *myProcess = new QProcess(this);

    // check if os edition on v23 or later
    static const int kMinOsEdition = 23;
    const int osMajor = DSysInfo::majorVersion().toInt();

    if (osMajor >= kMinOsEdition) {
        //玲珑环境下无法通过QProcess方式打开应用，通过DBus打开
        if (fileInfo.suffix() == "jpg") {
            //用看图打开
            QDBusMessage message =
                QDBusMessage::createMethodCall("com.deepin.imageViewer", "/", "com.deepin.imageViewer", "openImageFile");
            message << m_path;
            QDBusMessage retMessage = QDBusConnection::sessionBus().call(message);

            if (retMessage.type() != QDBusMessage::ErrorMessage) {
                ret = true;
                qDebug() << "[dbus] Open it with deepin-image-viewer";
            } else {
                qWarning() << retMessage.errorMessage();
            }
        } else {
            //用影院打开
#if 0
            program = "ll-cli";
            arguments << "run"
                      << "org.deepin.movie"
                      << "--"
                      << "deepin-movie" << m_path;
            qDebug() << "[process] Open it with deepin-movie (ll-cli)";
            ret = myProcess->startDetached(program, arguments);
#else
            QDBusMessage message = QDBusMessage::createMethodCall("com.deepin.movie", "/", "com.deepin.movie", "openFile");
            message << m_path;
            QDBusMessage retMessage = QDBusConnection::sessionBus().call(message);

            if (retMessage.type() != QDBusMessage::ErrorMessage) {
                ret = true;
                qDebug() << "[dbus] Open it with deepin-movie";
            } else {
                qWarning() << retMessage.errorMessage();
            }
#endif
        }
    }

    // try backup way
    if (!ret) {
        if (fileInfo.suffix() == "jpg") {
            program = "deepin-image-viewer";  //用看图打开
            arguments << m_path;
            qDebug() << "[process] Open it with deepin-image-viewer";
        } else {
            program = "deepin-movie"; //用影院打开
            arguments << m_path;
            qDebug() << "[process] Open it with deepin-movie";
        }
        ret = myProcess->startDetached(program, arguments);
    }

    qInfo() << m_path;

    if (CamApp->isPanelEnvironment())
        CamApp->getMainWindow()->showMinimized();

    if (!ret) { //打开失败，调用文管选择“打开方式”窗口
        qWarning() << "QProcess startDetached error";
        arguments.clear();
        program = "dde-file-manager";
        arguments << "-o" << m_path;
        ret = myProcess->startDetached(program, arguments);
    }
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
    QStringList tempExistPaths;

    // 加载有效图片并记录存在的路径
    for (const QString &path : paths) {
        QImage img(path);
        if (!img.isNull()) {
            m_imgs << img;
            tempExistPaths << path;  // 修复原代码中错误的 paths 赋值
        }
    }

    DPrintPreviewDialog printDialog(parent);
    printDialog.setObjectName(PRINT_DIALOG);
    printDialog.setAccessibleName(PRINT_DIALOG);

    // 版本兼容处理
    const bool useNewAPI = (
#if (DTK_VERSION_MAJOR > 5 || (DTK_VERSION_MAJOR ==5 && (DTK_VERSION_MINOR > 4 || (DTK_VERSION_MINOR ==4 && DTK_VERSION_PATCH >=10))))
        DApplication::runtimeDtkVersion() >= DTK_VERSION_CHECK(5, 4, 10, 0)
#else
        false
#endif
    );

    // 设置文档名称
    if (tempExistPaths.size() == 1) {
        QFileInfo fi(tempExistPaths.first());
        printDialog.setDocName(fi.completeBaseName() + ".pdf");
    }

    // 连接打印信号
    if (useNewAPI && printDialog.setAsynPreview(m_imgs.size())) {
        connect(&printDialog, SIGNAL(paintRequested(DPrinter *, const QVector<int> &)),
                this, SLOT(paintRequestedAsyn(DPrinter *, const QVector<int> &)));
    } else {
        connect(&printDialog, SIGNAL(paintRequested(DPrinter *)),
                this, SLOT(paintRequestSync(DPrinter *)));
    }

#ifndef UNITTEST
    printDialog.exec();
#else
    printDialog.show();
#endif
}

// 公共绘图方法
void ImageItem::drawImageOnPage(DPrinter *printer, const QImage &img)
{
    QPainter painter(printer);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    const QRectF wRect = printer->pageRect(QPrinter::DevicePixel);
    
    // 计算最佳缩放比例
    const qreal widthRatio = wRect.width() / img.width();
    const qreal heightRatio = wRect.height() / img.height();
    const qreal ratio = qMin(widthRatio, heightRatio);
    
    // 计算绘制位置
    const QSizeF scaledSize = img.size() * ratio;
    const QPointF position(
        (wRect.width() - scaledSize.width()) / 2,
        (wRect.height() - scaledSize.height()) / 2
    );
    
    painter.drawImage(QRectF(position, scaledSize), img);

    painter.end();
}

void ImageItem::paintRequestedAsyn(DPrinter *printer, const QVector<int> &pageRange)
{
    qDebug() << "Async paint pages:" << pageRange.size();

    for (int i = 0; i < pageRange.size(); ++i) {
        const int page = pageRange[i];
        if (page >= 1 && page <= m_imgs.size()) {
            drawImageOnPage(printer, m_imgs.at(page-1));
            if (i != pageRange.size()-1) {
                printer->newPage();
            }
        }
    }
}

void ImageItem::paintRequestSync(DPrinter *printer)
{
    for (int i = 0; i < m_imgs.size(); ++i) {
        drawImageOnPage(printer, m_imgs.at(i));
        if (i != m_imgs.size()-1) {
            printer->newPage();
        }
    }
}

void AnimationWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QRect pixmapRect = rect();
    QPainter painter(this);
    QPainterPath path;

#if QT_VERSION_MAJOR > 5
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#else
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#endif

    path.addRoundedRect(pixmapRect, width(), height());
    painter.fillPath(path, QBrush(m_animatePix));
}

AnimationWidget::AnimationWidget(QPixmap pixmap, QWidget *parent) : m_animatePix(pixmap)
{
    this->setParent(parent);
    setMargin(0);
    setContentsMargins(0, 0, 0, 0);
    resize(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE);
}

AnimationWidget::~AnimationWidget()
{

}

ImageItem::~ImageItem()
{
    delete m_pAniWidget;
    m_pAniWidget = nullptr;

}
