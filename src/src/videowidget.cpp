/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
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

#include "videowidget.h"
#include "mainwindow.h"
#include "datamanager.h"
#include "ac-deepin-camera-define.h"
#include "capplication.h"

#include <DGuiApplicationHelper>
#include <DApplicationHelper>

#include <QPixmap>
#include <QTimer>
#include <QGraphicsView>
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

static PRIVIEW_ENUM_STATE g_Enum_Camera_State = PICTRUE;

videowidget::videowidget(DWidget *parent)
    : DWidget(parent)
{
#ifndef __mips__
    m_openglwidget = new PreviewOpenglWidget(this);
#endif
    m_bActive = false;   //是否录制中
    m_takePicSound = new QSound(":/resource/Camera.wav");
    m_countTimer = new QTimer(this);
    m_flashTimer = new QTimer(this);
    m_recordingTimer = new QTimer(this);
    m_pNormalView = new QGraphicsView(this);
    m_flashLabel  = new DLabel(this);
    m_btnVdTime = new DPushButton(this);
    m_fWgtCountdown = new DFloatingWidget(this);
    m_dLabel = new DLabel(m_fWgtCountdown);
    m_endBtn = new DPushButton(this);
    m_pNormalScene = new QGraphicsScene();
    m_pNormalItem = new QGraphicsPixmapItem;
    m_pCamErrItem = new QGraphicsTextItem;
    m_pGridLayout = new QGridLayout(this);

#ifndef __mips__
    m_openglwidget->setFocusPolicy(Qt::ClickFocus);
#endif
    m_btnClickTime = QDateTime::currentDateTime();
    m_savePicFolder = "";
    m_saveVdFolder = "";
    m_nFileID = 0;
    m_nInterval = 0;
    m_curTakePicTime = 0;
    m_nCount = 0;
    if (parentWidget()) {
        m_flashLabel->resize(parentWidget()->size());
        m_flashLabel->move(mapToGlobal(QPoint(0, 0)));
    }
    m_pNormalView->setFrameShape(QFrame::Shape::NoFrame);
    m_pNormalView->setFocusPolicy(Qt::NoFocus);
    forbidScrollBar(m_pNormalView);
    m_pNormalView->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);
    m_pNormalView->setScene(m_pNormalScene);
    m_pNormalView->setAttribute(Qt::WA_TranslucentBackground);
    m_pGridLayout->setContentsMargins(0, 0, 0, 0);
    m_pGridLayout->addWidget(m_pNormalView);
    m_pNormalScene->addItem(m_pNormalItem);
    m_pNormalScene->addItem(m_pCamErrItem);
    m_flashLabel->setFocusPolicy(Qt::NoFocus);
    m_fWgtCountdown->hide(); //先隐藏
    m_fWgtCountdown->setFixedSize(160, 144);
    m_fWgtCountdown->setBlurBackgroundEnabled(true);
    m_fWgtCountdown->setFocusPolicy(Qt::NoFocus);
    m_btnVdTime->setIcon(QIcon(":/images/icons/light/Timer Status.svg"));
    m_btnVdTime->setIconSize(QSize(6, 6));
    m_btnVdTime->hide(); //先隐藏
    m_btnVdTime->setFixedSize(92, 36);
    m_btnVdTime->setAttribute(Qt::WA_TranslucentBackground);
    m_btnVdTime->setEnabled(false);
    m_dLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_dLabel->setFocusPolicy(Qt::NoFocus);
    m_dLabel->setAlignment(Qt::AlignCenter);

    //wayland平台设置背景色为黑色
    if (get_wayland_status() == true) {
        QPalette pal(palette());
        pal.setColor(QPalette::Background, Qt::black);
        setAutoFillBackground(true);
        setPalette(pal);
    }

    QFont ftLabel("SourceHanSansSC-ExtraLight");
    ftLabel.setWeight(QFont::Thin);//最小就是50,更小的设置不进去，也是bug？
    ftLabel.setPixelSize(60);
    m_dLabel->setFont(ftLabel);
    QPalette pltLabel = m_dLabel->palette();

    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType()) {
        pltLabel.setColor(QPalette::WindowText, QColor("#000000"));
        QColor clrFill(235, 235, 235);
        clrFill.setAlphaF(0.3);
        pltLabel.setColor(QPalette::Base, clrFill);
    } else {
        pltLabel.setColor(QPalette::WindowText, QColor("#ffffff"));
        QColor clrFill(25, 25, 25);
        clrFill.setAlphaF(0.8);
        pltLabel.setColor(QPalette::Base, clrFill);
    }

    QColor clrShadow(0, 0, 0);
    clrShadow.setAlphaF(0.2);
    pltLabel.setColor(QPalette::Shadow, clrShadow);
    m_dLabel->setPalette(pltLabel);
    DPalette pa_cb = DApplicationHelper::instance()->palette(m_btnVdTime);//不用槽函数，程序打开如果是深色主题，可以正常切换颜色，其他主题不行，DTK的bug？

    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType())
        pa_cb.setColor(QPalette::ButtonText, QColor(255, 44, 44));
    else
        pa_cb.setColor(QPalette::ButtonText, QColor(202, 0, 0));

    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::Normal);
    ft.setPixelSize(14);
    m_btnVdTime->setPalette(pa_cb);
    m_btnVdTime->setFont(ft);
    m_btnVdTime->setText(QString("00:00:00"));
    m_endBtn->setObjectName(BUTTON_TAKE_VIDEO_END);
    m_endBtn->setAccessibleName(BUTTON_TAKE_VIDEO_END);
    m_endBtn->setFlat(true);
    m_endBtn->setFixedSize(QSize(40, 40));
    m_endBtn->setFocusPolicy(Qt::TabFocus);
    m_endBtn->setIconSize(QSize(35, 35));
    m_endBtn->setIcon(QIcon(":/images/icons/light/Stop Recording.svg"));
    m_endBtn->setToolTip(tr("Stop recording"));
    m_endBtn->setToolTipDuration(500); //0.5s消失
    m_endBtn->setWindowFlags(Qt::FramelessWindowHint);
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(10);                  // 阴影圆角的大小
    effect->setColor(QColor(202, 0, 0, 100));       //阴影的颜色
    effect->setOffset(0, 2);                    //阴影的偏移量
    m_endBtn->setGraphicsEffect(effect);        //给那个控件设置阴影，这里需要注意的是所有此控件的子控件，也都继承这个阴影。
    m_endBtn->hide();

    connect(m_countTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));//默认
    connect(m_flashTimer, SIGNAL(timeout()), this, SLOT(flash()));//
    connect(m_recordingTimer, SIGNAL(timeout()), this, SLOT(showRecTime()));//默认
    connect(m_endBtn, SIGNAL(clicked()), this, SLOT(onEndBtnClicked()));
}

