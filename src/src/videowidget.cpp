// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videowidget.h"
#include "mainwindow.h"
#include "datamanager.h"
#include "ac-deepin-camera-define.h"
#include "capplication.h"
#include "photorecordbtn.h"
#include "camera.h"
#include "eventlogutils.h"
#include "config.h"
#include "globalutils.h"

#include <DBlurEffectWidget>
#include <dsysinfo.h>

#include <QPixmap>
#include <QTimer>
#include <QDateTime>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QThread>
#include <QScrollBar>
#include <QGraphicsBlurEffect>
#include <QKeyEvent>
#include <QDir>
#include <QGraphicsProxyWidget>
#include <QStandardPaths>
#include <QSvgRenderer>
#if QT_VERSION_MAJOR <= 5
#include <QDesktopWidget>
#include <QGraphicsSvgItem>
#else
#include <QtSvgWidgets/QGraphicsSvgItem>
#include <QScreen>
#endif

#define COUNTDOWN_WIDTH 32
#define COUNTDOWN_HEIGHT 61
#define COUNTDOWN_OFFECT 20

static PRIVIEW_ENUM_STATE g_Enum_Camera_State = PICTRUE;

// 重写QGraphicsView类，在子类中，直接将鼠标事件路由到QWidget，这样QGraphicsScene不会接收和处理鼠标事件
// bug链接 bug 100791
QGraphicsViewEx::QGraphicsViewEx(QWidget *parent)
    : QGraphicsView(parent)
{
    // qDebug() << "QGraphicsViewEx created";
}

void QGraphicsViewEx::mouseMoveEvent(QMouseEvent *e)
{
    // qDebug() << "QGraphicsViewEx mouseMoveEvent";
    // 画布视图鼠标事件，默认接收鼠标事件(accepted为true)，即默认不向父窗口传递鼠标事件
    // 若在鼠标事件处理完成后，需要父窗口继续处理鼠标事件，需要在函数末尾将accepted改为false
    QGraphicsView::mouseMoveEvent(e);

    //若没有选中任何图元，才执行移动相机窗口操作
    if (scene() && scene()->selectedItems().isEmpty()) {
        // qDebug() << "QGraphicsViewEx mouseMoveEvent";
        //accepted设为false，鼠标移动事件会继续向父窗口传递，拖动窗口功能可正常运行
        e->setAccepted(false);
    }
}

videowidget::videowidget(DWidget *parent)
    : DWidget(parent),
      m_imgPrcThread(nullptr),
      m_audPrcThread(nullptr),
      m_nMaxRecTime(60), //默认60小时
      m_openglwidget(nullptr),
      m_gridlinewidget(nullptr),
      m_GridType(Grid_None),
      m_videoWriter(nullptr),
      m_filterType(filter_Normal),
      m_exposure(0),
      m_videoFormat("mp4")
{
    qDebug() << "Initializing VideoWidget";
#ifndef __mips__
    if (!get_wayland_status()) {
        m_openglwidget = new PreviewOpenglWidget(this);
        m_openglwidget->setFocusPolicy(Qt::ClickFocus);
        qDebug() << "Created OpenGL widget for non-Wayland environment";
    }
#endif
    m_pNormalItem = new QGraphicsPixmapItem;
    m_pNormalItem->setZValue(0);

    m_bActive = false;   //是否录制中
#if QT_VERSION_MAJOR > 5
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);
    m_player->setSource(QUrl("qrc:/resource/Camera.wav"));
#else
    m_takePicSound = new QSound(":/resource/Camera.wav");
#endif
    m_countTimer = new QTimer(this);
    m_flashTimer = new QTimer(this);
    m_recordingTimer = new QTimer(this);
    m_pNormalView = new QGraphicsViewEx(this);
    m_gridlinewidget = new GridLineWidget(this);
    m_gridlinewidget->setGridType(Grid_None);
#if QT_VERSION_MAJOR > 5
    QScreen *screen = QGuiApplication::primaryScreen();
    //获取设备屏幕大小
    QRect screenRect = screen->geometry();
    m_flashLabel  = new DLabel();
#else
    QDesktopWidget *desktopWidget = QApplication::desktop();
    //获取设备屏幕大小
    QRect screenRect = desktopWidget->screenGeometry();
    m_flashLabel  = new DLabel(desktopWidget);
#endif
    m_flashLabel->setWindowFlags(m_flashLabel->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_flashLabel->move(0, 0);
    m_flashLabel->setFixedSize(screenRect.width(), screenRect.height());
    m_flashLabel->hide();
    m_recordingTimeWidget = new DLabel(this);
    m_recordingTime = new DLabel;
    m_dLabel = new DLabel(this);
    m_dLabel->setFixedSize(COUNTDOWN_WIDTH, COUNTDOWN_HEIGHT);
    QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
    shadow_effect->setOffset(0, 0);              //阴影的偏移量
    shadow_effect->setColor(QColor(33, 33, 33)); //阴影的颜色
    shadow_effect->setBlurRadius(45);             // 阴影圆角的大小
    m_dLabel->setGraphicsEffect(shadow_effect);

    m_pNormalScene = new QGraphicsScene();
    // 画布网格线图元
    m_pGridLineItem = new GridLineItem;
    m_pGridLineItem->setGridType(Grid_None);
    m_pGridLineItem->setZValue(1);
    // 画布svg图标图元
    m_pSvgItem = new QGraphicsSvgItem;
    m_pSvgItem->setZValue(2);
    m_pSvgItem->hide();
    // 画布错误提示图元
    m_pCamErrItem = new QGraphicsTextItem;
    m_pCamErrItem->setZValue(2);
    m_pGridLayout = new QGridLayout(this);

    DLabel *recordingRedStatus = new DLabel;//录制状态红点
    QHBoxLayout *recordingwidgetlay = new QHBoxLayout;

    m_btnClickTime = QDateTime::currentDateTime();
    m_savePicFolder = "";
    m_saveVdFolder = "";
    m_nFileID = 0;
    m_nInterval = 0;
    m_curTakePicTime = 0;
    m_nCount = 0;
    m_pNormalView->setFrameShape(QFrame::Shape::NoFrame);
    m_pNormalView->setFocusPolicy(Qt::NoFocus);
    forbidScrollBar(m_pNormalView);
    m_pNormalView->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
    m_pNormalView->setScene(m_pNormalScene);
    m_pGridLayout->setContentsMargins(0, 0, 0, 0);
    m_pGridLayout->addWidget(m_pNormalView);

    m_pNormalScene->addItem(m_pNormalItem);
    m_pNormalScene->addItem(m_pSvgItem);
    m_pNormalScene->addItem(m_pCamErrItem);
    m_pNormalScene->addItem(m_pGridLineItem);

    recordingwidgetlay->setSpacing(0);
    recordingwidgetlay->setContentsMargins(0, 0, 0, 0);
    recordingwidgetlay->addWidget(recordingRedStatus, 0, Qt::AlignBottom);
    recordingwidgetlay->addWidget(m_recordingTime, 0, Qt::AlignCenter);
    recordingRedStatus->setPixmap(QPixmap(":/images/icons/light/circular.svg"));
    recordingRedStatus->setFixedSize(QSize(22, 22));
    m_recordingTime->setFixedSize(93, 26);
    m_recordingTime->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QGraphicsDropShadowEffect *shadow_effect1 = new QGraphicsDropShadowEffect(this);
    shadow_effect1->setOffset(0, 0);              //阴影的偏移量
    shadow_effect1->setColor(QColor(33, 33, 33)); //阴影的颜色
    shadow_effect1->setBlurRadius(15);             // 阴影圆角的大小
    m_recordingTime->setGraphicsEffect(shadow_effect1);
    m_recordingTimeWidget->setLayout(recordingwidgetlay);
    m_recordingTimeWidget->hide(); //先隐藏
    m_recordingTimeWidget->setFixedSize(115, 26);
    m_dLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_dLabel->setFocusPolicy(Qt::NoFocus);
    m_dLabel->setAlignment(Qt::AlignCenter);

    m_pSvgItem->setCacheMode(QGraphicsItem::NoCache);

    //wayland平台设置背景色为黑色
    if (get_wayland_status() == true) {
        qDebug() << "wayland platform";
        QPalette pal(palette());
        pal.setColor(QPalette::Window, Qt::black);
        setAutoFillBackground(true);
        setPalette(pal);
    }

    QFont ftLabel("SourceHanSansSC-ExtraLight");
    ftLabel.setWeight(QFont::Normal);
    ftLabel.setPointSize(40);
    m_dLabel->setFont(ftLabel);
    QPalette pltLabel = m_dLabel->palette();

    pltLabel.setColor(QPalette::WindowText, QColor("#ffffff"));
    QColor clrFill(25, 25, 25);
    pltLabel.setColor(QPalette::Base, clrFill);

    QColor clrShadow(0, 0, 0);
    clrShadow.setAlphaF(0.2);
    pltLabel.setColor(QPalette::Shadow, clrShadow);
    m_dLabel->setPalette(pltLabel);
    QPalette pa_cb = m_recordingTime->palette();//不用槽函数，程序打开如果是深色主题，可以正常切换颜色，其他主题不行，DTK的bug？

    pa_cb.setColor(QPalette::Window, QColor(255, 0, 0, 30));
    pa_cb.setColor(QPalette::WindowText, QColor(255, 255, 255));
    QFont ft("SourceHanSansSC-Medium");
    ft.setWeight(QFont::Medium);
    ft.setPixelSize(18);
    m_recordingTime->setPalette(pa_cb);
    m_recordingTime->setFont(ft);
    m_recordingTime->setText(QString("00:00:00"));

    connect(m_countTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));//显示倒计时
    connect(m_flashTimer, SIGNAL(timeout()), this, SLOT(flash()));
    connect(m_recordingTimer, SIGNAL(timeout()), this, SLOT(showRecTime()));//显示录制时长
    m_flashTimer->setSingleShot(true);

    //设置相机背景色为灰色
    QColor bgColor(25, 25, 25);
    QPalette pal(this->palette());
    pal.setColor(QPalette::Window, bgColor);
    this->setAutoFillBackground(true);
    this->setPalette(pal);
    m_pNormalScene->setBackgroundBrush(bgColor);

    // 默认不显示网格线
    setGridType(Grid_None);

    if (DataManager::instance()->encodeEnv() != FFmpeg_Env || !DataManager::instance()->encExists() || GlobalUtils::isLowPerformanceBoard()) {
        qDebug() << "Unsupported environment or encoding error";
        m_videoFormat = "webm";
    }
    if (dc::Settings::get().getOption("outsetting.outformat.vidformat").toInt()) {
        qDebug() << "Video format setting detected";
        if (!GlobalUtils::isLowPerformanceBoard())
            m_videoFormat = "webm";
        else
            m_videoFormat = "mp4";
    }
    qDebug() << "VideoWidget initialization complete";
}

