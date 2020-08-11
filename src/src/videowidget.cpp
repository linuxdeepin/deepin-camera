/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer:  shicetu <shicetu@uniontech.com>
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
#include "camview.h"
#include "gui.h"
#include <QPixmap>
#include <QTimer>
#include <QGraphicsView>
#include <QDateTime>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QThread>
#include <QScrollBar>
#include <DGuiApplicationHelper>
#include <DApplicationHelper>
#include <QGraphicsBlurEffect>
#include <QKeyEvent>
#include <QDir>

static PRIVIEW_STATE VIDEO_STATE = NORMALVIDEO;
QString g_strFileName = nullptr;
volatile int g_devStatus = NOCAM;
videowidget::videowidget(DWidget *parent) : DWidget(parent)
{
    m_btnClickTime = QDateTime::currentDateTime();
    m_strFolder = "";
    m_nFileID = 0;
    m_nInterval = 0;
    m_curTakePicTime = 0;
    m_nCount = 0;
    countTimer = new QTimer(this);
    connect(countTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));//默认

    flashTimer = new QTimer(this);
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(flash()));//默认
//    this->setFixedSize(1000, 1000);
    m_pNormalView = new QGraphicsView(this);
    m_flashLabel  = new DLabel(this);
    m_btnVdTime = new DPushButton(this);
    m_fWgtCountdown = new DFloatingWidget(this);
    m_fWgtCountdown->hide(); //先隐藏
    m_fWgtCountdown->setFixedSize(160, 144);
    m_fWgtCountdown->setBlurBackgroundEnabled(true);
    m_dLabel = new DLabel(m_fWgtCountdown);

    m_btnVdTime->setIcon(QIcon(":/images/icons/light/Timer Status.svg"));
    m_btnVdTime->setIconSize(QSize(6, 6));
    m_btnVdTime->hide(); //先隐藏
    m_btnVdTime->setFixedSize(92, 36);
    m_btnVdTime->setAttribute(Qt::WA_TranslucentBackground);
    m_btnVdTime->setEnabled(false);

    int nType = DGuiApplicationHelper::instance()->themeType();

    m_dLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_dLabel->setAlignment(Qt::AlignCenter);
    QFont ftLabel("SourceHanSansSC-ExtraLight");
    ftLabel.setWeight(QFont::Thin);//最小就是50,更小的设置不进去，也是bug？
    ftLabel.setPixelSize(60);
    m_dLabel->setFont(ftLabel);
    //ftLabel.setBold(true);
    //qDebug() << ftLabel.weight();
    QPalette pltLabel = m_dLabel->palette();
    if (DGuiApplicationHelper::LightType == nType) {
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
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType() ) {
//        QColor clr(Qt::white);//未生效
//        clr.setAlphaF(0.2);
//        pa_cb.setBrush(QPalette::Light, clr); //浅色
        pa_cb.setColor(QPalette::ButtonText, QColor(255, 44, 44));
    } else {
//        QColor clr(Qt::black);
//        clr.setAlphaF(0.2);
//        pa_cb.setColor(QPalette::Light, clr); //深色
        pa_cb.setColor(QPalette::ButtonText, QColor(202, 0, 0));


    }
    m_btnVdTime->setPalette(pa_cb);



    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::Normal);
    ft.setPixelSize(14);
    m_btnVdTime->setFont(ft);
    m_btnVdTime->setText(QString("00:00:00"));

    m_endBtn = new DPushButton(this);
    m_endBtn->setFlat(true);
    m_endBtn->setFixedSize(QSize(56, 51));

    m_endBtn->setIconSize(QSize(56, 51));
    m_endBtn->setIcon(QIcon(":/images/icons/light/Stop Recording.svg"));

    connect(m_endBtn, SIGNAL(clicked()), this, SLOT(endBtnClicked()));

    m_endBtn->setToolTip(tr("Stop taking video"));
    m_endBtn->setToolTipDuration(500); //0.5s消失
    m_endBtn->hide();

    m_pNormalScene = new QGraphicsScene;
    //禁用滚动条
    forbidScrollBar(m_pNormalView);

    m_pNormalView->setAlignment(Qt::AlignHCenter | Qt::AlignJustify);

    m_pNormalView->setScene(m_pNormalScene);

    m_pNormalView->setAttribute(Qt::WA_TranslucentBackground);