videowidget::~videowidget()
{
    m_imgPrcThread->stop();
    m_imgPrcThread->wait();
    delete m_imgPrcThread;

#ifndef __mips__

    if (m_openglwidget) {
        delete m_openglwidget;
        m_openglwidget = nullptr;
    }

#endif

    delete m_flashLabel;
    m_flashLabel = nullptr;

    delete m_takePicSound;
    m_takePicSound = nullptr;

    delete m_pNormalView;
    m_pNormalView = nullptr;

    delete m_pNormalScene;
    m_pNormalScene = nullptr;

    delete m_takePicSound;
    m_takePicSound = nullptr;

    delete m_pGridLayout;
    m_pGridLayout = nullptr;

    delete m_fWgtCountdown;
    m_fWgtCountdown = nullptr;

    delete m_btnVdTime;
    m_btnVdTime = nullptr;

    delete m_endBtn;
    m_endBtn = nullptr;

    delete m_countTimer;
    m_countTimer = nullptr;

    delete m_flashTimer;
    m_flashTimer = nullptr;

    delete m_recordingTimer;
    m_recordingTimer = nullptr;

    delete m_thumbnail;
    m_thumbnail = nullptr;

    delete m_endBtn;
    m_endBtn = nullptr;
}

//延迟加载
void videowidget::delayInit()
{
    m_imgPrcThread = new MajorImageProcessingThread;
    m_imgPrcThread->setParent(this);
    m_imgPrcThread->setObjectName("MajorThread");
    setCapStatus(false);
    m_imgPrcThread->m_bTake = false;
#ifdef __mips__
    connect(m_imgPrcThread, SIGNAL(SendMajorImageProcessing(QImage *, int)),
            this, SLOT(ReceiveMajorImage(QImage *, int)));
#else
    connect(m_imgPrcThread, SIGNAL(sigRenderYuv(bool)), this, SLOT(ReceiveOpenGLstatus(bool)));
    connect(m_imgPrcThread, SIGNAL(sigYUVFrame(uchar *, uint, uint)),
            m_openglwidget, SLOT(slotShowYuv(uchar *, uint, uint)));
#endif
    connect(m_imgPrcThread, SIGNAL(reachMaxDelayedFrames()),
            this, SLOT(onReachMaxDelayedFrames()));

    QPalette pltFlashLabel = m_flashLabel->palette();
    pltFlashLabel.setColor(QPalette::Window, QColor(Qt::white));
    m_flashLabel->setPalette(pltFlashLabel);
    m_flashLabel->hide();
    //启动视频
    int ret =  camInit("");

    if (ret == E_OK) {
        m_pCamErrItem->hide();;
        m_imgPrcThread->start();
        DataManager::instance()->setdevStatus(CAM_CANUSE);
    } else if (ret == E_FORMAT_ERR) {
        //启动失败
        v4l2_dev_t *vd = get_v4l2_device_handler();

        //如果不为空，则关闭vd
        if (vd != nullptr) {
            close_v4l2_device_handler();
            vd = nullptr;
        }

        if (DataManager::instance()->getdevStatus() != CAM_CANNOT_USE)
            DataManager::instance()->setdevStatus(CAM_CANNOT_USE);

        showCamUsed();
        qDebug() << "cam in use" << endl;
    } else if (ret == E_NO_DEVICE_ERR) {
        if (DataManager::instance()->getdevStatus() != NOCAM) {
            DataManager::instance()->setdevStatus(NOCAM);
            //启动失败
            v4l2_dev_t *vd = get_v4l2_device_handler();

            //如果不为空，则关闭vd
            if (vd != nullptr) {
                close_v4l2_device_handler();
                vd = nullptr;
            }

        }

        showNocam();
        qWarning() << "No webcam found" << endl;
    }
    QObject::connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
    [ = ](DGuiApplicationHelper::ColorType type) {
        QPalette pltLabel = m_dLabel->palette();

        if (DGuiApplicationHelper::LightType == type) {
            pltLabel.setColor(QPalette::WindowText, QColor("#000000"));
            QColor clrFill(235, 235, 235);
            clrFill.setAlphaF(0.3);
            pltLabel.setColor(QPalette::Base, clrFill);
        } else {
            pltLabel.setColor(QPalette::WindowText, QColor("#ffffff"));
            QColor clrFill(25, 25, 25);
            clrFill.setAlphaF(0.8);
            pltLabel.setColor(QPalette::Base, clrFill);
        }

        m_dLabel->setPalette(pltLabel);

        if (m_btnVdTime->isVisible()) {
            if (type == DGuiApplicationHelper::LightType) {
                DPalette pa_cb = m_btnVdTime->palette();
                pa_cb.setColor(QPalette::ButtonText, QColor(255, 44, 44));
                m_btnVdTime->setPalette(pa_cb);
            } else {
                DPalette pa_cb = m_btnVdTime->palette();
                pa_cb.setColor(QPalette::ButtonText, QColor(202, 0, 0));
                m_btnVdTime->setPalette(pa_cb);
            }

        }

        if (m_pCamErrItem->isVisible()) {
            QString str;
            if (type == DGuiApplicationHelper::LightType) {
                if (DataManager::instance()->getdevStatus() == NOCAM) {
                    qDebug() << "changed theme 1";
                    QImage img(":/images/icons/light/Not connected.svg");
                    m_pixmap = QPixmap::fromImage(img);
                    str = tr("No webcam found");//未连接摄像头
                } else {//仅CAM_CANNOT_USE
                    qDebug() << "changed theme 2";
                    QImage img(":/images/icons/light/Take up.svg");
                    m_pixmap = QPixmap::fromImage(img);
                    str = tr("The webcam is in use");//摄像头已被占用
                }

                QColor clrText(Qt::white);
                clrText.setAlphaF(0.8);
                m_pCamErrItem->setDefaultTextColor(clrText);
                m_pCamErrItem->setPlainText(str);
                QColor clrBase(Qt::black);
                clrBase.setAlphaF(0.7);
                QPalette plt = palette();
                plt.setColor(QPalette::Base, clrBase);
                setPalette(plt);
            } else if (type == DGuiApplicationHelper::DarkType) {
                if (DataManager::instance()->getdevStatus() == NOCAM) {
                    qDebug() << "changed theme 3";
                    QImage img(":/images/icons/dark/Not connected_dark.svg");
                    m_pixmap = QPixmap::fromImage(img);
                    str = tr("No webcam found");//未连接摄像头
                } else {//仅CAM_CANNOT_USE
                    qDebug() << "changed theme 4";
                    QImage img(":/images/icons/dark/Take up_dark.svg");
                    m_pixmap = QPixmap::fromImage(img);
                    str = tr("The webcam is in use");//摄像头已被占用
                }

                QColor clor(0, 0, 0);
                clor.setAlphaF(0.8);
                m_pCamErrItem->setDefaultTextColor(clor);//浅色主题文字和图片是白色，特殊处理
                m_pCamErrItem->setPlainText(str);
                QColor clrBase(255, 255, 255);
                clrBase.setAlphaF(0.7);
                QPalette plt = palette();
                plt.setColor(QPalette::Base, clrBase);
                setPalette(plt);
            }

            QFont ft("SourceHanSansSC");
            ft.setWeight(QFont::DemiBold);
            ft.setPixelSize(17);
            m_pCamErrItem->setFont(ft);
            m_pNormalScene->setSceneRect(m_pixmap.rect());
            itemPosChange();
            m_pNormalItem->setPixmap(m_pixmap);

            if (m_flashLabel->isVisible())
                m_flashLabel->hide();

        }

    });

}

