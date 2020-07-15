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
//#include "LPF_V4L2.h"

#define MAX_REC_TIME 60 * 30 /*Maximum record time*/

static PRIVIEW_STATE VIDEO_STATE = NORMALVIDEO;
QString g_strFileName = nullptr;

videowidget::videowidget(DWidget *parent) : DWidget(parent)
{
    isFindedDevice = true;

    m_btnClickTime = QDateTime::currentDateTime();
    m_strFolder = "";
    m_nFileID = 0;
    char *tmp = new char[1024 * 720];
    if (tmp == nullptr) {
        int i = 0;
        i++;
    }
    m_nInterval = 0;
    m_curTakePicTime = 0;
    m_nCount = 0;
    //eff = new videoEffect;

    countTimer = new QTimer(this);
    connect(countTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));//默认

    flashTimer = new QTimer(this);
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(flash()));//默认

    m_pNormalView = new QGraphicsView(this);

    m_btnVdTime = new DPushButton(m_pNormalView);
    m_btnVdTime->setIcon(QIcon(":/images/icons/light/Timer Status.svg"));
    m_btnVdTime->setIconSize(QSize(6, 6));
    m_btnVdTime->hide(); //先隐藏
    m_btnVdTime->setFixedSize(84, 40);
    m_btnVdTime->setAttribute(Qt::WA_TranslucentBackground);
    m_btnVdTime->setEnabled(false);

    //设置字体颜色
    QPalette paletteTime = m_btnVdTime->palette();
    paletteTime.setColor(QPalette::ButtonText, QColor("#FF2C2C"));
    //paletteTime.setColor(QPalette::Button, Qt::transparent);
    m_btnVdTime->setPalette(paletteTime);

    DPalette pa_cb = DApplicationHelper::instance()->palette(m_btnVdTime);
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType() ) {
        pa_cb.setBrush(QPalette::Light, QColor(/*"#D2D2D2"*/255, 255, 255, 51)); //浅色
        //pa_cb.setBrush(QPalette::Dark, QColor(155, 155, 155, 230));
    } else {
        //pa_cb.setBrush(QPalette::Light, QColor(155, 155, 155, 230));
        pa_cb.setBrush(QPalette::Dark, QColor(/*"#202020"*/0, 0, 0, 51)); //深色
    }
    m_btnVdTime->setPalette(pa_cb);

    //设置模糊
//    QGraphicsBlurEffect *effect = new QGraphicsBlurEffect();
//    //effect.setBlurRadius(1.5);
//    effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
//    m_btnVdTime->setGraphicsEffect(effect);

//    与QPalette效果相同
//    m_btnVdTime->setStyleSheet(QString("background-color:rgba(155,155,155,230);border-style: outset;border-radius: 8px;"));

    m_btnVdTime->setFont(QFont("SourceHanSansSC", 10, QFont::ExtraLight));
    m_time.setHMS(0, 0, 0, 0);
    m_btnVdTime->setText(m_time.addSecs(m_nCount).toString("mm:ss"));

    m_endBtn = new DPushButton(m_pNormalView);
    m_endBtn->setFlat(true);
    m_endBtn->setFixedSize(QSize(56, 51));

    //    DPalette pa = m_endBtn->palette();
    //    QColor clo("#E5463D");
    //    pa.setColor(DPalette::Dark, clo);
    //    pa.setColor(DPalette::Light, clo);
    //    m_endBtn->setPalette(pa);
    m_endBtn->setIconSize(QSize(56, 51));
    m_endBtn->setIcon(QIcon(":/images/icons/light/Stop Recording.svg"));

    connect(m_endBtn, SIGNAL(clicked()), this, SLOT(endBtnClicked()));

    m_endBtn->setToolTip(tr("Stop taking video"));
    m_endBtn->setToolTipDuration(500); //0.5s消失
    m_endBtn->hide();

    m_fWgtCountdown = new DFloatingWidget(m_pNormalView);
    m_fWgtCountdown->hide(); //先隐藏
    m_fWgtCountdown->setFixedSize(160, 144);
    m_fWgtCountdown->setBlurBackgroundEnabled(true);
    m_dLabel = new DLabel(m_fWgtCountdown);
    m_dLabel->setAttribute(Qt::WA_TranslucentBackground);
    QPalette palette;
    palette.setColor(QPalette::Text, QColor("#000000"));
    m_dLabel->setPalette(palette);
    m_dLabel->setFont(QFont("SourceHanSansSC", 60, QFont::ExtraLight));
    m_dLabel->setAlignment(Qt::AlignCenter);

    m_pNormalScene = new QGraphicsScene;
    //禁用滚动条
    forbidScrollBar(m_pNormalView);

    m_pNormalView->setScene(m_pNormalScene);
    //m_pNormalScene->setSceneRect(0, 0, m_pNormalView->width(), m_pNormalView->height());

    m_pNormalView->setAttribute(Qt::WA_TranslucentBackground);
    //m_pNormalView->setWindowFlag(Qt::QGraphicsScene);

    m_pNormalItem = new QGraphicsPixmapItem;
    m_pCountItem = new QGraphicsTextItem;

    m_pTimeItem = new QGraphicsTextItem;
    m_pGridLayout = new QGridLayout(this);

    m_pGridLayout->setContentsMargins(0, 0, 0, 0);

    m_pGridLayout->addWidget(m_pNormalView);

    m_pNormalScene->addItem(m_pNormalItem);
    m_pNormalScene->addItem(m_pCountItem);
    m_pNormalScene->addItem(m_pTimeItem);
    qDebug() << "this widget--height:" << this->height() << "--width:" << this->width() << endl;
    qDebug() << "this widget--height:" << m_pNormalView->height() << "--width:" << m_pNormalView->width() << endl;
    qDebug() << "this widget--height:" << m_pNormalScene->height() << "--width:" << m_pNormalScene->width() << endl;
    init();
}