//延迟加载
void videowidget::delayInit()
{
    qDebug() << "Starting delayed initialization of VideoWidget";
    m_imgPrcThread = new MajorImageProcessingThread;
    m_imgPrcThread->setParent(this);
    m_imgPrcThread->setObjectName("MajorThread");

    m_imgPrcThread->setHorizontalMirror(dc::Settings::get().getOption("photosetting.mirrorMode.mirrorMode").toBool());
    setFlash(dc::Settings::get().getOption("photosetting.Flashlight.Flashlight").toBool());
    setCapStatus(false);
    m_imgPrcThread->m_bTake = false;
    if (get_wayland_status() == true) {
        qDebug() << "Wayland detected";
        connect(m_imgPrcThread, SIGNAL(SendMajorImageProcessing(QImage *, int)),
                this, SLOT(ReceiveMajorImage(QImage *, int)));
    } else {
        qDebug() << "X11 detected";
        connect(m_imgPrcThread, SIGNAL(SendMajorImageProcessing(QImage *, int)),
                this, SLOT(ReceiveMajorImage(QImage *, int)));
        connect(m_imgPrcThread, SIGNAL(sigRenderYuv(bool)), this, SLOT(ReceiveOpenGLstatus(bool)));
        connect(m_imgPrcThread, SIGNAL(sigYUVFrame(uchar *, uint, uint)),
                m_openglwidget, SLOT(slotShowYuv(uchar *, uint, uint)));
    }

    connect(m_imgPrcThread, SIGNAL(reachMaxDelayedFrames()),
            this, SLOT(onReachMaxDelayedFrames()));
    connect(m_imgPrcThread, SIGNAL(SendFilterImageProcessing(QImage *)),
            this, SIGNAL(updateFilterImage(QImage *)));
    connect(m_imgPrcThread, SIGNAL(sigReflushSnapshotLabel()),
            this, SIGNAL(reflushSnapshotLabel()));

    connect(m_imgPrcThread, &MajorImageProcessingThread::sigReflushSnapshotLabel, [=](const QString &filePath){
        emit reflushSnapshotLabel(filePath);
    });

    connect(m_imgPrcThread, SIGNAL(sigRecordFrame(uchar*, uint)),
            this, SLOT(onRecordFrame(uchar*, uint)));

    m_audPrcThread = new AudioProcessingThread;
    m_audPrcThread->setParent(this);
    m_audPrcThread->setObjectName("AudioThread");
    connect(m_audPrcThread, SIGNAL(sendAudioProcessing(uchar*, uint)), this, SLOT(onRecordAudio(uchar*, uint)));

    QPalette pltFlashLabel = m_flashLabel->palette();
    pltFlashLabel.setColor(QPalette::Window, QColor(Qt::white));
    m_flashLabel->setPalette(pltFlashLabel);
    m_flashLabel->hide();

    QString device = dc::Settings::get().getBackOption("device").toString();
    //启动视频
    switchCamera(device.toStdString().c_str(), "");

    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged,
                     this, &videowidget::onThemeTypeChanged);
    qDebug() << "Delayed initialization complete";
}

void videowidget::showNocam()
{
    qDebug() << "Showing no camera state";
    if (!m_pNormalView->isVisible())
        m_pNormalView->show();

    if (!m_pCamErrItem->isVisible())
        m_pCamErrItem->show();

    if (!m_pSvgItem->isVisible())
        m_pSvgItem->show();

    if (m_pNormalItem)
        m_pNormalItem->hide();
    if (m_openglwidget)
        m_openglwidget->hide();

    if (m_GridType != Grid_None) {
        qDebug() << "Grid detected";
        if (!m_pGridLineItem->isVisible())
            m_pGridLineItem->show();
        if (m_gridlinewidget->isVisible())
            m_gridlinewidget->hide();
    }

    emit sigDeviceChange();

    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType()) {
        qDebug() << "Light theme detected";
        m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/light/Not connected.svg"), this));
        QColor clr(255, 255, 255);
        clr.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clr);
    } else {
        qDebug() << "Dark theme detected";
        m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/dark/Not connected_dark.svg"), this));
        QColor clr(Qt::white);
        clr.setAlphaF(0.2);
        m_pCamErrItem->setDefaultTextColor(clr);
    }

    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::DemiBold);
    ft.setPixelSize(17);
    m_pCamErrItem->setFont(ft);
    QString str(tr("No webcam found"));//未连接摄像头
    m_pCamErrItem->setPlainText(str);
    m_pNormalScene->setSceneRect(m_pSvgItem->boundingRect());

    m_flashLabel->hide();

    itemPosChange();
    m_pCamErrItem->show();
    m_pSvgItem->show();

    emit noCam();
    qDebug() << "No camera state displayed";

    if (getCapStatus())  //录制完成处理
        onEndBtnClicked();
    qDebug() << "End button clicked";
}

