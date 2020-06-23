/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
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
#include "myscene.h"
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
//#include "LPF_V4L2.h"
#define MAX_REC_TIME 60 * 30 /*Maximum record time*/

videowidget::videowidget(DWidget *parent) : DWidget(parent)
{
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
    eff = new videoEffect;

    countTimer = new QTimer(this);
    connect(countTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));//默认

    flashTimer = new QTimer(this);
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(flash()));//默认

    m_pNormalView = new QGraphicsView(this);

    m_fWgtTime = new DFloatingWidget(m_pNormalView);
    m_fWgtTime->hide(); //先隐藏
    m_fWgtTime->setFixedSize(84, 40);
    m_fWgtTime->setBlurBackgroundEnabled(true);
    m_fWgtTime->setFramRadius(1);

    m_dLabelVdTime = new DLabel(m_fWgtTime); //有个圆点，待处理
    m_dLabelVdTime->setFixedWidth(84);
    m_dLabelVdTime->setAttribute(Qt::WA_TranslucentBackground);
    QPalette paletteTime;
    paletteTime.setColor(QPalette::Text, QColor("#FF2C2C"));
    m_dLabelVdTime->setPalette(paletteTime);
    m_dLabelVdTime->setFont(QFont("思源黑体 ExtraLight", 10, QFont::ExtraLight));
    m_dLabelVdTime->setAlignment(Qt::AlignCenter);
    m_time.setHMS(0, 0, 0, 0);
    m_dLabelVdTime->setText(m_time.addSecs(m_nCount).toString("mm:ss"));

    m_fWgtBtn = new DFloatingWidget(m_pNormalView);
    m_fWgtBtn->setFixedSize(QSize(36, 40));

    m_endBtn = new DPushButton(m_fWgtBtn);
    m_endBtn->setFixedSize(QSize(36, 36));
    DPalette pa = m_endBtn->palette();
    QColor clo("#FF2C2C");
    pa.setColor(DPalette::Dark, clo);
    pa.setColor(DPalette::Light, clo);
    m_endBtn->setPalette(pa);
    connect(m_endBtn, SIGNAL(clicked()), this, SLOT(endBtnClicked()));

    m_endBtn->setToolTip(tr("Stop taking photos"));
    m_endBtn->setToolTipDuration(500); //0.5s消失
    m_fWgtBtn->hide();

    m_fWgtCountdown = new DFloatingWidget(m_pNormalView);
    m_fWgtCountdown->hide(); //先隐藏
    m_fWgtCountdown->setFixedSize(160, 144);
    m_fWgtCountdown->setBlurBackgroundEnabled(true);
    m_dLabel = new DLabel(m_fWgtCountdown);
    m_dLabel->setAttribute(Qt::WA_TranslucentBackground);
    QPalette palette;
    palette.setColor(QPalette::Text, QColor("#000000"));
    m_dLabel->setPalette(palette);
    m_dLabel->setFont(QFont("思源黑体 ExtraLight", 60, QFont::ExtraLight));
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
    showPreviewByState(NORMALVIDEO);
}

void videowidget::init()
{
    m_bActive = false;

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
        //imageprocessthread->init();
        m_imgPrcThread->start();

    } else {
        isFindedDevice = false;

        showNocam();
        qDebug() << "No webcam found" << endl;
    }

    connect(m_imgPrcThread, SIGNAL(SendMajorImageProcessing(QImage, int)),
            this, SLOT(ReceiveMajorImage(QImage, int)));

    connect(m_imgPrcThread, SIGNAL(reachMaxDelayedFrames()),
            this, SLOT(onReachMaxDelayedFrames()));

    connect(this, SIGNAL(sigFlash()), this, SLOT(flash()));
}

void videowidget::showNocam()
{
    DPalette paPic;
    QColor cloPic(0, 0, 0, 178);
    paPic.setColor(DPalette::Base, cloPic);
    setPalette(paPic);

    QImage img(":/images/icons/Not connected.svg");
    m_pixmap = QPixmap::fromImage(img);
    //m_pixmap.fill(Qt::red);
    m_pNormalItem->setPixmap(m_pixmap);

    QString str = "No webcam found";
    m_countdownLen = str.length() * 20;
    setFont(m_pCountItem, 12, str);
    m_pNormalItem->setPos(100, -200);
    m_pCountItem->setPos(100, -80);
}