void videowidget::init()
{
    setCapstatus(false);

    m_imgPrcThread = new MajorImageProcessingThread;
    m_imgPrcThread->m_bTake = false;

    m_flashLabel.setWindowFlag(Qt::WindowType::ToolTip);
    m_flashLabel.hide();

    //启动视频
    if (camInit("/dev/video0") == E_OK) {
        //imageprocessthread->init();(/*ui->camComboBox->currentIndex()*/0);
        //QWaitCondition *condition  = new QWaitCondition();
        //mutex->lock();
        isFindedDevice = true;

        m_pCountItem->hide();
        m_imgPrcThread->start();
    } else {
        showNocam();
        qDebug() << "No webcam found" << endl;
    }

    connect(m_imgPrcThread, SIGNAL(SendMajorImageProcessing(QImage, int)),
            this, SLOT(ReceiveMajorImage(QImage, int)));

    connect(m_imgPrcThread, SIGNAL(reachMaxDelayedFrames()),
            this, SLOT(onReachMaxDelayedFrames()));

    connect(this, SIGNAL(sigFlash()), this, SLOT(flash()));
}

//显示没有设备的图片的槽函数
void videowidget::showNocam()
{
    DPalette paPic;
    QColor cloPic(0, 0, 0, 178);
    paPic.setColor(DPalette::Base, cloPic);
    setPalette(paPic);

    isFindedDevice = false;//没有设备
    int nWidth,nHeight;
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType() ) {
        QImage img(":/images/icons/light/Not connected.svg");
        nWidth = img.width();
        nHeight = img.height();
        m_pixmap = QPixmap::fromImage(img);
    } else {
        QImage img(":/images/icons/dark/Not connected_dark.svg");
        nWidth = img.width();
        nHeight = img.height();
        m_pixmap = QPixmap::fromImage(img);
    }

    //m_pixmap.fill(Qt::red);
    m_pNormalItem->setPixmap(m_pixmap);

    QString str(tr("No webcam found"));
    m_countdownLen = str.length() * 20;
    setFont(m_pCountItem, 12, str);
    m_pNormalItem->setPos((this->width()-nWidth)/2, (this->height()-nHeight)/2);
    m_pCountItem->setPos((this->width()-nWidth)/2 + 30, (this->height()-nHeight)/2 + nHeight + 10);
}