void videowidget::showCamUsed()
{
    qDebug() << "Showing camera in use state";

    if (!m_pNormalView->isVisible())
        m_pNormalView->show();

    if (m_pNormalItem)
        m_pNormalItem->hide();
    if (m_openglwidget)
        m_openglwidget->hide();

    if (m_GridType != Grid_None) {
        qDebug() << "Grid type is not Grid_None";
        if (!m_pGridLineItem->isVisible())
            m_pGridLineItem->show();
        if (m_gridlinewidget->isVisible())
            m_gridlinewidget->hide();
    }

    emit sigDeviceChange();
    QString str(tr("The webcam is in use"));//摄像头已被占用
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType()) {
        qDebug() << "Light theme";
        m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/light/Take up.svg"), this));
        QColor clrText(Qt::white);
        clrText.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clrText);//浅色主题文字和图片是白色，特殊处理
        m_pCamErrItem->setPlainText(str);
    } else {
        qDebug() << "Dark theme";
        m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/dark/Take up_dark.svg"), this));
        QColor clrText(Qt::white);
        clrText.setAlphaF(0.2);
        m_pCamErrItem->setDefaultTextColor(clrText);
        m_pCamErrItem->setPlainText(str);
    }

    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::DemiBold);
    ft.setPixelSize(17);
    m_pCamErrItem->setFont(ft);
    m_pNormalScene->setSceneRect(m_pSvgItem->boundingRect());

    m_flashLabel->hide();

    itemPosChange();
    m_pCamErrItem->show();
    m_pSvgItem->show();
    emit noCamAvailable();
    qDebug() << "Camera in use state displayed";

    if (getCapStatus())//录制完成处理
        onEndBtnClicked();
}
#ifndef __mips__
void videowidget::ReceiveOpenGLstatus(bool result)
{
    qDebug() << "OpenGL status received";
    if (result && m_openglwidget) {
        qDebug() << "OpenGL status received";
        //Success
        if (m_pCamErrItem->isVisible())
            m_pCamErrItem->hide();

        if (m_pSvgItem->isVisible())
            m_pSvgItem->hide();

        if (m_pNormalView->isVisible())
            m_pNormalView->hide();

        if (m_GridType != Grid_None) {
            qDebug() << "Grid status received";
            if (m_pGridLineItem->isVisible())
                m_pGridLineItem->hide();

            if (!m_gridlinewidget->isVisible())
                m_gridlinewidget->show();
        }

        m_pNormalScene->update();

        if (get_encoder_status() == 0 && getCapStatus() && DataManager::instance()->encodeEnv() == FFmpeg_Env)
            onEndBtnClicked();

        // 画布窗口等比例缩放画面
        int framewidth = m_openglwidget->getFrameWidth();
        int frameheight = m_openglwidget->getFrameHeight();
        int widgetwidth = width();
        int widgetheight = height();
        if (!framewidth && !frameheight) {
            qDebug() << "Frame size not available";
            m_openglwidget->resize(widgetwidth, widgetheight);
        } else {
            qDebug() << "Frame size available";
            if ((framewidth * 100 / frameheight) > (widgetwidth * 100 / widgetheight)) {
                qDebug() << "Frame width is larger than widget width";
                m_openglwidget->resize(widgetwidth, widgetwidth * frameheight / framewidth);
                m_openglwidget->move(0, (widgetheight - widgetwidth * frameheight / framewidth) / 2);
            } else {
                qDebug() << "Frame height is larger than widget height";
                m_openglwidget->resize(widgetheight * framewidth / frameheight, widgetheight);
                m_openglwidget->move((widgetwidth - widgetheight * framewidth / frameheight) / 2, 0);
            }
        }

        if (!m_openglwidget->isVisible() && !m_isFlash)
            m_openglwidget->show();

        malloc_trim(0);
        emit camAvailable();
    }
    qDebug() << "OpenGL status received";
}

#endif