void videowidget::showNocam()
{
    if (!m_pNormalView->isVisible()) {
        m_pNormalView->show();
    }

    if (!m_pNormalItem->isVisible()) {
        m_pNormalItem->show();
    }

    if (!m_pCamErrItem->isVisible()) {
        m_pCamErrItem->show();
    }

#ifndef __mips__
    m_openglwidget->hide();
#endif

    emit sigDeviceChange();

    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType()) {
        QImage imgNocam = QImage(":/images/icons/light/Not connected.svg");
        m_pixmap = QPixmap::fromImage(imgNocam);
        QColor clr(255, 255, 255);
        clr.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clr);

        QColor clrBase(0, 0, 0);
        clrBase.setAlphaF(0.7);
        QPalette plt = palette();
        plt.setColor(QPalette::Base, clrBase);
        setPalette(plt);
    } else {
        QImage imgNocam = QImage(":/images/icons/dark/Not connected_dark.svg");
        m_pixmap = QPixmap::fromImage(imgNocam);
        QColor clr(0, 0, 0);
        clr.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clr);

        QColor clrBase(255, 255, 255);
        clrBase.setAlphaF(0.7);
        QPalette plt = palette();
        plt.setColor(QPalette::Base, clrBase);
        setPalette(plt);
    }

    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::DemiBold);
    ft.setPixelSize(17);
    m_pCamErrItem->setFont(ft);
    QString str(tr("No webcam found"));//未连接摄像头
    m_pCamErrItem->setPlainText(str);
    m_pNormalScene->setSceneRect(m_pixmap.rect());
    m_pNormalItem->setPixmap(m_pixmap);

    if (m_flashLabel->isVisible())
        m_flashLabel->hide();

    itemPosChange();
    m_pCamErrItem->show();
    emit noCam();

    if (getCapStatus())  //录制完成处理
        onEndBtnClicked();
}