//显示设备被占用或者拔掉的图片的槽函数
void videowidget::showCamUsed()
{
    DPalette paPic;
    QColor cloPic(0, 0, 0, 178);
    paPic.setColor(DPalette::Base, cloPic);
    setPalette(paPic);

    isFindedDevice = false;//没有设备
    int nWidth,nHeight;
    if (DGuiApplicationHelper::LightType == DGuiApplicationHelper::instance()->themeType() ) {
        QImage img(":/images/icons/light/Take up.svg");
        nWidth = img.width();
        nHeight = img.height();
        m_pixmap = QPixmap::fromImage(img);
    } else {
        QImage img(":/images/icons/dark/Take up_dark.svg");
        nWidth = img.width();
        nHeight = img.height();
        m_pixmap = QPixmap::fromImage(img);
    }
    m_pNormalItem->setPixmap(m_pixmap);

    QString str(tr("The webcam is in use"));//摄像头已被占用
    m_countdownLen = str.length() * 20;
    setFont(m_pCountItem, 12, str);
    m_pNormalItem->setPos((this->width()-nWidth)/2, (this->height()-nHeight)/2);
    m_pCountItem->show();
    m_pCountItem->setPos((this->width()-nWidth)/2 + 30, (this->height()-nHeight)/2 + nHeight + 10);
}

void videowidget::ReceiveMajorImage(QImage image, int result)
{
    //超时后关闭视频
    //超时代表着VIDIOC_DQBUF会阻塞，直接关闭视频即可
    if (result == -1) {
        m_imgPrcThread->wait();

        QString str = "获取设备图像超时！";
        m_countdownLen = str.length() * 20;
        setFont(m_pCountItem, 20, str);
        m_pCountItem->setPlainText(str);
    }

    if (!image.isNull()) {
        switch (result) {
        case 0:     //Success
            err11 = err19 = 0;
            if (image.isNull()) {
                QString str = "画面丢失！";
                m_countdownLen = str.length() * 20;
                setFont(m_pCountItem, 20, str);
                m_pCountItem->setPlainText(str);
            }
            m_imgPrcThread->m_rwMtxImg.lock();
            m_imgPrcThread->m_img = m_imgPrcThread->m_img.scaled(this->width(), this->height());
            m_pixmap = QPixmap::fromImage(m_imgPrcThread->m_img);
            m_imgPrcThread->m_rwMtxImg.unlock();

            m_pNormalItem->setPixmap(m_pixmap);
            break;
        case 11:
            err11++;
            if (err11 == 10) {
                QString str = "设备已打开，但获取视频失败！\n请尝试切换USB端口后断开重试！";
                m_countdownLen = str.length() * 20;
                setFont(m_pCountItem, 20, str);
                m_pCountItem->setPlainText(str);
            }
            break;
        case 19:
            err19++;
            if (err19 == 10) {
                QString str = "设备丢失！";
                m_countdownLen = str.length() * 20;
                setFont(m_pCountItem, 20, str);
                m_pCountItem->setPlainText(str);
            }
            break;
        }
    }
}

void videowidget::onReachMaxDelayedFrames()
{
    showCamUsed();
    emit setBtnStatues(false);
}

void videowidget::showCountDownLabel(PRIVIEW_STATE state)
{
    //qDebug() << "countDown" << m_nInterval;
    QString str;
    switch (state) {
    case NORMALVIDEO:
        //no device found

        m_fWgtCountdown->move((this->width() - m_fWgtCountdown->width()) / 2,
                              (this->height() - m_fWgtCountdown->height()) / 2);

        m_fWgtCountdown->show();

        //m_pCountItem->show();
        m_pTimeItem->hide();
        m_btnVdTime->hide();
        m_endBtn->hide();

        m_countdownLen = 50;
        setFont(m_pCountItem, 50, QString::number(m_nInterval));
        m_dLabel->move((m_fWgtCountdown->width() - m_dLabel->width()) / 2,
                       (m_fWgtCountdown->height() - m_dLabel->height()) / 2);
        m_dLabel->setText(QString::number(m_nInterval));
        resizePixMap();
        break;
    case AUDIO:
        m_pCountItem->hide();
        m_fWgtCountdown->hide();
        if (!get_capture_pause())//判断是否是暂停状态
            m_btnVdTime->setText(m_time.addSecs(m_nCount++).toString("mm:ss"));
        if (m_nCount >= MAX_REC_TIME) {
            endBtnClicked(); //结束录制
        }
        resizePixMap();
        break;
    case SHOOT:
        m_fWgtCountdown->move((this->width() - m_fWgtCountdown->width()) / 2,
                              (this->height() - m_fWgtCountdown->height()) / 2);

        m_fWgtCountdown->show();
        m_pTimeItem->hide();
        m_btnVdTime->hide();
        m_endBtn->hide();
        str = QString::number(m_nInterval);
        setFont(m_pCountItem, 50, str);

        break;
    default:
        m_pTimeItem->hide();
        m_pCountItem->hide();
        m_fWgtCountdown->hide();
        m_btnVdTime->hide();
        m_endBtn->hide();
        break;
    }
}