void videowidget::ReceiveMajorImage(QImage *image, int result)
{
    qDebug() << "Received major image";
    // 若窗口高度改变，需要刷新整个窗口，防止上一帧图像出现在其它区域
    static int height = this->height();
    if (height != this->height()) {
        qDebug() << "Window height changed";
        update();
        height = this->height();
    }

    if (!image->isNull()) {
        qDebug() << "Image is not null";
        switch (result) {
        case 0:     //Success
            if (!m_isFlash)
                m_pNormalView->show();
            m_pCamErrItem->hide();
            m_pSvgItem->hide();
            m_pNormalItem->show();

            if (m_GridType != Grid_None) {
                if (!m_pGridLineItem->isVisible())
                    m_pGridLineItem->show();
                if (m_gridlinewidget->isVisible())
                    m_gridlinewidget->hide();
            }

            if (m_openglwidget && m_openglwidget->isVisible())
                m_openglwidget->hide();
            {
                int widgetwidth = this->width();
                int widgetheight = this->height();
                if ((image->width() * 100 / image->height()) > (widgetwidth * 100 / widgetheight)) {
                    QImage img = image->scaled(widgetwidth, widgetwidth * image->height() / image->width(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    m_framePixmap = QPixmap::fromImage(img);
                } else {
                    QImage img = image->scaled(widgetheight * image->width() / image->height(), widgetheight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    m_framePixmap = QPixmap::fromImage(img);
                }

                m_pNormalScene->setSceneRect(m_framePixmap.rect());
                m_pNormalItem->setPixmap(m_framePixmap);

                if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
                    if (get_encoder_status() == 0 && getCapStatus() == true)
                        onEndBtnClicked();
                } else if (DataManager::instance()->encodeEnv() == QCamera_Env) {
                    if (Camera::instance()->getRecoderState() == 0 && getCapStatus() == true && !Camera::instance()->isReadyRecord())
                        onEndBtnClicked();
                }

                malloc_trim(0);
            }
            break;
        default:
            qWarning() << "Received invalid image with result code:" << result;
            break;
        }
        emit camAvailable();
    }
    qDebug() << "Received major image";
}

void videowidget::onReachMaxDelayedFrames()
{
    qWarning() << "Reached maximum delayed frames, stopping camera";
    check_device_list_events(get_v4l2_device_handler());
    v4l2_dev_t *videoDevHand =  get_v4l2_device_handler();

    if (m_imgPrcThread != nullptr)
        m_imgPrcThread->stop();

    while (m_imgPrcThread->isRunning());//等待编码线程结束

    if (videoDevHand != nullptr)
        close_v4l2_device_handler();

    DataManager::instance()->setdevStatus(NOCAM);
    stopEverything();
    showNocam();
    emit updatePhotoState(CMainWindow::photoNormal);

    m_pNormalItem->hide() ;
    if (m_openglwidget)
        m_openglwidget->hide();
    qDebug() << "Camera stopped due to maximum delayed frames";
}

void videowidget::showCountDownLabel(PRIVIEW_ENUM_STATE state)
{
    // qDebug() << "Entering showCountDownLabel";
    QString str;
    switch (state) {
    case PICTRUE:
        // qDebug() << "Showing countdown label for photo";
        m_recordingTimeWidget->hide();
        break;

    case VIDEO:
        // qDebug() << "Showing countdown label for video";
        m_pCamErrItem->hide();
        m_pSvgItem->hide();

        if (!get_capture_pause()) {//判断是否是暂停状态
            // qDebug() << "capture is not pause";
            QString strTime = "";
            int nHour = m_nCount / 3600;

            if (nHour == 0) {
                strTime.append("00");
            } else if (nHour < 10) {
                strTime.append("0");
                strTime.append(QString::number(nHour));
            } else
                strTime.append(QString::number(nHour));

            strTime.append(":");
            int nOutHour = m_nCount % 3600;
            int nMins = nOutHour / 60;

            if (nMins == 0) {
                strTime.append("00");
            } else if (nMins < 10) {
                strTime.append("0");
                strTime.append(QString::number(nMins));
            } else
                strTime.append(QString::number(nMins));

            strTime.append(":");
            int nSecs = nOutHour % 60;

            if (nSecs == 0) {
                strTime.append("00");
            } else if (nSecs < 10) {
                strTime.append("0");
                strTime.append(QString::number(nSecs));
            } else
                strTime.append(QString::number(nSecs));

            m_recordingTime->setText(strTime);

//            if ((m_nCount >= 3)) {
//                //开启多线程，每100ms读取时间并显示
                m_countTimer->stop();
                m_recordingTimer->start(200);
//                return;
//            }

//            m_nCount++;
        }
        break;

    default:
        // qDebug() << "Showing countdown label for unknown state";
        m_pCamErrItem->hide();
        m_pSvgItem->hide();
        m_recordingTimeWidget->hide();
        break;
    }
    // qDebug() << "Exiting showCountDownLabel";
}

void videowidget::resizeEvent(QResizeEvent *size)
{
    // qDebug() << "Entering resizeEvent";
    Q_UNUSED(size);

    //计时窗口放大缩小的显示
    m_recordingTimeWidget->move((width() - m_recordingTimeWidget->width() - 10) / 2,
                                height() - m_recordingTimeWidget->height() - 15);

    m_dLabel->move((width() - m_dLabel->width()) / 2,
                   (height() - m_dLabel->height()) - COUNTDOWN_OFFECT);


    if (m_pCamErrItem->isVisible()) {
        // qDebug() << "Showing camera error item";
        itemPosChange();
        m_pCamErrItem->show();
        m_pSvgItem->show();
    } else {
        // qDebug() << "Hiding camera error item";
        itemPosChange();
        m_pCamErrItem->hide();
    }

    if (m_gridlinewidget)
        m_gridlinewidget->resize(rect().size());

    if(m_openglwidget && m_openglwidget->isVisible()) {
        ReceiveOpenGLstatus(true);
    }
    // qDebug() << "Exiting resizeEvent";
}

void videowidget::showCountdown()
{
    qDebug() << "Entering showCountdown";
    if (DataManager::instance()->getdevStatus() == NOCAM) {
        qDebug() << "No camera available, stopping countdown";
        if (m_flashTimer->isActive())
            m_flashTimer->stop();

        if (m_countTimer->isActive())
            m_countTimer->stop();

        m_dLabel->hide();
        m_flashLabel->hide();
    }

    //显示倒数，m_nMaxInterval秒后结束，并拍照
    if (m_nInterval == 0 && DataManager::instance()->getdevStatus() == CAM_CANUSE) {
        //倒数到最后一次，隐藏计数器
        qDebug() << "Countdown complete, current state:" << (g_Enum_Camera_State == VIDEO ? "Video" : "Photo");
        m_dLabel->hide();
        if (g_Enum_Camera_State == VIDEO) {
            qDebug() << "Showing countdown label for video";
            if (!getCapStatus()) {
                if (DataManager::instance()->encodeEnv() != QCamera_Env) {
                    /*m_bActive录制状态判断
                    *false：非录制状态
                    *true：录制状态
                    */
                    startTakeVideo();
                } else
                    startCaptureVideo();
            }

            showCountDownLabel(g_Enum_Camera_State);
            //向mainwindow 发送录像状态为正在录像
            emit updateRecordState(photoRecordBtn::Recording);
        }

        if (g_Enum_Camera_State == PICTRUE) {
            qDebug() << "Showing countdown label for photo";
            if (m_nInterval == 0 && m_curTakePicTime >= 0) {
                if (m_flashEnable && 1 == m_nMaxContinuous) {
#if QT_VERSION_MAJOR > 5
                    QScreen *screen = QGuiApplication::primaryScreen();
                    QRect rt = screen->geometry();
#else
                    int index = QApplication::desktop()->screenNumber(this);
                    QRect rt = QApplication::desktop()->screenGeometry(index);
#endif
                    m_flashLabel->setGeometry(rt);
                    m_flashLabel->setFixedSize(rt.size());
                    m_flashLabel->show();
                }
                m_flashTimer->start(500);
                qDebug() << "flashTimer->start();";

                if (m_pNormalView) {
                    m_isFlash = true;
                    m_pNormalView->hide();
                }
                if (m_openglwidget)
                    m_openglwidget->hide();
                if (m_gridlinewidget)
                    m_gridlinewidget->hide();
            }

            //发送就结束信号处理按钮状态
            m_countTimer->stop();
            emit updatePhotoState(CMainWindow::Photoing);

            if (QDir(m_savePicFolder).exists() == false) {
                qDebug() << "Creating default picture folder";
                QString strDefaultPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator() + "Camera";
                if (QDir(strDefaultPath).exists() == false) {
                    qDebug() << "Creating default picture folder";
                    QDir dir;
                    dir.mkdir(strDefaultPath);
                }

                m_savePicFolder = strDefaultPath;
            }

            QString strFileName = getSaveFilePrefix() + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".jpg";
            emit filename(strFileName);
            m_imgPrcThread->m_strPath = m_savePicFolder + QDir::separator() + strFileName;
            m_imgPrcThread->m_bTake = true; //保存图片标志
            m_nFileID++;
            m_curTakePicTime -= 1;

            if (m_curTakePicTime == 0) {
                qDebug() << "Taking picture";
                //拍照结束，恢复按钮状态和缩略图标志位
                QThread *thread = QThread::create([ = ]() {
                    int nCount = 0;

                    while (true) {
                        if (nCount > 50) {
                            qWarning() << "too long to emit takePicDone";
                            break;
                        }

                        if (!m_imgPrcThread->m_bTake)
                            break;

                        nCount ++;
                        QThread::msleep(100);
                    }
                    emit takePicDone();
                    QThread::currentThread()->quit();
                    emit updateBlockSystem(false);//连拍结束，取消阻止关机
                });
                connect(thread, &QThread::finished, thread, &QObject::deleteLater);
                thread->start();
            } else {
                qDebug() << "Taking picture once";
                QThread *thread = QThread::create([ = ]() {
                    int nCount = 0;

                    while (true) {
                        if (nCount > 50) {
                            qWarning() << "too long to emit takePicOnce";
                            break;
                        }

                        if (!m_imgPrcThread->m_bTake)
                            break;
                        nCount ++;
                        QThread::msleep(100);
                    }

                    emit takePicOnce();
                    QThread::currentThread()->quit();
                });
                connect(thread, &QThread::finished, thread, &QObject::deleteLater);
                thread->start();

            }

            if (m_curTakePicTime > 0 && DataManager::instance()->getdevStatus() == CAM_CANUSE)
                m_countTimer->start(m_nMaxInterval == 34 ? 0 : 1000);

            m_pCamErrItem->hide();
            m_pSvgItem->hide();
            m_pNormalView->hide();
        }

    } else {
        qDebug() << "Showing countdown label for photo";
        m_dLabel->setText(QString::number(m_nInterval));
        m_dLabel->show();
        showCountDownLabel(PICTRUE); //拍照录像都要显示倒计时
        m_nInterval--;
        qInfo() << "m_nInterval:" << m_nInterval;
    }
}

void videowidget::showRecTime()
{
    qDebug() << "Updating recording time display";
    //获取写video的时间
    if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
        /** On the equipment with poor performance, the coding time is long,
         * which will cause 1 to 2S katto after exceeding 3S. Get recording duration.
         *
         * his change may lead to the short and shorter time of the video duration ratio,
         * but this is an abnormal situation, but it should not belong to the bug.
         *
         * Reserved the relevant code first to prevent other unknown problems from emerging.
         *
         * powered by xxxxpf
         */
//        m_nCount = static_cast<int>(get_video_time_capture());
        m_nCount = m_imgPrcThread->getRecCount();

        //过滤不正常的时间
//        if (m_nCount <= 3) {
//            qWarning() << "error time" << m_nCount;
//            return;
//        }
    } else if (DataManager::instance()->encodeEnv() == QCamera_Env) {
        qDebug() << "Getting recording time from camera";
        m_nCount = Camera::instance()->getRecoderTime() / 1000;
        m_nCount += 2;
    } else if (DataManager::instance()->encodeEnv() == GStreamer_Env) {
        qDebug() << "Getting recording time from gstreamer";
        m_nCount = m_videoWriter->getRecrodTime();
    }

    if (m_nCount >= m_nMaxRecTime * 60 * 60)//最大录制时长，平板与主线保持一致
        onEndBtnClicked();//结束录制

    QString strTime = "";
    //计算小时数
    int nHour = m_nCount / 3600;

    if (nHour == 0) {
        strTime.append("00");
    } else if (nHour < 10) {
        strTime.append("0");
        strTime.append(QString::number(nHour));
    } else
        strTime.append(QString::number(nHour));

    strTime.append(":");
    //计算分钟数
    int nOutHour = m_nCount % 3600;
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
    //计算秒钟数
    int nSecs = nOutHour % 60;

    if (nSecs == 0) {
        strTime.append("00");
    } else if (nSecs < 10) {
        strTime.append("0");
        strTime.append(QString::number(nSecs));
    } else {
        strTime.append(QString::number(nSecs));
    }

    m_recordingTime->setText(strTime);
    qDebug() << "Recording time updated:" << strTime;
}

void videowidget::flash()
{
    qDebug() << "Entering flash";
    if (get_sound_of_takeing_photo()) {
#if QT_VERSION_MAJOR > 5
        m_player->play();
#else
        m_takePicSound->play();
#endif
    }

#ifndef __mips__
    if (!get_wayland_status()) {
        if (!m_openglwidget->isVisible() && filter_Normal == m_filterType && 0 == m_exposure)
            m_openglwidget->show();
    } else {
        m_pNormalView->show();
    }
#endif
    m_isFlash = false;
    m_flashLabel->hide(); //为避免没有关闭，放到定时器里边关闭

    if (m_curTakePicTime == 0)
        stopEverything();

    //最后一次拍照完成，设置状态恢复
    if (0 == m_curTakePicTime) {
        qDebug() << "Setting photo state to normal";
        emit updatePhotoState(CMainWindow::photoNormal);
    }
    qDebug() << "Exiting flash";
}

void videowidget::slotresolutionchanged(const QString &resolution)
{
    qDebug() << "Entering slotresolutionchanged";
    if (DataManager::instance()->getdevStatus() != CAM_CANUSE)
        return ;

    QStringList ResStr = resolution.split("x");
    //解决传人参数不正确时崩溃问题 by wuzhigang 2021-06-17
    if (2 > ResStr.size()) {
        return;
    }

    int newwidth = ResStr[0].toInt();//新的宽度
    int newheight = ResStr[1].toInt();//新的高度

    if (DataManager::instance()->encodeEnv() == QCamera_Env) {
        qDebug() << "Getting camera resolution";
        QSize resolution = Camera::instance()->getCameraResolution();
        if (resolution.width() != newwidth || resolution.height() != newheight) {
            QSize size(newwidth, newheight);
            Camera::instance()->setCameraResolution(size);
        }

    } else {
        qDebug() << "Getting v4l2 resolution";
        int nWidth = v4l2core_get_frame_width(get_v4l2_device_handler());
        int nHeight = v4l2core_get_frame_height(get_v4l2_device_handler());

        if (newwidth != nWidth || newheight != nHeight) {
            //设置刷新率
            v4l2core_define_fps(get_v4l2_device_handler(), 1, 30);
            //设置新的分辨率
            v4l2core_prepare_new_resolution(get_v4l2_device_handler(), newwidth, newheight);
            request_format_update(1);
        }
    }
    qDebug() << "Exiting slotresolutionchanged";
}

void videowidget::slotGridTypeChanged(int type)
{
    // qDebug() << "Entering slotGridTypeChanged";
    setGridType(static_cast<GridType>(type));
}

void videowidget::slotVideoFormatChanged(const QString &format)
{
    // qDebug() << "Entering slotVideoFormatChanged";
    m_videoFormat = format;
}

void videowidget::onRecordFrame(uchar *rgb, uint size)
{
    // qDebug() << "Entering onRecordFrame";
    if (m_videoWriter)
        m_videoWriter->writeFrame(rgb, size);
}

void videowidget::onRecordAudio(uchar *data, uint size)
{
    // qDebug() << "Entering onRecordAudio";
    if (m_videoWriter)
        m_videoWriter->writeAudio(data, size);
}

void videowidget::onEndBtnClicked()
{
    qDebug() << "End button clicked, stopping recording";
    if (m_countTimer->isActive())
        m_countTimer->stop();

    if (m_recordingTimer->isActive())
        m_recordingTimer->stop();

    if (m_pCamErrItem->isVisible() && (m_imgPrcThread->getStatus() == 0) && DataManager::instance()->encodeEnv() != QCamera_Env)
        m_pCamErrItem->hide();

    if (m_pSvgItem->isVisible() && (m_imgPrcThread->getStatus() == 0) && DataManager::instance()->encodeEnv() != QCamera_Env)
        m_pSvgItem->hide();
    m_dLabel->hide();

    //取消拍照或者停止拍照，发送按钮状态普通
    emit updateRecordState(photoRecordBtn::Normal);
    //结束录制阶段,获取焦点的窗口索引，焦点在结束按钮，设置焦点索引为拍照/录制按钮
    if (DataManager::instance()->getNowTabIndex() != DataManager::instance()->m_tabIndex) {

        DataManager::instance()->setNowTabIndex(DataManager::instance()->m_tabIndex);
        if (DataManager::instance()->getNowTabIndex() == 9)
            DataManager::instance()->setNowTabIndex(8);
        else {
            uint tabindex = DataManager::instance()->getNowTabIndex();
            DataManager::instance()->setNowTabIndex(tabindex);
        }
    }

    m_recordingTimeWidget->hide();
#ifdef __sw_64__
    photoRecordBtn* recordBtn = nullptr;
    CMainWindow* mainwindow = qobject_cast<CMainWindow*>(parentWidget());
    if (mainwindow) {
        recordBtn = mainwindow->findChild<photoRecordBtn *>(BUTTON_PICTURE_VIDEO);
    }
    if (recordBtn)
        recordBtn->blockSignals(true);
#endif

#if QT_VERSION_MAJOR > 5
    QElapsedTimer time;
#else
    QTime time;
#endif
    time.start();
    int duration = 0;
    if (getCapStatus()) { //录制完成处理
        qDebug() << "stop takeVideo";

        if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
            if (video_capture_get_save_video() == 1) {
                duration = round(get_video_time_capture());
                set_video_time_capture(0);
                stop_encoder_thread();
            }
        } else if (DataManager::instance()->encodeEnv() == QCamera_Env) {
            if (Camera::instance()->isRecording()) {
                duration = Camera::instance()->getRecoderTime();
                Camera::instance()->stopRecoder();
            }
        } else if (DataManager::instance()->encodeEnv() == GStreamer_Env) {
            duration = m_nCount;
            if (m_audPrcThread != nullptr)
                m_audPrcThread->stop();
            while (m_audPrcThread->isRunning());//等待音频线程结束

            if (m_videoWriter)
                m_videoWriter->stop();
        }

        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
        emit takeVdDone();
    }

    qInfo() << QString("-------------------stop record cost[%1]ms..----------------\n").arg(time.elapsed());

    QJsonObject obj {
        {"tid", EventLogUtils::EndRecording},
        {"version", VERSION},
        {"duration", duration}
    };
    EventLogUtils::get().writeLogs(obj);

#ifdef __sw_64__
    // 过滤停止录像编码期间的键鼠点击事件
    qApp->processEvents();
    if (recordBtn)
        recordBtn->blockSignals(false);
#endif
    qDebug() << "Recording stopped, duration:" << duration << "ms";
}