void videowidget::showCamUsed()
{
    DPalette paPic;
    QColor cloPic(0, 0, 0, 178);
    paPic.setColor(DPalette::Base, cloPic);
    setPalette(paPic);

    QImage img(":/images/icons/Take up.svg");
    //    img = img.scaled(img.size());
    m_pixmap = QPixmap::fromImage(img);

    m_pNormalItem->setPixmap(m_pixmap);

    QString str = "The webcam is in use";
    m_countdownLen = str.length() * 20;
    setFont(m_pCountItem, 12, str);
    m_pNormalItem->setPos(320, 100);
    m_pCountItem->show();
    m_pCountItem->setPos(330, 240);

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
}

void videowidget::sceneAddItem()
{

}

void videowidget::updateEffectName()
{

}
void videowidget::showPreviewByState(PRIVIEW_STATE state)
{
    switch (state) {
    case NODEVICE:
    //创建黑屏效果
    case  NORMALVIDEO:
    case AUDIO:
    case SHOOT:
        m_pGridLayout->addWidget(m_pNormalView, 0, 0);

        m_pNormalView->show();
        this->setLayout(m_pGridLayout);
        break;

    case EFFECT:
        //现实九宫格效果
        m_pNormalView->hide();
        int x, y;


        m_pGridLayout->removeWidget(m_pNormalView); //需要remove？？
        //m_pGridLayout->setAlignment(m_VEffectPreview[1], Qt::AlignJustify | Qt::AlignBaseline);
        //m_pGridLayout->setVerticalSpacing(1);

        //end
        this->setLayout(m_pGridLayout);
        break;

    default:
        break;
    }
}

void videowidget::transformImage(QImage *img)
{
    if (img == NULL || img->isNull())
        return;
    float wImg = img->width();
    float hImg = img->height();

    if (wImg == 0 || hImg == 0) {
        return;
    }

    //声明一个QMatrix类的实例
    QMatrix martix;
    if (wImg <= hImg) {
        martix.scale(hImg / wImg * 4 / 3, 1);
    } else {
        martix.scale(1, wImg / hImg * 3 / 4);
    }
    *img = img->transformed(martix);
    resizeImage(img);
}

void videowidget::resizeImage(QImage *img)
{
    //*img = img->scaled(this->width(),this->height());
    //    float wImg = img->width();
    //    float hImg = img->height();
    //    float  wLab = this->width();
    //    float hLab = this->height();
    //    if (STATE == EFFECT) {
    //    } else {
    //        wLab = this->width();
    //        hLab = this->height();
    //    }

    //    if (wImg == 0 || hImg == 0 || wLab == 0 || hLab == 0) {
    //        return;
    //    }

    //    //声明一个QMatrix类的实例
    //    QMatrix martix;

    //    //取小边
    //    bool    isW = wLab * 3 / 4 <= hLab;
    //    float timeW = isW  ? wLab / img->width() : hLab / img->height();

    //    martix.scale(timeW, timeW);

    //    *img = img->transformed(martix);
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
        m_fWgtTime->hide();
        m_fWgtBtn->hide();
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
        if (!get_capture_pause())
            m_dLabelVdTime->setText(m_time.addSecs(m_nCount++).toString("mm:ss"));
        if (m_nCount >= MAX_REC_TIME) {
            endBtnClicked(); //结束录制
        }
        resizePixMap();
        break;
    case SHOOT:
        m_fWgtCountdown->move((this->width() - m_fWgtCountdown->width()) / 2,
                              (this->height() - m_fWgtCountdown->height()) / 2);

        //m_pCountItem->show();
        m_fWgtCountdown->show();
        m_pTimeItem->hide();
        m_fWgtTime->hide();
        m_fWgtBtn->hide();
        str = QString::number(m_nInterval);
        setFont(m_pCountItem, 50, str);
        //m_pNormalScene->addItem(m_pCountItem);
        break;
    default:
        m_pTimeItem->hide();
        m_pCountItem->hide();
        m_fWgtCountdown->hide();
        m_fWgtTime->hide();
        m_fWgtBtn->hide();
        break;
    }
}

void videowidget::setFont(QGraphicsTextItem *item, int size, QString str)
{
    if (item == nullptr)
        return;
    item->setFont(QFont("思源黑体 ExtraLight", size, QFont::ExtraLight));
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
    m_fWgtTime->hide();
    m_fWgtBtn->hide();
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
    //resizePixMap();

    return DWidget::resizeEvent(size);

}