void videowidget::setThumbnail(ThumbnailsBar *thumb)
{
    if (thumb != nullptr) {
        m_thumbnail = thumb;
    }
}

void videowidget::showCamUsed()
{
    qDebug() << "show camUsed" << endl;

    if (!m_pNormalView->isVisible())
        m_pNormalView->show();

    if (!m_pNormalItem->isVisible())
        m_pNormalItem->show();

    if (!m_pCamErrItem->isVisible())
        m_pCamErrItem->show();

#ifndef __mips__
    m_openglwidget->hide();
#endif
    emit sigDeviceChange();
    QString str(tr("The webcam is in use"));//摄像头已被占用

    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType()) {
        QImage imgcamused = QImage(":/images/icons/light/Take up.svg");
        m_pixmap = QPixmap::fromImage(imgcamused);
        QColor clrText(Qt::white);
        clrText.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clrText);//浅色主题文字和图片是白色，特殊处理
        m_pCamErrItem->setPlainText(str);
        QPalette plt = palette();
        QColor clrBackGRD(Qt::black);
        clrBackGRD.setAlphaF(0.7);
        plt.setColor(QPalette::Base, clrBackGRD);
        setPalette(plt);
    } else {
        QImage imgcamused = QImage(":/images/icons/dark/Take up_dark.svg");
        m_pixmap = QPixmap::fromImage(imgcamused);
        QColor clrText(Qt::black);
        clrText.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clrText);
        m_pCamErrItem->setPlainText(str);
        QPalette plt = palette();
        QColor clrBackGRD(Qt::white);
        clrBackGRD.setAlphaF(0.7);
        plt.setColor(QPalette::Base, clrBackGRD);
        setPalette(plt);
    }

    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::DemiBold);
    ft.setPixelSize(17);
    m_pCamErrItem->setFont(ft);
    m_pNormalScene->setSceneRect(m_pixmap.rect());
    m_pNormalItem->setPixmap(m_pixmap);

    if (m_flashLabel->isVisible())
        m_flashLabel->hide();

    itemPosChange();
    m_pCamErrItem->show();
    emit noCamAvailable();

    if (getCapStatus())//录制完成处理
        onEndBtnClicked();

}
#ifndef __mips__
void videowidget::ReceiveOpenGLstatus(bool result)
{
    if (result) {
        //Success
        if (m_pCamErrItem->isVisible())
            m_pCamErrItem->hide();

        if (m_pNormalView->isVisible())
            m_pNormalView->hide();

        if (m_pNormalItem->isVisible())
            m_pNormalItem->hide();

        m_pNormalScene->update();

        if (get_encoder_status() == 0 && getCapStatus())
            onEndBtnClicked();

        //wayland平台等比例缩放画面
        if (get_wayland_status() == true) {
            int framewidth = m_openglwidget->getFrameWidth();
            int frameheight = m_openglwidget->getFrameHeight();
            int widgetwidth = width();
            int widgetheight = height();

            if (!framewidth && !frameheight) {
                m_openglwidget->resize(widgetwidth, widgetheight);
            } else {
                if ((framewidth * 100 / frameheight) > (widgetwidth * 100 / widgetheight)) {
                    m_openglwidget->resize(widgetwidth, widgetwidth * frameheight / framewidth);
                    m_openglwidget->move(0, (widgetheight - widgetwidth * frameheight / framewidth) / 2);
                } else {

                    m_openglwidget->resize(widgetheight * framewidth / frameheight, widgetheight);
                    m_openglwidget->move((widgetwidth - widgetheight * framewidth / frameheight) / 2, 0);
                }

            }

        } else
            m_openglwidget->resize(width(), height());
        malloc_trim(0);
    }

}

#endif