void videowidget::onRestartDevices()
{
    qDebug() << "Entering onRestartDevices";
    if (DataManager::instance()->getdevStatus() != CAM_CANUSE) {
        qDebug() << "Device status is not CAM_CANUSE";
        onChangeDev();

        if (DataManager::instance()->getdevStatus() == CAM_CANUSE) {
            qDebug() << "Device status is changed to CAM_CANUSE";
            emit sigDeviceChange();
            QPalette plt = palette();
            plt.setColor(QPalette::Window, Qt::white);
#ifndef __mips__
            if (!get_wayland_status())
                m_openglwidget->show();
#endif
        }

    }
    qDebug() << "Exiting onRestartDevices";
}

void videowidget::onChangeDev()
{
    qDebug() << "Changing camera device";
    v4l2_dev_t *devicehandler =  get_v4l2_device_handler();

    if (m_imgPrcThread != nullptr)
        m_imgPrcThread->stop();

    while (m_imgPrcThread->isRunning());
    QString str;

    if (devicehandler != nullptr) {
        qDebug() << "Closing v4l2 device handler";
        str = QString(devicehandler->videodevice);
        close_v4l2_device_handler();
    }

    v4l2_device_list_t *devlist = get_device_list();
    if (devlist->num_devices == 2) {
        qDebug() << "device list has 2 devices";
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (str != str1) {
                if (E_OK == switchCamera(devlist->list_devices[i].device, devlist->list_devices[i].name)) {
                    break;
                }
            }
        }
    } else {
        qDebug() << "device list has not 2 devices";
        if (devlist->num_devices == 0) {
            DataManager::instance()->setdevStatus(NOCAM);
            showNocam();
        }

        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);

            if (str == str1) {
                if (i == devlist->num_devices - 1) {
                    switchCamera(devlist->list_devices[0].device, devlist->list_devices[0].name);
                    break;
                } else {
                    switchCamera(devlist->list_devices[i + 1].device, devlist->list_devices[i + 1].name);
                    break;
                }
            }

            if (str.isEmpty()) {
                switchCamera(devlist->list_devices[0].device, devlist->list_devices[0].name);
                break;
            }
        }
    }
    qDebug() << "Camera device change complete";
}