void videowidget::setFont(QGraphicsTextItem *item, int size, QString str)
{
    if (item == nullptr)
        return;
    item->setFont(QFont("SourceHanSansSC", size, QFont::ExtraLight));
    item->setDefaultTextColor(QColor(40, 39, 39));
    //item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    item->setPlainText(str);

}
void videowidget::hideCountDownLabel()
{
    //关闭
    m_pCountItem->hide();
    m_fWgtCountdown->hide();
}

void videowidget::hideTimeLabel()
{
    m_pTimeItem->hide();
    m_btnVdTime->hide();
    m_endBtn->hide();
}

void videowidget::resizePixMap()
{
    if (m_pCountItem->isVisible()) {
        QRect rect = this->rect();
        m_pNormalScene->setSceneRect(rect);
        int x = this->x();
        int y = this->y();

        m_pCountItem->setX(x + rect.width() / 2 - m_countdownLen / 2);
        m_pCountItem->setY(y + rect.height() / 2 - 100); //设置高度的一半
    }
    if (m_pTimeItem->isVisible()) {
        QRect rect = this->rect();
        m_pNormalScene->setSceneRect(rect);
        int x = this->x();
        int y = this->y();

        m_pTimeItem->setX(x + rect.width() / 2 - 50);
        m_pTimeItem->setY(y + rect.height());
    }
}

void videowidget::resizeEvent(QResizeEvent *size)
{
    Q_UNUSED(size);
    int nWidth = this->width();
    int nHeight = this->height();

    if (m_endBtn) {
        m_endBtn->move((nWidth + m_btnVdTime->width() + 10 - m_endBtn->width()) / 2,
                       nHeight - m_btnVdTime->height() - 10);
    }

    if (m_btnVdTime) {
        m_btnVdTime->move((nWidth - m_btnVdTime->width() - 10 - m_endBtn->width()) / 2,
                          nHeight - m_btnVdTime->height() - 5);
    }
//    m_imgPrcThread->m_rwMtxImg.lock();
//    m_imgPrcThread->m_img = m_imgPrcThread->m_img.scaled(nWidth, nHeight);
//    m_pixmap = QPixmap::fromImage(m_imgPrcThread->m_img);
//    m_imgPrcThread->m_rwMtxImg.unlock();
//    m_pNormalItem->setPixmap(m_pixmap);
//    repaint();
    //    return DWidget::resizeEvent(size);
}

void videowidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
}

void videowidget::showCountdown()
{
    if (m_imgPrcThread->getStatus()) {//拍照拍摄时拔出摄像头的异常处理
        if (flashTimer->isActive()) {
            flashTimer->stop();
        }
        if (countTimer->isActive()) {
            countTimer->stop();
        }
        if (VIDEO_STATE == AUDIO) {
            if (getCapstatus()) {//录制中
                endBtnClicked();
            }
            else {
                emit takeVdCancel();
            }
        }
        if (VIDEO_STATE == NORMALVIDEO) {
            onTakePic();
            emit takePicDone();
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
                flashTimer->start(500);

                m_flashLabel.resize(this->size());
                m_flashLabel.move(this->mapToGlobal(this->pos()));
                qDebug() << "****m_flashLabel.show();";
                m_flashLabel.show();
            }
            //发送就结束信号处理按钮状态
            countTimer->stop();

            m_imgPrcThread->m_strPath = m_strFolder + "/UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".jpg";
            m_imgPrcThread->m_bTake = true; //保存图片

            m_nFileID++;
            if (--m_curTakePicTime == 0) {
                //拍照结束，恢复按钮状态和缩略图标志位
                emit takePicDone();
            }

            if (m_curTakePicTime > 0) {
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
                    flashTimer->start(500);
                }
            }

            m_nInterval--;
        }
    }
}

void videowidget::flash()
{
    if (m_flashLabel.isVisible()) {
        //隐藏闪光窗口
        qDebug() << "****m_flashLabel.hide();";
        m_flashLabel.hide(); //为避免没有关闭，放到定时器里边关闭
        flashTimer->stop();
    } else {
        m_flashLabel.resize(this->size());
        m_flashLabel.move(this->mapToGlobal(this->pos()));
        qDebug() << "****m_flashLabel.show();";
        m_flashLabel.show();
        if (flashTimer->isActive()) { //连续点击拍照
            flashTimer->stop();
        }
        flashTimer->start(500);
    }
}