#ifdef __mips__
void videowidget::ReceiveMajorImage(QImage *image, int result)
{
    if (!image->isNull()) {
        switch (result) {
        case 0:     //Success
            m_imgPrcThread->m_rwMtxImg.lock();

            if (m_pCamErrItem->isVisible())
                m_pCamErrItem->hide();

            if (!m_pNormalView->isVisible())
                m_pNormalView->show();

            {
                int widgetwidth = width();

                int widgetheight = height();
                //qDebug()<<"widgetwidth :" << width() << " widgetheight: " << height() << endl;

                if (get_wayland_status() == true) {
                    if ((image->width() * 100 / image->height()) > (widgetwidth * 100 / widgetheight)) {
                        QImage img = image->scaled(widgetwidth, widgetwidth * image->height() / image->width(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                        m_pixmap = QPixmap::fromImage(img);
                    } else {
                        QImage img = image->scaled(widgetheight * image->width() / image->height(), widgetheight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                        m_pixmap = QPixmap::fromImage(img);
                    }
                } else {
                    QImage img = image->scaled(widgetwidth, widgetheight, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    m_pixmap = QPixmap::fromImage(img);
                }

                m_pNormalScene->setSceneRect(m_pixmap.rect());
                m_pNormalItem->setPixmap(m_pixmap);
                m_imgPrcThread->m_rwMtxImg.unlock();

                if (get_encoder_status() == 0 && getCapStatus() == true)
                    onEndBtnClicked();

                malloc_trim(0);
            }
            break;
        default:
            break;

        }
    }
}
#endif

void videowidget::onReachMaxDelayedFrames()
{
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
#ifndef __mips__

    if (m_openglwidget->isVisible())
        m_openglwidget->hide();//隐藏opengl窗口

#endif
    emit setBtnStatues(false);
}

void videowidget::showCountDownLabel(PRIVIEW_ENUM_STATE state)
{
    QString str;
    switch (state) {
    case PICTRUE:
        m_fWgtCountdown->move((width() - m_fWgtCountdown->width()) / 2,
                              (height() - m_fWgtCountdown->height()) / 2);
        m_fWgtCountdown->show();
        m_btnVdTime->hide();
        m_endBtn->hide();

        if (m_dLabel->pos() == QPoint(0, 0))
            m_dLabel->move((m_fWgtCountdown->width() - m_dLabel->width()) / 2,
                           (m_fWgtCountdown->height() - m_dLabel->height()) / 2);

        m_dLabel->setText(QString::number(m_nInterval));
        break;

    case VIDEO:
        m_pCamErrItem->hide();

        if (m_nCount > MAX_REC_TIME)
            onEndBtnClicked(); //结束录制

        m_fWgtCountdown->hide();

        if (!get_capture_pause()) {//判断是否是暂停状态
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

            m_btnVdTime->setText(strTime);

            if ((m_nCount >= 3)) {
                //开启多线程，每100ms读取时间并显示
                m_countTimer->stop();
                m_recordingTimer->start(200);
                return;
            }

            m_nCount++;
        }
        break;

    default:
        m_pCamErrItem->hide();
        m_fWgtCountdown->hide();
        m_btnVdTime->hide();
        m_endBtn->hide();
        break;
    }

}

void videowidget::resizeEvent(QResizeEvent *size)
{
    Q_UNUSED(size);
    //m_pNormalView->resize(m_pNormalView->parentWidget()->size());
    if (size->size() != size->oldSize()) {
        QSize size = QSize(parentWidget()->size().width(), parentWidget()->size().height());
        m_flashLabel->resize(size);
        m_flashLabel->move(mapToGlobal(QPoint(0, 0)));
    }
    if (m_flashLabel->isVisible()) {
        m_flashLabel->repaint();
    }

    //结束按钮放大缩小的显示
    if (m_endBtn->isVisible())
        m_endBtn->move((width() + m_btnVdTime->width() + 10 - m_endBtn->width()) / 2,
                       height() - m_btnVdTime->height() - 10);

    //计时窗口放大缩小的显示
    if (m_btnVdTime->isVisible())
        m_btnVdTime->move((width() - m_btnVdTime->width() - 10 - m_endBtn->width()) / 2,
                          height() - m_btnVdTime->height() - 5);

    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->move((width() - m_fWgtCountdown->width()) / 2,
                              (height() - m_fWgtCountdown->height()) / 2);
        m_dLabel->move((m_fWgtCountdown->width() - m_dLabel->width()) / 2,
                       (m_fWgtCountdown->height() - m_dLabel->height()) / 2);
    }

    if (m_pCamErrItem->isVisible()) {
        itemPosChange();
        m_pCamErrItem->show();
    } else {
        itemPosChange();
        m_pCamErrItem->hide();
    }

}

void videowidget::recoverTabWidget()
{
    if ((DataManager::instance()->getNowTabIndex() > 0 && DataManager::instance()->getNowTabIndex() < 4)
            || DataManager::instance()->getNowTabIndex() > 7) {
        CMainWindow *parentwidget = CamApp->getMainWindow();

        if (parentwidget) {
            DIconButton *selectbtn = parentwidget->findChild<DIconButton *>(BUTTOM_TITLE_SELECT);
            DButtonBoxButton *titlepicbtn = parentwidget->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
            DButtonBoxButton *titlevdbtn = parentwidget->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);
            DPushButton *picvideobtn = parentwidget->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO);
            ThumbWidget *thumbwidget = parentwidget->findChild<ThumbWidget *>("thumbLeftWidget");

            if (DataManager::instance()->getNowTabIndex() != DataManager::instance()->m_tabIndex)
                DataManager::instance()->setNowTabIndex(DataManager::instance()->m_tabIndex);

            switch (DataManager::instance()->getNowTabIndex()) {
            case 1:
                if (selectbtn) {
                    if (selectbtn->isEnabled())
                        selectbtn->setFocus();
                    else
                        setFocus();
                }
                break;
            case 2:
                if (titlepicbtn) {
                    if (titlepicbtn->isEnabled())
                        titlepicbtn->setFocus();
                    else
                        setFocus();
                }
                break;
            case 3:
                if (titlevdbtn) {
                    if (titlevdbtn->isEnabled())
                        titlevdbtn->setFocus();
                    else
                        setFocus();
                }
                break;
            case 8:
                if (picvideobtn)
                    picvideobtn->setFocus();
                break;
            case 10:
                if (thumbwidget)
                    thumbwidget->setFocus();
                break;
            default:
                break;
            }
        }
    }
}

void videowidget::showCountdown()
{
    if (DataManager::instance()->getdevStatus() == NOCAM) {

        if (m_flashTimer->isActive())
            m_flashTimer->stop();

        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_fWgtCountdown->isVisible())
            m_fWgtCountdown->hide();

        if (m_flashLabel->isVisible())
            m_flashLabel->hide();
    }
    //显示倒数，m_nMaxInterval秒后结束，并拍照
    if (m_nInterval == 0 && DataManager::instance()->getdevStatus() == CAM_CANUSE) {

        if (g_Enum_Camera_State == VIDEO) {
            if (!getCapStatus()) {
                /*m_bActive录制状态判断
                *false：非录制状态
                *true：录制状态
                */
                startTakeVideo();
            }

            //显示录制时长
            showCountDownLabel(g_Enum_Camera_State);
        }

        if (g_Enum_Camera_State == PICTRUE) {
            if (m_nInterval == 0 && m_curTakePicTime > 0) {
                m_flashLabel->show();

                /**
                  * @brief m_flashLabel显示，控件在摄像头切换，标题栏录制，拍照/录制，缩略图左边窗体，
                  * 将焦点移到m_flashlabel
                  */
                if ((DataManager::instance()->m_tabIndex > 0
                        && DataManager::instance()->m_tabIndex < 4
                        && DataManager::instance()->m_tabIndex != 2)
                        || DataManager::instance()->m_tabIndex > 7)
                    m_flashLabel->setFocus();

                m_fWgtCountdown->hide();
                //立即闪光，500ms后关闭
                m_flashTimer->start(500);
                qDebug() << "flashTimer->start();";
#ifndef __mips__
                m_openglwidget->hide();
#endif
                m_thumbnail->hide();
            }

            if (m_curTakePicTime == 0 && m_nInterval == 0) {
                m_flashLabel->show();

                /**
                  * @brief m_flashLabel显示，控件在摄像头切换，标题栏录制，拍照/录制，缩略图左边窗体，
                  * 将焦点移到m_flashlabel
                  */
                if ((DataManager::instance()->m_tabIndex > 0
                        && DataManager::instance()->m_tabIndex < 4
                        && DataManager::instance()->m_tabIndex != 2)
                        || DataManager::instance()->m_tabIndex > 7)
                    m_flashLabel->setFocus();

                m_fWgtCountdown->hide();
                //立即闪光，500ms后关闭
                m_flashTimer->start(500);
#ifndef __mips__
                m_openglwidget->hide();

#endif
                m_thumbnail->hide();
            }
            //发送就结束信号处理按钮状态
            m_countTimer->stop();

            if (QDir(m_savePicFolder).exists() == false) {
                QString strDefaultPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator() + QObject::tr("Camera");
                if (QDir(strDefaultPath).exists() == false) {
                    QDir dir;
                    dir.mkdir(strDefaultPath);
                }

                m_savePicFolder = strDefaultPath;
            }

            QString strFileName = "UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".jpg";
            emit filename(strFileName);
            m_imgPrcThread->m_strPath = m_savePicFolder + QDir::separator() + strFileName;
            m_imgPrcThread->m_bTake = true; //保存图片标志
            m_nFileID++;
            m_curTakePicTime -= 1;

            if (m_curTakePicTime == 0) {
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
            m_fWgtCountdown->hide();
            m_pNormalView->hide();
        }

    } else {
        showCountDownLabel(PICTRUE); //拍照录像都要显示倒计时
        m_nInterval--;
        qInfo() << "m_nInterval:" << m_nInterval;
    }

}

void videowidget::showRecTime()
{
    //获取写video的时间
    m_nCount = static_cast<int>(get_video_time_capture());

    //过滤不正常的时间
    if (m_nCount <= 3) {
        qWarning() << "error time" << m_nCount;
        return;
    }

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

    m_btnVdTime->setText(strTime);
}

void videowidget::flash()
{
    if (m_flashLabel->isVisible()) {
        //隐藏闪光窗口

        if (get_sound_of_takeing_photo())
            m_takePicSound->play();

#ifndef __mips__

        if (m_openglwidget->isVisible() == false)
            m_openglwidget->show();

#endif
        m_thumbnail->show();
        recoverTabWidget();

        m_flashLabel->hide(); //为避免没有关闭，放到定时器里边关闭

        if (m_curTakePicTime == 0)
            stopEverything();

        qDebug() << "m_flashLabel->hide();";
    }
}
void videowidget::slotresolutionchanged(const QString &resolution)
{
    if (DataManager::instance()->getdevStatus() != CAM_CANUSE)
        return ;

    QStringList ResStr = resolution.split("x");
    int newwidth = ResStr[0].toInt();//新的宽度
    int newheight = ResStr[1].toInt();//新的高度

    int nWidth = v4l2core_get_frame_width(get_v4l2_device_handler());
    int nHeight = v4l2core_get_frame_height(get_v4l2_device_handler());

    if (newwidth != nWidth && newheight != nHeight) {
        //设置刷新率
        v4l2core_define_fps(get_v4l2_device_handler(), 1, 30);
        //设置新的分辨率
        v4l2core_prepare_new_resolution(get_v4l2_device_handler(), newwidth, newheight);
        request_format_update(1);
    }

}

void videowidget::onEndBtnClicked()
{
    if (m_countTimer->isActive())
        m_countTimer->stop();

    if (m_recordingTimer->isActive())
        m_recordingTimer->stop();

    if (m_pCamErrItem->isVisible() && (m_imgPrcThread->getStatus() == 0))
        m_pCamErrItem->hide();

    if (m_fWgtCountdown->isVisible())
        m_fWgtCountdown->hide();


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

    m_btnVdTime->hide();
    m_endBtn->hide();

    if (getCapStatus()) { //录制完成处理
        qDebug() << "stop takeVideo";

        if (video_capture_get_save_video() == 1) {
            set_video_time_capture(0);
            stop_encoder_thread();
        }

        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
        emit takeVdDone();
    }
}

void videowidget::onRestartDevices()
{
    if (DataManager::instance()->getdevStatus() != CAM_CANUSE) {
        onChangeDev();

        if (DataManager::instance()->getdevStatus() == CAM_CANUSE) {
            emit sigDeviceChange();
            QPalette plt = palette();
            plt.setColor(QPalette::Window, Qt::white);
            m_pNormalView->hide();
#ifndef __mips__
            m_openglwidget->show();
#endif
        }

    }

}

void videowidget::onChangeDev()
{
    v4l2_dev_t *devicehandler =  get_v4l2_device_handler();

    if (m_imgPrcThread != nullptr)
        m_imgPrcThread->stop();

    while (m_imgPrcThread->isRunning());
    QString str;

    if (devicehandler != nullptr) {
        str = QString(devicehandler->videodevice);
        close_v4l2_device_handler();
    }

    v4l2_device_list_t *devlist = get_device_list();
    if (devlist->num_devices == 2) {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (str != str1) {
                int ret = camInit(devlist->list_devices[i].device);
                if (ret == E_OK) {
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                    DataManager::instance()->setdevStatus(CAM_CANUSE);
                    break;

                } else if (ret == E_FORMAT_ERR) {
                    v4l2_dev_t *vd =  get_v4l2_device_handler();

                    if (vd != nullptr)
                        close_v4l2_device_handler();

                    if (DataManager::instance()->getdevStatus() != CAM_CANNOT_USE)
                        showCamUsed();

                    DataManager::instance()->setdevStatus(CAM_CANNOT_USE);

                } else if (ret == E_NO_DEVICE_ERR) {
                    v4l2_dev_t *vd =  get_v4l2_device_handler();

                    if (vd != nullptr)
                        close_v4l2_device_handler();

                    DataManager::instance()->setdevStatus(NOCAM);
                    showNocam();
                }

            }

        }

    } else {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);

            if (str == str1) {
                if (i == devlist->num_devices - 1) {
                    int ret = camInit(devlist->list_devices[0].device);

                    if (ret == E_OK) {
                        m_imgPrcThread->init();
                        m_imgPrcThread->start();
                        DataManager::instance()->setdevStatus(CAM_CANUSE);
                    } else if (ret == E_FORMAT_ERR) {
                        v4l2_dev_t *vd =  get_v4l2_device_handler();

                        if (vd != nullptr)
                            close_v4l2_device_handler();

                        if (DataManager::instance()->getdevStatus() != CAM_CANNOT_USE)
                            showCamUsed();

                        DataManager::instance()->setdevStatus(CAM_CANNOT_USE);
                    } else if (ret == E_NO_DEVICE_ERR) {
                        v4l2_dev_t *vd =  get_v4l2_device_handler();

                        if (vd != nullptr)
                            close_v4l2_device_handler();

                        DataManager::instance()->setdevStatus(NOCAM);
                        showNocam();
                    }

                    break;
                } else {
                    int ret = camInit(devlist->list_devices[i + 1].device);

                    if (ret == E_OK) {
                        m_imgPrcThread->init();
                        m_imgPrcThread->start();
                        DataManager::instance()->setdevStatus(CAM_CANUSE);
                    } else if (ret == E_FORMAT_ERR) {
                        v4l2_dev_t *vd =  get_v4l2_device_handler();

                        if (vd != nullptr)
                            close_v4l2_device_handler();

                        if (DataManager::instance()->getdevStatus() != CAM_CANNOT_USE)
                            showCamUsed();

                        DataManager::instance()->setdevStatus(CAM_CANNOT_USE);
                    } else if (ret == E_NO_DEVICE_ERR) {
                        v4l2_dev_t *vd =  get_v4l2_device_handler();

                        if (vd != nullptr)
                            close_v4l2_device_handler();

                        DataManager::instance()->setdevStatus(NOCAM);
                        showNocam();
                    }

                    break;
                }

            }

            if (str.isEmpty()) {
                int ret = camInit(devlist->list_devices[0].device);

                if (ret == E_OK) {
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                    DataManager::instance()->setdevStatus(CAM_CANUSE);
                } else if (ret == E_FORMAT_ERR) {
                    v4l2_dev_t *vd =  get_v4l2_device_handler();

                    if (vd != nullptr)
                        close_v4l2_device_handler();

                    qWarning() << "camInit failed";

                    if (DataManager::instance()->getdevStatus() != CAM_CANNOT_USE)
                        showCamUsed();

                    DataManager::instance()->setdevStatus(CAM_CANNOT_USE);
                } else if (ret == E_NO_DEVICE_ERR) {
                    v4l2_dev_t *vd =  get_v4l2_device_handler();

                    if (vd != nullptr)
                        close_v4l2_device_handler();

                    DataManager::instance()->setdevStatus(NOCAM);
                    showNocam();
                }

                break;
            }

        }

    }

}