//void videowidget::paintEvent(QPaintEvent *event)
//{
//    if (m_fWgtCountdown) {
//        QPainter painter(this);
//        painter.setPen(QColor(0, 160, 230));
//        QFont font;
//        font.setFamily("Microsoft YaHei");
//        font.setPointSize(16);
//        painter.setFont(font);
////        QTransform transform;
////        transform.rotate(45);
////        painter.setTransform(transform);
//        painter.drawText(rect(), QStringLiteral("3"));
//    }

//    return DWidget::paintEvent(event);

//}

void videowidget::showCountdown()
{
    //qDebug() << "showCountdown";
    //显示倒数，m_nMaxInterval秒后结束，并拍照
    if (m_nInterval == 0) {
        if (VIDEO_STATE == AUDIO) {
            if (!m_bActive) {
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
    if (m_pCountItem->isVisible()) {
        m_pCountItem->hide();
    }
    if (m_fWgtCountdown->isVisible()) {
        m_fWgtCountdown->hide();
    }

    m_fWgtTime->hide();
    m_fWgtBtn->hide();
    if (m_bActive) { //录制完成处理
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        m_bActive = false;
        reset_video_timer();
    }
    emit takeVdCancel();
}

void videowidget::changeDev()
{
    v4l2_dev_t *vd =  get_v4l2_device_handler();
    if (m_imgPrcThread != nullptr) {
        m_imgPrcThread->stop();
    }
    while (m_imgPrcThread->isRunning())
        ;
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
                camInit(devlist->list_devices[i].device);
                m_imgPrcThread->init();
                m_imgPrcThread->start();
                break;
            }
        }
    } else {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (str == str1) {
                if (i == devlist->num_devices - 1) {
                    m_pCountItem->hide();
                    camInit(devlist->list_devices[0].device);
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                    break;
                } else {
                    m_pCountItem->hide();
                    camInit(devlist->list_devices[i + 1].device);
                    m_imgPrcThread->init();
                    m_imgPrcThread->start();
                    break;
                }
            }
            if (str.isEmpty() == true) {
                m_pCountItem->hide();
                camInit(devlist->list_devices[0].device);
                m_imgPrcThread->init();
                m_imgPrcThread->start();
                break;
            }
        }
    }
}

void videowidget::onTakePic() //待解决高频单张拍照问题，因为闪屏导致无法继续
{
    //    if (m_bWgtTime) {
    //        m_bWgtTime->move((this->width() - m_bWgtTime->width()) / 2,
    //                          this->height() - m_bWgtTime->height() - 80);
    //    }
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
    if (m_pCountItem->isVisible()) {
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
    if (m_bActive) { //录制完成处理
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        m_bActive = false;
        reset_video_timer();
        return;
    }

    VIDEO_STATE = AUDIO;

    if (m_nMaxInterval == 0) {
        //直接录制
        showCountdown();
        countTimer->start(1000);
    } else {
        //倒计时结束后录制
        m_nInterval = m_nMaxInterval;
        countTimer->start(1000);
    }
}

void videowidget::onTakeVideoOver()
{
    qDebug() << "onTakeVideoOver";
    VIDEO_STATE = NORMALVIDEO;
    countTimer->stop();
    hideTimeLabel();
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
    if (m_bActive) {
        qDebug() << "stop takeVideo";
        stop_encoder_thread();
        m_bActive = false;
        reset_video_timer();

    } else {
        qDebug() << "start takeVideo";
        m_strFileName = "/UOS_" + QDateTime::currentDateTime().toString("yyyyMMddHHmmss") + "_" + QString::number(m_nFileID) + ".mp4";
        set_video_path(m_strFolder.toStdString().c_str());
        set_video_name(m_strFileName.toStdString().c_str());
        start_encoder_thread();
        m_bActive = true;
        begin_time = QDateTime::currentDateTime();
        countTimer->stop();
        countTimer->start(1000); //重新计时，否则时间与显示时间

        m_fWgtTime->show();
        m_time.setHMS(0, 0, 0, 0);
        m_nCount = 0;
        m_dLabelVdTime->setText(m_time.toString("mm:ss"));
        m_fWgtTime->move((this->width() - m_fWgtTime->width()) / 2,
                         this->height() - m_fWgtTime->height() - 5);
        m_dLabelVdTime->move((m_fWgtTime->width() - m_dLabelVdTime->width()) / 2,
                             (m_fWgtTime->height() - m_dLabelVdTime->height()) / 2);
        m_fWgtBtn->show();
        m_fWgtBtn->move((this->width() - m_fWgtTime->width()) / 2 + 20 + 84,
                        this->height() - m_fWgtTime->height() - 5);
        qDebug() << m_fWgtTime->width() << "******";
    }
}