//    m_pNormalView->fitInView(m_pNormalScene->sceneRect(), Qt::KeepAspectRatioByExpanding);

    m_pNormalItem = new QGraphicsPixmapItem;
    m_pCamErrItem = new QGraphicsTextItem;

    //添加布局
    m_pGridLayout = new QGridLayout(this);
    m_pGridLayout->setContentsMargins(0, 0, 0, 0);
    m_pGridLayout->addWidget(m_pNormalView);

    m_pNormalScene->addItem(m_pNormalItem);
    m_pNormalScene->addItem(m_pCamErrItem);
    //m_pNormalScene->addItem(m_pTimeItem);
//    m_pNormalScene->setSceneRect(this->rect());
//    m_pNormalItem->setPos(0, 0);
//    m_pNormalScene->addRect(-100, -75, 200, 150, QPen(Qt::red));
    qDebug() << "this widget--height:" << this->height() << "--width:" << this->width() << endl;
    qDebug() << "this widget--height:" << m_pNormalView->height() << "--width:" << m_pNormalView->width() << endl;
    qDebug() << "this widget--height:" << m_pNormalScene->height() << "--width:" << m_pNormalScene->width() << endl;
    init();
}

videowidget::~videowidget()
{
    m_imgPrcThread->deleteLater();
}

void videowidget::init()
{
    setCapstatus(false);

    m_imgPrcThread = new MajorImageProcessingThread;
    m_imgPrcThread->m_bTake = false;


    m_flashLabel->setWindowFlag(Qt::WindowType::ToolTip);
    m_flashLabel->hide();

    //启动视频
    int ret =  camInit("/dev/video0");
    if (ret == E_OK) {
        g_devStatus = CAM_CANUSE;

        m_pCamErrItem->hide();
        m_imgPrcThread->start();
    } else if (ret == E_FORMAT_ERR) {
        g_devStatus = CAM_CANNOT_USE;
        //启动失败
//        v4l2_dev_t *vd = get_v4l2_device_handler();
//        //如果不为空，则关闭vd
//        if (vd != nullptr) {
//            close_v4l2_device_handler();
//            vd = nullptr;
//        }
        showCamUsed();
        qDebug() << "cam in use" << endl;
    } else if (ret == E_NO_DEVICE_ERR) {
        g_devStatus = NOCAM;
        //启动失败
        v4l2_dev_t *vd = get_v4l2_device_handler();
        //如果不为空，则关闭vd
        if (vd != nullptr) {
            close_v4l2_device_handler();
            vd = nullptr;
        }
        showNocam();
        qDebug() << "No webcam found" << endl;
    }

    connect(m_imgPrcThread, SIGNAL(SendMajorImageProcessing(QPixmap, int)),
            this, SLOT(ReceiveMajorImage(QPixmap, int)));

    connect(m_imgPrcThread, SIGNAL(reachMaxDelayedFrames()),
            this, SLOT(onReachMaxDelayedFrames()));

    connect(this, SIGNAL(sigFlash()), this, SLOT(flash()));

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
//        QColor clrShadow(0,0,0);
//        clrShadow.setAlphaF(0.2);
//        pltLabel.setColor(QPalette::Shadow, clrShadow);
        m_dLabel->setPalette(pltLabel);

        if (m_btnVdTime->isVisible()) {
            if (type == DGuiApplicationHelper::LightType) {
                DPalette pa_cb = m_btnVdTime->palette();
//                QColor clr(Qt::white);
//                clr.setAlphaF(0.2);
//                pa_cb.setBrush(QPalette::Light, clr); //浅色
                pa_cb.setColor(QPalette::ButtonText, QColor(255, 44, 44));
                m_btnVdTime->setPalette(pa_cb);
            } else {
                DPalette pa_cb = m_btnVdTime->palette();
//                QColor clr(Qt::black);
//                clr.setAlphaF(0.2);
//                pa_cb.setBrush(QPalette::Dark, clr); //深色
                pa_cb.setColor(QPalette::ButtonText, QColor(202, 0, 0));
                m_btnVdTime->setPalette(pa_cb);
            }
        }
        if (m_pCamErrItem->isVisible()) {
            QString str;
            if (type == DGuiApplicationHelper::LightType) {
                if (g_devStatus == NOCAM) {
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
                QPalette plt = this->palette();
                plt.setColor(QPalette::Base, clrBase);
                this->setPalette(plt);


            } else if (type == DGuiApplicationHelper::DarkType) {
                if (g_devStatus == NOCAM) {
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
                QPalette plt = this->palette();
                plt.setColor(QPalette::Base, clrBase);
                this->setPalette(plt);
            }
            QFont ft("SourceHanSansSC");
            ft.setWeight(QFont::DemiBold);
            ft.setPixelSize(17);
            m_pCamErrItem->setFont(ft);

            m_pNormalScene->setSceneRect(m_pixmap.rect());
            itemPosChange();
            m_pNormalItem->setPixmap(m_pixmap);
            if (m_flashLabel->isVisible()) {
                m_flashLabel->hide();
            }
        }
    });
}