int videowidget::switchCamera(const char *device, const char *devName)
{
    qDebug() << "Switching camera to device:" << (device ? device : "null");
    if (NULL == device) {
        return -1;
    }
    int ret = -1;
    if (DataManager::instance()->encodeEnv() != QCamera_Env) {
        qDebug() << "encodeEnv is not QCamera_Env";
        ret = camInit(device);
    } else {
        qDebug() << "encodeEnv is QCamera_Env";
        ret = 0;
    }
    if (ret == E_OK) {
        qDebug() << "ret is E_OK";
        m_imgPrcThread->init();
        m_imgPrcThread->start();
        DataManager::instance()->setdevStatus(CAM_CANUSE);
        //切换摄像机成功，发送设备名称信号到主界面。
        if (devName && strlen(devName)) {
            emit switchCameraSuccess(devName);
        }
        dc::Settings::get().setBackOption("device", device);
        qDebug() << "Camera switch successful to device:" << (devName ? devName : "unknown");
    } else if (ret == E_FORMAT_ERR) {
        qDebug() << "ret is E_FORMAT_ERR";
        v4l2_dev_t *vd =  get_v4l2_device_handler();

        if (vd != nullptr)
            close_v4l2_device_handler();

        qWarning() << "camInit failed";

        if (DataManager::instance()->getdevStatus() != CAM_CANNOT_USE)
            showCamUsed();

        DataManager::instance()->setdevStatus(CAM_CANNOT_USE);
        qWarning() << "Camera switch failed - Format error";
    } else {
        qDebug() << "ret is not E_OK or E_FORMAT_ERR";
        v4l2_dev_t *vd =  get_v4l2_device_handler();

        if (vd != nullptr)
            close_v4l2_device_handler();

        DataManager::instance()->setdevStatus(NOCAM);
        showNocam();
        qWarning() << "Camera switch failed - Unknown error";
    }
    qDebug() << "Exiting switchCamera, ret: " << ret;
    return ret;
}

QString videowidget::getSaveFilePrefix()
{
    qDebug() << "Entering getSaveFilePrefix";
    QString filePrefix = "Camera_";
    if (DSysInfo::deepinType() == DSysInfo::DeepinProfessional) {
        qDebug() << "DeepinProfessional";
        filePrefix = "UOS_";
    } else if (DSysInfo::deepinType() == DSysInfo::DeepinDesktop) {
        qDebug() << "DeepinDesktop";
        filePrefix = "DEEPIN_";
    }
    qDebug() << "Exiting getSaveFilePrefix";
    return filePrefix;
}

QString videowidget::getSaveVdFolder() const
{
    return m_saveVdFolder;
}

void videowidget::onTakePic(bool bTrue)
{
    qDebug() << "Take photo triggered:" << bTrue;
    g_Enum_Camera_State = PICTRUE;

    if (bTrue) {
        qDebug() << "Taking photo";
        //1、重置状态
        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_pCamErrItem->isVisible() && (m_imgPrcThread->getStatus() == 0))
            m_pCamErrItem->hide();

        if (m_pSvgItem->isVisible() && (m_imgPrcThread->getStatus() == 0))
            m_pSvgItem->hide();

        m_nInterval = m_nMaxInterval = m_Maxinterval;
        m_curTakePicTime = m_nMaxContinuous;
        emit updateBlockSystem(true);//连拍时开启关机阻止
        showCountdown();  //直接执行拍照动作
        if (0 != m_nMaxInterval) {
            //连拍，启动timer
            emit updatePhotoState(CMainWindow::prePhoto);
            m_countTimer->start(1000);
        }
    } else {
        qDebug() << "Taking photo";
        emit updatePhotoState(CMainWindow::photoNormal);
        emit takePicCancel();
        emit updateBlockSystem(false);//连拍取消时，取消阻止关机
        m_nInterval = 0; //下次可开启
        m_curTakePicTime = 0; //结束当前拍照

        if (m_countTimer->isActive())
            m_countTimer->stop();


        m_dLabel->hide();
        m_flashLabel->hide();

        QEventLoop eventloop;
        QTimer::singleShot(300, &eventloop, SLOT(quit()));
        eventloop.exec();
#ifndef __mips__

        if (!get_wayland_status())
            if (!m_openglwidget->isVisible())
                m_openglwidget->show();

#endif
    }
    qDebug() << "Photo operation complete";
}