void videowidget::onTakePic(bool bTrue)
{
    g_Enum_Camera_State = PICTRUE;

    if (bTrue) {
        if (m_fWgtCountdown) {
            m_fWgtCountdown->move((width() - m_fWgtCountdown->width()) / 2,
                                  (height() - m_fWgtCountdown->height()) / 2);
        }

        //1、重置状态
        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_pCamErrItem->isVisible() && (m_imgPrcThread->getStatus() == 0))
            m_pCamErrItem->hide();

        if (m_fWgtCountdown->isVisible())
            m_fWgtCountdown->hide();

        m_nInterval = m_nMaxInterval = m_Maxinterval;
        m_curTakePicTime = m_nMaxContinuous;
        emit updateBlockSystem(true);//连拍时开启关机阻止
        m_countTimer->start(m_nMaxInterval == 0 ? 34 : 1000);
    } else {
        emit takePicCancel();
        emit updateBlockSystem(false);//连拍取消时，取消阻止关机
        m_nInterval = 0; //下次可开启
        m_curTakePicTime = 0; //结束当前拍照

        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_fWgtCountdown->isVisible())
            m_fWgtCountdown->hide();

        if (m_flashLabel->isVisible())
            m_flashLabel->hide();

        QEventLoop eventloop;
        QTimer::singleShot(300, &eventloop, SLOT(quit()));
        eventloop.exec();