void videowidget::endBtnClicked()
{
    if (countTimer->isActive()) {
        countTimer->stop();
    }
    if (m_pCountItem->isVisible() && !m_imgPrcThread->getStatus()) {
        m_pCountItem->hide();
    }
    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->hide();
    }

    m_btnVdTime->hide();
    m_endBtn->hide();
    VIDEO_STATE = NORMALVIDEO;
    g_strFileName = nullptr;
    if (getCapstatus()) { //录制完成处理
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        setCapstatus(false);
        reset_video_timer();
    }
    emit takeVdCancel();
}

void videowidget::restartDevices()
{
    if (isFindedDevice == false) {
        isFindedDevice = true;
        m_pNormalItem->setPos(0, 0);
        changeDev();
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
                m_pCountItem->hide();
                if (camInit(devlist->list_devices[i].device) == E_OK) {
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                }
                break;
            }
        }
    } else {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (str == str1) {
                if (i == devlist->num_devices - 1) {
                    m_pCountItem->hide();
                    if (camInit(devlist->list_devices[0].device) == E_OK) {
                        m_imgPrcThread->init();
                        m_imgPrcThread->start();
                    }
                    break;
                } else {
                    m_pCountItem->hide();
                    if (camInit(devlist->list_devices[i + 1].device) == E_OK) {
                        m_imgPrcThread->init();
                        m_imgPrcThread->start();
                    }
                    break;
                }
            }
            if (str.isEmpty() == true) {
                m_pCountItem->hide();
                if (camInit(devlist->list_devices[0].device) == E_OK) {
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                }
                break;
            }
        }
    }
}

void videowidget::onTakePic()
{
    if (m_fWgtCountdown) {
        m_fWgtCountdown->move((this->width() - m_fWgtCountdown->width()) / 2,
                              (this->height() - m_fWgtCountdown->height()) / 2);
    }
    if (m_nMaxInterval == 0) {
        m_nInterval = m_nMaxInterval;
        m_curTakePicTime = m_nMaxContinuous;
        countTimer->start(m_nMaxInterval == 0 ? 34 : 1000); //最小时间大于一帧
        return;
    }
    if (countTimer->isActive()) {
        countTimer->stop();
    }
    if (m_pCountItem->isVisible() && !m_imgPrcThread->getStatus()) {
        m_pCountItem->hide();
    }
    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->hide();
    }
    if (m_nInterval > 0) { //倒计时期间的处理
        m_nInterval = 0; //下次可开启
        m_curTakePicTime = 0; //结束当前拍照
        return; //return即可，这个是外部过来的信号，外部有处理相关按钮状态、恢复缩略图状态
    }
    VIDEO_STATE = NORMALVIDEO;
    if (countTimer->isActive()) { //连续点击拍照
        countTimer->stop();
    }
    m_nInterval = m_nMaxInterval;
    m_curTakePicTime = m_nMaxContinuous;
    countTimer->start(m_nMaxInterval == 0 ? 34 : 1000);
}

void videowidget::onTakeVideo() //点一次开，再点一次关
{
    if (countTimer->isActive()) {
        countTimer->stop();
    }
    if (m_pCountItem->isVisible()) {
        m_pCountItem->hide();
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
        return;
    }

    VIDEO_STATE = AUDIO;

    if (m_nMaxInterval == 0) {
        //直接录制
        showCountdown();
        if (getCapstatus())
            countTimer->start(1000);
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

    } else {
        if (get_v4l2_device_handler()) {
            qDebug() << "start takeVideo";
            g_strFileName = "UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".webm";
            QString str = m_strFolder;
            set_video_path(m_strFolder.toStdString().c_str());
            set_video_name(g_strFileName.toStdString().c_str());

            start_encoder_thread();
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
        m_nCount = 0;
        m_time.setHMS(0, 0, 0, 0);
        m_btnVdTime->setText(m_time.toString("mm:ss"));

        int nWidth = this->width();
        int nHeight = this->height();

        m_endBtn->show();
        m_btnVdTime->show();

        m_btnVdTime->move((nWidth - m_btnVdTime->width() - 10 - m_endBtn->width()) / 2,
                          nHeight - m_btnVdTime->height() - 5);

        m_endBtn->move((nWidth + m_btnVdTime->width() + 10 - m_endBtn->width()) / 2,
                       nHeight - m_btnVdTime->height() - 10);
    }
}