void videowidget::onTakeVideo() //点一次开，再点一次关
{
    qDebug() << "Entering onTakeVideo";
    if (m_nInterval >= 0 && m_countTimer->isActive()) { //倒计时期间的处理
        qDebug() << "m_nInterval >= 0 && m_countTimer->isActive()";
        //向mainwindow 发送录像状态通知信号
        emit updateRecordState(photoRecordBtn::Normal);
        m_nInterval = 0; //下次可开启
        set_video_time_capture(0);
        emit takeVdCancel(); //用于恢复缩略图
        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_pCamErrItem->isVisible())
            m_pCamErrItem->hide();

        if (m_pSvgItem->isVisible())
            m_pSvgItem->hide();

        return; //return即可，这个是外部过来的信号，外部有处理相关按钮状态、恢复缩略图状态
    }

    if (getCapStatus()) { //录制完成处理
        qDebug() << "stop takeVideo";
        //向mainwindow 发送录像状态通知信号
        emit updateRecordState(photoRecordBtn::Normal);
        if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
            stop_encoder_thread();
        } else if (DataManager::instance()->encodeEnv() == QCamera_Env) {
            Camera::instance()->stopRecoder();
        }
        else if (DataManager::instance()->encodeEnv() == GStreamer_Env) {
            if (m_audPrcThread != nullptr)
                m_audPrcThread->stop();
            while (m_audPrcThread->isRunning());//等待音频线程结束

            if (m_videoWriter)
                m_videoWriter->stop();
        }
        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_pCamErrItem->isVisible())
            m_pCamErrItem->hide();

        if (m_pSvgItem->isVisible())
            m_pSvgItem->hide();
        return;
    }

    g_Enum_Camera_State = VIDEO;

    m_nInterval = m_Maxinterval;
    showCountdown();
    if (0 != m_Maxinterval) {
        m_countTimer->start(1000);
        //向mainwindow 发送录像倒计时状态
        emit updateRecordState(photoRecordBtn::preRecord);
    }
    qDebug() << "Video operation complete";
}

void videowidget::forbidScrollBar(QGraphicsView *view)
{
    qDebug() << "Entering forbidScrollBar";
    view->horizontalScrollBar()->blockSignals(true);
    view->verticalScrollBar()->blockSignals(true);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void videowidget::startTakeVideo()
{
    qDebug() << "Starting video capture";
#ifdef __sw_64__
    if (video_capture_get_save_video() == 1) {
        qInfo() << "-------------------------takeVideo reclicked..----------------------";
        return;
    }
#endif

    QDir dir;

    QString strDefaultVdPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + "Camera";
    if (QDir(strDefaultVdPath).exists() == false)
        dir.mkdir(strDefaultVdPath);

    if (getCapStatus()) {
        qDebug() << "stop takeVideo";
        if (DataManager::instance()->encodeEnv() == FFmpeg_Env)
            stop_encoder_thread();
        else if (DataManager::instance()->encodeEnv() == GStreamer_Env) {
            if (m_audPrcThread != nullptr)
                m_audPrcThread->stop();
            while (m_audPrcThread->isRunning());//等待音频线程结束

            if (m_videoWriter)
                m_videoWriter->stop();
        }

        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
    } else {
        qDebug() << "GetCapStatus is false";
        if (DataManager::instance()->getdevStatus() == CAM_CANUSE) {
            qDebug() << "start takeVideo";
            DataManager::instance()->getstrFileName() = getSaveFilePrefix() + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_"
                    + QString::number(m_nFileID) + "." + m_videoFormat;
            emit filename(DataManager::instance()->getstrFileName());
            m_nFileID ++;

            if (QDir(m_saveVdFolder).exists() == false) {
                m_saveVdFolder = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + "Camera";
            }

            if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
                set_video_path(m_saveVdFolder.toStdString().c_str());
                set_video_name(DataManager::instance()->getstrFileName().toStdString().c_str());
                start_encoder_thread();
            } else if (DataManager::instance()->encodeEnv() == GStreamer_Env) {
                QString recordFileName = m_saveVdFolder + "/" + DataManager::instance()->getstrFileName();
                if (!m_videoWriter)
                    m_videoWriter = new GstVideoWriter(recordFileName);
                else
                    m_videoWriter->setVideoPath(recordFileName);

                m_videoWriter->start();
                m_audPrcThread->init();
                m_audPrcThread->start();
            }
            emit updateBlockSystem(true);
            setCapStatus(true);
            m_countTimer->stop();
            m_countTimer->start(1000); //重新计时，否则时间与显示时间
        } else {
            reset_video_timer();//重置底层定时器
            m_countTimer->stop();
            setCapStatus(false);
        }

        m_nCount = 0;//5184000;
        m_recordingTime->setText(QString("00:00:00"));
        int nWidth = width();
        int nHeight = height();

        //判断倒计时阶段焦点位移到拍照/录制按钮，重新设置tab的索引
        if (DataManager::instance()->m_tabIndex == 8)
            DataManager::instance()->setNowTabIndex(8);

        m_recordingTimeWidget->show();
        m_recordingTimeWidget->move((nWidth - m_recordingTimeWidget->width() - 10) / 2,
                                    nHeight - m_recordingTimeWidget->height() - 15);
    }
    qDebug() << "Video capture started with format:" << m_videoFormat;
}

void videowidget::startCaptureVideo()
{
    qDebug() << "Starting video capture with QCamera";
    QDir dir;

    QString strDefaultVdPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + "Camera";
    if (QDir(strDefaultVdPath).exists() == false)
        dir.mkdir(strDefaultVdPath);

    if (getCapStatus()) {
        qDebug() << "stop takeVideo";
        Camera::instance()->stopRecoder();
        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
    } else {
        if (DataManager::instance()->getdevStatus() == CAM_CANUSE) {
            qDebug() << "start Gstreamer takeVideo";
            DataManager::instance()->getstrFileName() = getSaveFilePrefix() + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID);
            emit filename(DataManager::instance()->getstrFileName());
            m_nFileID ++;

            if (QDir(m_saveVdFolder).exists() == false) {
                m_saveVdFolder = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + "Camera";
            }

            QString outPutPath = QString(m_saveVdFolder + QDir::separator() + DataManager::instance()->getstrFileName());
            Camera::instance()->setVideoOutPutPath(outPutPath);
            Camera::instance()->startRecoder();
            emit updateBlockSystem(true);
            setCapStatus(true);
            m_countTimer->stop();
            m_countTimer->start(1000); //重新计时，否则时间与显示时间
        } else {
            reset_video_timer();//重置底层定时器
            m_countTimer->stop();
            setCapStatus(false);
        }

        m_nCount = 0;//5184000;
        m_recordingTime->setText(QString("00:00:00"));
        int nWidth = width();
        int nHeight = height();

        //判断倒计时阶段焦点位移到拍照/录制按钮，重新设置tab的索引
        if (DataManager::instance()->m_tabIndex == 8)
            DataManager::instance()->setNowTabIndex(8);

        m_recordingTimeWidget->show();
        m_recordingTimeWidget->move((nWidth - m_recordingTimeWidget->width() - 10) / 2,
                                    nHeight - m_recordingTimeWidget->height() - 15);
    }
    qDebug() << "QCamera video capture started";
}

void videowidget::itemPosChange()
{
    qDebug() << "Entering itemPosChange";
    m_pCamErrItem->setPos((m_pSvgItem->boundingRect().width() - static_cast<int>(m_pCamErrItem->boundingRect().width())) / 2, m_pSvgItem->boundingRect().height());

    m_pSvgItem->update();
    m_pCamErrItem->update();
}