#ifndef __mips__

        if (!m_openglwidget->isVisible())
            m_openglwidget->show();

#endif

        if (!m_thumbnail->isVisible()) {
            m_thumbnail->show();
            recoverTabWidget();
        }
    }
}

void videowidget::onTakeVideo() //点一次开，再点一次关
{
    if (m_nInterval >= 0 && m_countTimer->isActive()) { //倒计时期间的处理
        m_nInterval = 0; //下次可开启
        set_video_time_capture(0);
        emit takeVdCancel(); //用于恢复缩略图
        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_pCamErrItem->isVisible())
            m_pCamErrItem->hide();

        if (m_fWgtCountdown->isVisible())
            m_fWgtCountdown->hide();
        return; //return即可，这个是外部过来的信号，外部有处理相关按钮状态、恢复缩略图状态
    }

    if (getCapStatus()) { //录制完成处理
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
        if (m_countTimer->isActive())
            m_countTimer->stop();

        if (m_pCamErrItem->isVisible())
            m_pCamErrItem->hide();

        if (m_fWgtCountdown->isVisible())
            m_fWgtCountdown->hide();
        return;
    }

    g_Enum_Camera_State = VIDEO;

    if (m_Maxinterval == 0) {
        //直接录制
        showCountdown();
    } else {
        m_nInterval = m_Maxinterval;
        m_countTimer->start(1000);
    }

}