//显示没有设备的图片的槽函数
void videowidget::showNocam()
{
    emit sigDeviceChange();
    QString str(tr("No webcam found"));//未连接摄像头
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType() ) {
        QImage img(":/images/icons/light/Not connected.svg");
        m_pixmap = QPixmap::fromImage(img);
        QColor clr(255, 255, 255);
        clr.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clr);

        QColor clrBase(0, 0, 0);
        clrBase.setAlphaF(0.7);
        QPalette plt = this->palette();
        plt.setColor(QPalette::Base, clrBase);
        this->setPalette(plt);
    } else {
        QImage img(":/images/icons/dark/Not connected_dark.svg");
        m_pixmap = QPixmap::fromImage(img);
        QColor clr(0, 0, 0);
        clr.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clr);

        QColor clrBase(255, 255, 255);
        clrBase.setAlphaF(0.7);
        QPalette plt = this->palette();
        plt.setColor(QPalette::Base, clrBase);
        this->setPalette(plt);
    }
    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::DemiBold);
    ft.setPixelSize(17);
    m_pCamErrItem->setFont(ft);
    m_pCamErrItem->setPlainText(str);
    m_pNormalScene->setSceneRect(m_pixmap.rect());
    m_pNormalItem->setPixmap(m_pixmap);
    if (m_flashLabel->isVisible()) {
        m_flashLabel->hide();
    }

    itemPosChange();
    m_pCamErrItem->show();
    emit noCam();
    if (getCapstatus()) { //录制完成处理
        endBtnClicked();
    }
}

//显示设备被占用或者拔掉的图片的槽函数
void videowidget::showCamUsed()
{
    emit sigDeviceChange();
    QString str(tr("The webcam is in use"));//摄像头已被占用
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType() ) {
        QImage img(":/images/icons/light/Take up.svg");
        m_pixmap = QPixmap::fromImage(img);

        QColor clrText(Qt::white);
        clrText.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clrText);//浅色主题文字和图片是白色，特殊处理

        m_pCamErrItem->setPlainText(str);

        QPalette plt = this->palette();
        QColor clrBackGRD(Qt::black);
        clrBackGRD.setAlphaF(0.7);
        plt.setColor(QPalette::Base, clrBackGRD);
        this->setPalette(plt);
    } else {
        QImage img(":/images/icons/dark/Take up_dark.svg");
        m_pixmap = QPixmap::fromImage(img);

        QColor clrText(Qt::black);
        clrText.setAlphaF(0.8);
        m_pCamErrItem->setDefaultTextColor(clrText);

        m_pCamErrItem->setPlainText(str);

        QPalette plt = this->palette();
        QColor clrBackGRD(Qt::white);
        clrBackGRD.setAlphaF(0.7);
        plt.setColor(QPalette::Base, clrBackGRD);
        this->setPalette(plt);
    }
    QFont ft("SourceHanSansSC");
    ft.setWeight(QFont::DemiBold);
    ft.setPixelSize(17);
    m_pCamErrItem->setFont(ft);
    m_pNormalScene->setSceneRect(m_pixmap.rect());
    m_pNormalItem->setPixmap(m_pixmap);
    if (m_flashLabel->isVisible()) {
        m_flashLabel->hide();
    }

    itemPosChange();
    m_pCamErrItem->show();
    emit noCamAvailable();
    if (getCapstatus()) { //录制完成处理
        endBtnClicked();
    }
}