void videowidget::stopEverything()
{
    qDebug() << "Entering stopEverything";
    if (m_countTimer->isActive())
        m_countTimer->stop();

    m_flashLabel->hide();


    if (m_pCamErrItem->isVisible())
        m_pCamErrItem->hide();

    if (m_pSvgItem->isVisible())
        m_pSvgItem->hide();


    m_dLabel->hide();

#ifndef __mips__
    if (!get_wayland_status())
        if (!m_openglwidget->isVisible())
            m_openglwidget->show();
#endif
    qDebug() << "Exiting stopEverything";
}

void videowidget::onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType)
{
    qDebug() << "Entering onThemeTypeChanged";
    if (m_pSvgItem->isVisible()) {
        qDebug() << "m_pSvgItem is visible";
        QString str;
        if (themeType == DGuiApplicationHelper::LightType) {
            qDebug() << "themeType is LightType";
            if (DataManager::instance()->getdevStatus() == NOCAM) {
                qDebug() << "DataManager::instance()->getdevStatus() == NOCAM";
                m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/light/Not connected.svg"), this));
                str = tr("No webcam found");//未连接摄像头
            } else {//仅CAM_CANNOT_USE
                m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/light/Take up.svg"), this));
                str = tr("The webcam is in use");//摄像头已被占用
            }

            QColor clrText(Qt::white);
            clrText.setAlphaF(0.8);
            m_pCamErrItem->setDefaultTextColor(clrText);
            m_pCamErrItem->setPlainText(str);
        } else if (themeType == DGuiApplicationHelper::DarkType) {
            qDebug() << "themeType is DarkType";
            if (DataManager::instance()->getdevStatus() == NOCAM) {
                qDebug() << "changed theme 3";
                m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/dark/Not connected_dark.svg"), this));
                str = tr("No webcam found");//未连接摄像头
            } else {//仅CAM_CANNOT_USE
                qDebug() << "changed theme 4";
                m_pSvgItem->setSharedRenderer(new QSvgRenderer(QString(":/images/icons/dark/Take up_dark.svg"), this));
                str = tr("The webcam is in use");//摄像头已被占用
            }

            QColor clor(Qt::white);
            clor.setAlphaF(0.2);
            m_pCamErrItem->setDefaultTextColor(clor);//浅色主题文字和图片是白色，特殊处理
            m_pCamErrItem->setPlainText(str);
        }

        QFont ft("SourceHanSansSC");
        ft.setWeight(QFont::DemiBold);
        ft.setPixelSize(17);
        m_pCamErrItem->setFont(ft);
        if (DataManager::instance()->getdevStatus() == NOCAM) {
            qDebug() << "DataManager::instance()->getdevStatus() == NOCAM";
            m_pNormalScene->setSceneRect(m_pSvgItem->boundingRect());
            itemPosChange();
        } else {
            qDebug() << "DataManager::instance()->getdevStatus() != NOCAM";
            m_pNormalScene->setSceneRect(m_pSvgItem->boundingRect());
            itemPosChange();
        }

        if (m_flashLabel->isVisible())
            m_flashLabel->hide();
    }
    qDebug() << "Exiting onThemeTypeChanged";
}

void videowidget::setHorizontalMirror(bool bMirror)
{
    qDebug() << "Entering setHorizontalMirror";
    if (nullptr != m_imgPrcThread) {
        qDebug() << "m_imgPrcThread is not nullptr";
        m_imgPrcThread->setHorizontalMirror(bMirror);
    }
    qDebug() << "Exiting setHorizontalMirror";
}

void videowidget::setCapStatus(bool status)
{
    qDebug() << "Entering setCapStatus";
    m_bActive = status;
    if (DataManager::instance()->encodeEnv() == GStreamer_Env)
        m_imgPrcThread->setRecording(status);
}

void videowidget::setFlash(bool bFlashOn)
{
    // qDebug() << "Entering setFlash";
    m_flashEnable = bFlashOn;
}

void videowidget::setMaxRecTime(int hour)
{
    // qDebug() << "Entering setMaxRecTime";
    m_nMaxRecTime = hour;
}

void videowidget::setFilterType(efilterType type)
{
    qDebug() << "Entering setFilterType";
    m_filterType = type;
    if (m_imgPrcThread)
        m_imgPrcThread->setFilter(filterPreviewButton::filterName_CUBE(type));
}

void videowidget::setState(bool bPhoto)
{
    qDebug() << "Entering setState";
    m_bPhoto = bPhoto;
    if (m_imgPrcThread)
        m_imgPrcThread->setState(bPhoto);
}

QRect videowidget::getFrameRect()
{
    qDebug() << "Entering getFrameRect";
    QRect rect;
    if (m_openglwidget && m_openglwidget->isVisible()) {
        qDebug() << "m_openglwidget is visible";
        rect = m_openglwidget->geometry();
    } else if (m_pNormalView && m_pNormalView->isVisible()) {
        qDebug() << "m_pNormalView is visible";
        if (m_pNormalItem && m_pNormalItem->isVisible())
            rect = QRect(m_pNormalView->mapFromScene(0,0), m_pNormalView->mapFromScene(m_pNormalView->sceneRect().bottomRight()));
    }

    return rect;
}

void videowidget::setGridType(GridType type)
{
    qDebug() << "Setting grid type:" << type;
    if (type == m_GridType)
        return;

    m_GridType = type;

    m_pGridLineItem->setGridType(type);
    m_gridlinewidget->setGridType(type);

    if (type == Grid_None) {
        m_pGridLineItem->hide();
        m_gridlinewidget->hide();
    } else {
        if (m_openglwidget && m_openglwidget->isVisible()) {
            m_gridlinewidget->show();
            m_pGridLineItem->hide();
        } else {
            m_gridlinewidget->hide();
            m_pGridLineItem->show();
        }
    }
    qDebug() << "Grid type updated";
}

void videowidget::onExposureChanged(int exposure)
{
    qDebug() << "Exposure changed to:" << exposure;
    m_exposure = exposure;
    if (m_imgPrcThread)
        m_imgPrcThread->setExposure(exposure);
}

void videowidget::onFilterDisplayChanged(int bDisplay)
{
    qDebug() << "Filter display changed:" << bDisplay;
    if (m_imgPrcThread)
        m_imgPrcThread->setFilterGroupState(bDisplay);
}

videowidget::~videowidget()
{
    qDebug() << "Cleaning up VideoWidget";
    m_imgPrcThread->stop();
    m_imgPrcThread->wait();
    delete m_imgPrcThread;

    m_audPrcThread->stop();
    m_audPrcThread->wait();
    delete m_audPrcThread;
#ifndef __mips__
    if (!get_wayland_status()) {
        if (m_openglwidget) {
            delete m_openglwidget;
            m_openglwidget = nullptr;
        }
    }
#endif

    delete m_flashLabel;
    m_flashLabel = nullptr;

    delete m_pNormalView;
    m_pNormalView = nullptr;

    delete m_pNormalScene;
    m_pNormalScene = nullptr;

#if QT_VERSION_MAJOR > 5
    delete m_player;
    m_player = nullptr;

    delete m_audioOutput;
    m_audioOutput = nullptr;
#else
    delete m_takePicSound;
    m_takePicSound = nullptr;
#endif

    delete m_pGridLayout;
    m_pGridLayout = nullptr;

    delete m_dLabel;
    m_dLabel = nullptr;

    delete m_recordingTimeWidget;
    m_recordingTimeWidget = nullptr;

    delete m_countTimer;
    m_countTimer = nullptr;

    delete m_flashTimer;
    m_flashTimer = nullptr;

    delete m_recordingTimer;
    m_recordingTimer = nullptr;

    delete m_videoWriter;
    m_videoWriter = nullptr;

    camUnInit();
    qDebug() << "VideoWidget cleanup complete";
}