void videowidget::forbidScrollBar(QGraphicsView *view)
{
    view->horizontalScrollBar()->blockSignals(true);
    view->verticalScrollBar()->blockSignals(true);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void videowidget::startTakeVideo()
{
    QDir dir;

    QString strDefaultVdPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + QObject::tr("Camera");
    if (QDir(strDefaultVdPath).exists() == false)
        dir.mkdir(strDefaultVdPath);

    if (getCapStatus()) {
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        setCapStatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
    } else {
        if (DataManager::instance()->getdevStatus() == CAM_CANUSE) {
            qDebug() << "start takeVideo";
            DataManager::instance()->getstrFileName() = "UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".webm";
            emit filename(DataManager::instance()->getstrFileName());
            m_nFileID ++;

            if (QDir(m_saveVdFolder).exists() == false) {
                m_saveVdFolder = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + QObject::tr("Camera");
            }

            set_video_path(m_saveVdFolder.toStdString().c_str());
            set_video_name(DataManager::instance()->getstrFileName().toStdString().c_str());
            start_encoder_thread();
            emit updateBlockSystem(true);
            setCapStatus(true);
            //begin_time = QDateTime::currentDateTime();
            m_countTimer->stop();
            m_countTimer->start(1000); //重新计时，否则时间与显示时间
        } else {
            reset_video_timer();//重置底层定时器
            m_countTimer->stop();
            //countTimer->start(1000);
            setCapStatus(false);
        }

        m_nCount = 0;//5184000;
        m_btnVdTime->setText(QString("00:00:00"));
        int nWidth = width();
        int nHeight = height();

        //判断倒计时阶段焦点位移到拍照/录制按钮，重新设置tab的索引
        if (DataManager::instance()->m_tabIndex == 8)
            DataManager::instance()->setNowTabIndex(8);

        parentWidget()->findChild<ThumbnailsBar *>()->hide();
        m_endBtn->show();

        //录制视频开始之前，索引在拍照/录制按钮，设置焦点在结束按钮
        if (DataManager::instance()->getNowTabIndex() == 8)
            m_endBtn->setFocus();

        m_btnVdTime->show();
        m_btnVdTime->move((nWidth - m_btnVdTime->width() - 10 - m_endBtn->width()) / 2,
                          nHeight - m_btnVdTime->height() - 6);
        m_endBtn->move((nWidth + m_btnVdTime->width() + 10 - m_endBtn->width()) / 2,
                       nHeight - m_btnVdTime->height() - 10);
    }
}

void videowidget::itemPosChange()
{
    m_pCamErrItem->setPos((m_pixmap.width() - static_cast<int>(m_pCamErrItem->boundingRect().width())) / 2, m_pixmap.height());
    m_pNormalItem->update();
    m_pCamErrItem->update();
}

void videowidget::stopEverything()
{
    if (m_countTimer->isActive())
        m_countTimer->stop();


    if (m_flashLabel->isVisible())
        m_flashLabel->hide();


    if (m_pCamErrItem->isVisible())
        m_pCamErrItem->hide();


    if (m_fWgtCountdown->isVisible())
        m_fWgtCountdown->hide();

#ifndef __mips__
    if (!m_openglwidget->isVisible())
        m_openglwidget->show();
#endif
}