void videowidget::ReceiveMajorImage(QPixmap image, int result)
{
    if (!image.isNull()) {
        switch (result) {
        case 0:     //Success
            m_imgPrcThread->m_rwMtxImg.lock();
            if (m_pCamErrItem->isVisible() == true) {
                m_pCamErrItem->hide();
            }
            m_pixmap = image.scaled(this->parentWidget()->width(), this->parentWidget()->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            m_pNormalScene->setSceneRect(m_pixmap.rect());
            m_pNormalItem->setPixmap(m_pixmap);
            m_imgPrcThread->m_rwMtxImg.unlock();
            if (get_encoder_status() == 0 && getCapstatus() == true) {
                endBtnClicked();
            }
            break;
        default:
            break;
        }
    }
}

void videowidget::onReachMaxDelayedFrames()
{
    check_device_list_events(get_v4l2_device_handler());
    v4l2_dev_t *vd =  get_v4l2_device_handler();
    if (m_imgPrcThread != nullptr) {
        m_imgPrcThread->stop();
    }
    while (m_imgPrcThread->isRunning());
    if (vd != nullptr) {
        close_v4l2_device_handler();
    }
    if (get_device_list()->num_devices < 1) {
        g_devStatus = NOCAM;
        stopEverything();
        showNocam();
    } else {
        g_devStatus = CAM_CANNOT_USE;
        showCamUsed();
    }
    emit setBtnStatues(false);
}

void videowidget::showCountDownLabel(PRIVIEW_STATE state)
{

    QString str;
    switch (state) {
    case NORMALVIDEO:
        //no device found

        m_fWgtCountdown->move((this->width() - m_fWgtCountdown->width()) / 2,
                              (this->height() - m_fWgtCountdown->height()) / 2);

        m_fWgtCountdown->show();

        //m_pCountItem->show();
        m_btnVdTime->hide();
        m_endBtn->hide();

        if (m_dLabel->pos() == QPoint(0, 0))
            m_dLabel->move((m_fWgtCountdown->width() - m_dLabel->width()) / 2,
                           (m_fWgtCountdown->height() - m_dLabel->height()) / 2);
        m_dLabel->setText(QString::number(m_nInterval));
        break;
    case AUDIO:
        m_pCamErrItem->hide();
        if (m_nCount > MAX_REC_TIME) {
            endBtnClicked(); //结束录制
        }
        m_fWgtCountdown->hide();
        if (!get_capture_pause()) {//判断是否是暂停状态
            QString strTime = "";
            int nHour = m_nCount / 3600;
            if (nHour == 0) {
                strTime.append("00");
            } else if (nHour < 10) {
                strTime.append("0");
                strTime.append(QString::number(nHour));
            } else {
                strTime.append(QString::number(nHour));
            }
            strTime.append(":");
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

void videowidget::hideCountDownLabel()
{
    //关闭
    m_pCamErrItem->hide();
    m_fWgtCountdown->hide();
}

void videowidget::hideTimeLabel()
{
    m_btnVdTime->hide();
    m_endBtn->hide();
}

void videowidget::resizeEvent(QResizeEvent *size)
{
    Q_UNUSED(size);
    m_pNormalView->resize(m_pNormalView->parentWidget()->size());
    if (m_flashLabel->isVisible() == true) {
        m_flashLabel->resize(parentWidget()->size());
        m_flashLabel->move(mapToGlobal(QPoint(0, 0)));
        m_flashLabel->update();
    }

    //结束按钮放大缩小的显示
    if (m_endBtn->isVisible()) {
        m_endBtn->move((width() + m_btnVdTime->width() + 10 - m_endBtn->width()) / 2,
                       height() - m_btnVdTime->height() - 10);
    }

    //计时窗口放大缩小的显示
    if (m_btnVdTime->isVisible()) {
        m_btnVdTime->move((width() - m_btnVdTime->width() - 10 - m_endBtn->width()) / 2,
                          height() - m_btnVdTime->height() - 5);
    }

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

void videowidget::showCountdown()
{
    if (m_imgPrcThread->getStatus()) {//拍照拍摄时拔出摄像头的异常处理
        if (flashTimer->isActive() && !m_flashLabel->isVisible()) {
            flashTimer->stop();
        }
        if (countTimer->isActive()) {
            countTimer->stop();
        }
        if (VIDEO_STATE == AUDIO) {
            if (getCapstatus()) {//录制中
                endBtnClicked();
            } else {
                emit takeVdCancel();
            }
        }
        if (VIDEO_STATE == NORMALVIDEO) {
            emit takePicCancel();
        }
        //hideCountDownLabel();
        m_fWgtCountdown->hide();
        return;
    }
    //显示倒数，m_nMaxInterval秒后结束，并拍照
    if (m_nInterval == 0) {
        if (VIDEO_STATE == AUDIO) {
            if (!getCapstatus()) {
                /*m_bActive录制状态判断
                *false：非录制状态
                *true：录制状态
                */
                startTakeVideo();
            }

            //显示录制时长
            showCountDownLabel(VIDEO_STATE);
        }
        if (VIDEO_STATE == NORMALVIDEO) {
            if (m_nMaxInterval == 0) {
                if (flashTimer->isActive()) {
                    flashTimer->stop();
                }
                //立即闪光，500ms后关闭
                flashTimer->start(FLASH_TIME);

                m_flashLabel->resize(this->size());
                m_flashLabel->move(this->mapToGlobal(QPoint(0, 0)));
                qDebug() << "m_flashLabel->show();";
                m_flashLabel->show();
            }
            //发送就结束信号处理按钮状态
            countTimer->stop();
            if (QDir(m_strFolder).exists() == false) {
                m_strFolder = QDir::homePath() + QString("/Videos");
            }
            m_imgPrcThread->m_strPath = m_strFolder + "/UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".jpg";
            m_imgPrcThread->m_bTake = true; //保存图片标志

            m_nFileID++;
            if (--m_curTakePicTime == 0) {
                //拍照结束，恢复按钮状态和缩略图标志位
                QThread *thread = QThread::create([ = ]() {
                    int nCount = 0;
                    while(true) {
                        if (nCount > 50) {
                            qDebug() << "too long to emit takePicDone";
                            break;
                        }
                        if (!m_imgPrcThread->m_bTake) {
                            break;
                        }
                        nCount ++;
                        QThread::msleep(100);
                    }
                    emit takePicDone();
                    QThread::currentThread()->quit();
                });
                thread->start();
            } else {
                QThread *thread = QThread::create([ = ]() {
                    int nCount = 0;
                    while(true) {
                        if (nCount > 50) {
                            qDebug() << "too long to emit takePicOnce";
                            break;
                        }
                        if (!m_imgPrcThread->m_bTake) {
                            break;
                        }
                        nCount ++;
                        QThread::msleep(100);
                    }
                    emit takePicOnce();
                    QThread::currentThread()->quit();
                });
                thread->start();

            }

            if (m_curTakePicTime > 0 && g_devStatus == CAM_CANUSE) {
                countTimer->start(m_nMaxInterval == 34 ? 0 : 1000);
                m_nInterval = m_nMaxInterval; //改到开始的时候设置
            }
            hideCountDownLabel();
        }

    } else {
        if (countTimer->isActive()) {
            showCountDownLabel(NORMALVIDEO); //拍照录像都要显示倒计时
            if (VIDEO_STATE == NORMALVIDEO) {
                if (m_nInterval % m_nMaxInterval == 1) {
                    if (flashTimer->isActive()) {
                        flashTimer->stop();
                    }
                    //等500ms后闪光，内部关闭
                    flashTimer->start(FLASH_TIME);
                }
            }
            m_nInterval--;
        }
    }
}

void videowidget::flash()
{
    if (m_flashLabel->isVisible()) {
        //隐藏闪光窗口
        qDebug() << "m_flashLabel->hide();";
        m_flashLabel->hide(); //为避免没有关闭，放到定时器里边关闭
        flashTimer->stop();
    } else {
        m_flashLabel->resize(this->size());
        m_flashLabel->move(this->mapToGlobal(QPoint(0, 0)));
        qDebug() << "m_flashLabel->show();";
        m_flashLabel->show();
        if (flashTimer->isActive()) { //连续点击拍照
            flashTimer->stop();
        }
        flashTimer->start(FLASH_TIME);
    }
}

void videowidget::slotresolutionchanged(const QString &resolution)
{
    if (g_devStatus != CAM_CANUSE) {
        return ;
    }
    QStringList ResStr = resolution.split("x");
    int newwidth = ResStr[0].toInt();//新的宽度
    int newheight = ResStr[1].toInt();//新的高度
    //设置刷新率
    v4l2core_define_fps(get_v4l2_device_handler(), 1, 30);
    //设置新的分辨率
    v4l2core_prepare_new_resolution(get_v4l2_device_handler(), newwidth, newheight);
    request_format_update(1);
    config_t *my_config = config_get();

    my_config->width = newwidth;
    my_config->height = newheight;
}

void videowidget::endBtnClicked()
{
    if (countTimer->isActive()) {
        countTimer->stop();
    }
    if (m_pCamErrItem->isVisible() && !m_imgPrcThread->getStatus()) {
        m_pCamErrItem->hide();
    }
    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->hide();
    }
    m_btnVdTime->hide();
    m_endBtn->hide();

    if (getCapstatus()) { //录制完成处理
        qDebug() << "stop takeVideo";
        if (video_capture_get_save_video() == 1)
            stop_encoder_thread();
        setCapstatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
    }
    emit takeVdDone();
//    g_strFileName = nullptr;
}

void videowidget::restartDevices()
{
    if (g_devStatus != CAM_CANUSE) {
        changeDev();
        if (g_devStatus == CAM_CANUSE) {
            emit sigDeviceChange();
        }
    }
}

void videowidget::changeDev()
{
    v4l2_dev_t *vd =  get_v4l2_device_handler();
    if (m_imgPrcThread != nullptr) {
        m_imgPrcThread->stop();
    }
    while (m_imgPrcThread->isRunning());
    QString str;
    if (vd != nullptr) {
        str = QString(vd->videodevice);
        close_v4l2_device_handler();
    }
    v4l2_device_list_t *devlist = get_device_list();
    if (devlist->num_devices == 2) {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (str != str1) {
                if (camInit(devlist->list_devices[i].device) == E_OK) {
                    g_devStatus = CAM_CANUSE;
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                } else {
                    qDebug() << "camInit failed";
                    g_devStatus = CAM_CANNOT_USE;
                    showCamUsed();
                }
                break;
            }
        }
    } else {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (str == str1) {
                if (i == devlist->num_devices - 1) {
                    if (camInit(devlist->list_devices[0].device) == E_OK) {
                        m_imgPrcThread->init();
                        m_imgPrcThread->start();
                        g_devStatus = CAM_CANUSE;
                    } else {
                        qDebug() << "camInit failed";
                        g_devStatus = CAM_CANNOT_USE;
                        showCamUsed();
                    }
                    break;
                } else {
                    if (camInit(devlist->list_devices[i + 1].device) == E_OK) {
                        m_imgPrcThread->init();
                        m_imgPrcThread->start();
                        g_devStatus = CAM_CANUSE;
                    } else {
                        qDebug() << "camInit failed";
                        g_devStatus = CAM_CANNOT_USE;
                        showCamUsed();
                    }
                    break;
                }
            }
            if (str.isEmpty() == true) {
                if (camInit(devlist->list_devices[0].device) == E_OK) {
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                    g_devStatus = CAM_CANUSE;
                } else {
                    qDebug() << "camInit failed";
                    g_devStatus = CAM_CANNOT_USE;
                    showCamUsed();
                }
                break;
            }
        }
    }
}

void videowidget::onTakePic(bool bTrue)
{
    VIDEO_STATE = NORMALVIDEO;
    if (bTrue) {
        if (m_fWgtCountdown) {
            m_fWgtCountdown->move((this->width() - m_fWgtCountdown->width()) / 2,
                                  (this->height() - m_fWgtCountdown->height()) / 2);
        }
        //1、重置状态
        if (countTimer->isActive()) {
            countTimer->stop();
        }
        if (m_pCamErrItem->isVisible() && !m_imgPrcThread->getStatus()) {
            m_pCamErrItem->hide();
        }
        if (m_fWgtCountdown->isVisible()) {
            m_fWgtCountdown->hide();
        }

        m_nInterval = m_nMaxInterval;
        m_curTakePicTime = m_nMaxContinuous;
        countTimer->start(m_nMaxInterval == 0 ? 34 : 1000);
    } else {
        emit takePicCancel();
        m_nInterval = m_nMaxInterval;
        m_curTakePicTime = m_nMaxContinuous;
        m_nInterval = 0; //下次可开启
        m_curTakePicTime = 0; //结束当前拍照
        if (countTimer->isActive()) {
            countTimer->stop();
        }
        if (flashTimer->isActive()) {
            flashTimer->stop();
        }
        if (m_fWgtCountdown->isVisible()) {
            m_fWgtCountdown->hide();
        }
        if (m_flashLabel->isVisible()) {
            m_flashLabel->hide();
        }
    }
}

void videowidget::onTakeVideo() //点一次开，再点一次关
{
    if (countTimer->isActive()) {
        countTimer->stop();
    }
    if (m_pCamErrItem->isVisible()) {
        m_pCamErrItem->hide();
    }
    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->hide();
    }
    if (m_nInterval > 0) { //倒计时期间的处理
        m_nInterval = 0; //下次可开启
        emit takeVdCancel(); //用于恢复缩略图
        return; //return即可，这个是外部过来的信号，外部有处理相关按钮状态、恢复缩略图状态
    }
    if (getCapstatus()) { //录制完成处理
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        setCapstatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
        return;
    }

    VIDEO_STATE = AUDIO;

    if (m_nMaxInterval == 0) {
        //直接录制
        showCountdown();
//        if (getCapstatus())
//            countTimer->start(1000);
    } else {
        //倒计时结束后录制
        m_nInterval = m_nMaxInterval;
        countTimer->start(1000);
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
    if (getCapstatus()) {
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        setCapstatus(false);
        reset_video_timer();
        emit updateBlockSystem(false);
    } else {
        if (g_devStatus == CAM_CANUSE) {
            qDebug() << "start takeVideo";
            g_strFileName = "UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".webm";
            m_nFileID ++;
            QString str = m_strFolder;
            if (QDir(m_strFolder).exists() == false) {
                m_strFolder = QDir::homePath() + QString("/Videos");
            }
            set_video_path(m_strFolder.toStdString().c_str());
            set_video_name(g_strFileName.toStdString().c_str());

            start_encoder_thread();
            emit updateBlockSystem(true);
            setCapstatus(true);
            //begin_time = QDateTime::currentDateTime();
            countTimer->stop();
            countTimer->start(1000); //重新计时，否则时间与显示时间
        } else {
            reset_video_timer();//重置底层定时器
            countTimer->stop();
            //countTimer->start(1000);
            setCapstatus(false);
        }
        m_nCount = 0;//5184000;
        m_btnVdTime->setText(QString("00:00:00"));

        int nWidth = this->width();
        int nHeight = this->height();

        m_endBtn->show();
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
    //关闭相关动作，倒计时、闪光灯、计时器、等等
    //countTimer flashTimer m_flashLabel
    if (flashTimer->isActive()) {
        flashTimer->stop();
    }
    if (countTimer->isActive()) {
        countTimer->stop();
    }
    if (m_flashLabel->isVisible()) {
        m_flashLabel->hide();
    }
    if (m_pCamErrItem->isVisible()) {
        m_pCamErrItem->hide();
    }
    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->hide();
    }
}
