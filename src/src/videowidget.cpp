/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
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

#include "videowidget.h"
#include "myscene.h"
#include <QPixmap>
#include <QTimer>
#include <QGraphicsView>
#include <QDateTime>
#include <QGraphicsPixmapItem>
#include <QVBoxLayout>
#include <QThread>
#include <QScrollBar>
#include "LPF_V4L2.h"

videowidget::videowidget(DWidget *parent) : DWidget(parent)
{

    char *tmp = new char[1024 * 720];
    if (tmp == nullptr) {
        int i = 0;
        i++;
    }
    countDown = 3;
    m_curTakePicTime = 0;
    eff = new videoEffect;
    //labTimer = new DLabel;

    countTimer = new QTimer(this);
    connect(countTimer, SIGNAL(timeout()), this, SLOT(showCountdown()));//默认

    flashTimer = new QTimer(this);
    connect(flashTimer, SIGNAL(timeout()), this, SLOT(flash()));//默认

    m_pNormalView = new QGraphicsView(this);
    m_pNormalScene = new QGraphicsScene;
    //禁用滚动条
    forbidScrollBar(m_pNormalView);

    m_pNormalView->setScene(m_pNormalScene);
    m_pNormalItem = new QGraphicsPixmapItem;
    m_pCountItem = new QGraphicsTextItem;

    m_pTimeItem = new QGraphicsTextItem;
    m_pGridLayout = new QGridLayout(this);
    m_pGridLayout->setHorizontalSpacing(10);
    m_pGridLayout->setVerticalSpacing(10);
    m_pGridLayout->setContentsMargins(10, 10, 10, 10);


    m_pNormalScene->addItem(m_pNormalItem);
    m_pNormalScene->addItem(m_pCountItem);
    m_pNormalScene->addItem(m_pTimeItem);

    init();
    // by xxj
    newEffectPreview();
    sceneAddItem();
    //end
    showPreviewByState(NORMALVIDEO);

}

void videowidget::init()
{
    err11 = err19 = 0;
    m_bTakePic = false;
    imageprocessthread = new MajorImageProcessingThread;
    labTimer.showFullScreen();
    labTimer.setWindowFlag(Qt::WindowType::ToolTip);
    labTimer.hide();

    //启动视频
    if (camInit("/dev/video0") == E_OK) {
        //imageprocessthread->init();(/*ui->camComboBox->currentIndex()*/0);
        //QWaitCondition *condition  = new QWaitCondition();
        //mutex->lock();

        isFindedDevice = true;
        m_pCountItem->hide();
        imageprocessthread->init();
        imageprocessthread->start();

    } else {
        isFindedDevice = false;
        m_pCountItem->show();
        QString str = "no device found";
        m_countdownLen = str.length() * 20;
        setFont(m_pCountItem, 20, str);
        m_pNormalScene->addItem(m_pCountItem);
    }


    connect(imageprocessthread, SIGNAL(SendMajorImageProcessing(QImage, int)),
            this, SLOT(ReceiveMajorImage(QImage, int)));

    connect(this, SIGNAL(sigFlash()), this, SLOT(flash()));
}

void videowidget::ReceiveMajorImage(QImage image, int result)
{
    //超时后关闭视频
    //超时代表着VIDIOC_DQBUF会阻塞，直接关闭视频即可
    if (result == -1) {
        imageprocessthread->wait();

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

            CURRENT_IMAGE = &image;

            changePicture(STATE, &image, EFFECT_INDEX);

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

void videowidget::sceneAddItem()
{
    if (m_VEffectScene.length() == 0 || m_VEffectPreview.length() == 0) {
        qDebug() << "view/scene/item vector length cannot =0";
        return;
    }
    //int x, y;
    //加入图片,name
    for (int i = 0; i < m_VEffectScene.length(); ++i) {
        if (m_VPixmapItem.length() > i && m_VEffectName.length() > i) {
            m_VEffectScene[i]->addItem(m_VPixmapItem[i]);
            m_VEffectName[i]->setPlainText(NAME[i + EFFECT_PAGE * 9]);
            m_VEffectName[i]->setTextWidth(50);
            m_VEffectScene[i]->addItem(m_VEffectName[i]);

        } else {
            qDebug() << "this item is not correct init";
        }
    }
}

void videowidget::updateEffectName()
{
    for (int i = 0 ; i < 9 && EFFECT_PAGE <= 3 && EFFECT_PAGE >= 0; ++i) {
        //读取配置
        int index = i + EFFECT_PAGE * 9;
        if (index < EFFECTS_NUM) {
            m_VEffectName[i]->setPlainText(NAME[index]);
            m_VEffectScene[i]->setSceneName(NAME[index]);
        }
    }
}
void videowidget::showPreviewByState(PRIVIEW_STATE state)
{
    switch (state) {
    case NODEVICE:
    //创建黑屏效果
    case  NORMALVIDEO:
    case AUDIO:
    case SHOOT:
        //m_pGridLayout->addWidget(m_pNormalView, 0, Qt::AlignCenter);

        //m_pNormalView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        for (int i = 0; i < m_VEffectPreview.length(); ++i) {
            m_VEffectPreview[i]->hide();
            m_pGridLayout->removeWidget(m_VEffectPreview[i]);
        }
        m_pGridLayout->addWidget(m_pNormalView, 0, 0);

        m_pNormalView->show();
        this->setLayout(m_pGridLayout);
        break;

    case EFFECT:
        //现实九宫格效果
        m_pNormalView->hide();
        int x, y;


        m_pGridLayout->removeWidget(m_pNormalView); //需要remove？？
        //by xxj
        for (int i = 0; i < m_VEffectPreview.length(); ++i) {
            m_VEffectPreview[i]->show();

            m_pGridLayout->addWidget(m_VEffectPreview[i], i / 3, i % 3);
            QRect rect = m_VEffectPreview[i]->childrenRect();
            x = rect.width();
            y = rect.height();
        }
        //m_pGridLayout->setAlignment(m_VEffectPreview[1], Qt::AlignJustify | Qt::AlignBaseline);
        //m_pGridLayout->setVerticalSpacing(1);

        //end
        this->setLayout(m_pGridLayout);
        break;

    default:
        break;
    }
}

void videowidget::changePicture(PRIVIEW_STATE state,  QImage *img, int effectIndex = 0)
{
    if (img == nullptr || img->isNull()) {
        return;
    }
    transformImage(img);

    if (state == EFFECT) {

        for (int i = 0 ; i < m_VEffectScene.length(); ++i) {
            QImage *image = new QImage(*img);
            eff->chooseEffect(image, i + EFFECT_PAGE * 9);

            m_VPixmapItem[i]->setPixmap(QPixmap::fromImage(*image));
        }
    } else {
        eff->chooseEffect(img, effectIndex);
        //m_VPixmapItem[0]->setPixmap();
        m_pNormalItem->setPixmap(QPixmap::fromImage(*img));
    }
    //m_pPixmapItem->setTransformationMode(Qt::SmoothTransformation);

    //保存图片
    if (m_bTakePic) {
        QDateTime end_time;
        end_time = QDateTime::currentDateTime();             //获取或设置时间
        QString strTime = end_time.toString("yyyy-MM-dd-HHMMss");
        CURRENT_IMAGE->save((QString(getenv("HOME")) + "/Pictures/摄像头/" + strTime + ".jpg"), "jpg", 100);
        countDown = 3;
        //隐藏闪光窗口
        labTimer.hide();
        m_bTakePic = false;

        if (m_curTakePicTime > 1) {
            m_curTakePicTime --;
            onShowCountdown();
        }
    }
    resizeEvent(NULL);//消耗怎么样？？？？？
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
    //resizeImage(img);
}

void videowidget::resizeImage(QImage *img)
{
    float wImg = img->width();
    float hImg = img->height();
    float  wLab = this->width();
    float hLab = this->height();
    if (STATE == EFFECT) {
        wLab = m_VEffectPreview[0]->width();
        hLab = m_VEffectPreview[0]->height();
    } else {
        wLab = this->width();
        hLab = this->height();
    }


    if (wImg == 0 || hImg == 0 || wLab == 0 || hLab == 0) {
        return;
    }

    //声明一个QMatrix类的实例
    QMatrix martix;

    //取小边
    bool    isW = wLab * 3 / 4 <= hLab;
    float timeW = isW  ? wLab / img->width() : hLab / img->height();

    martix.scale(timeW, timeW);

    *img = img->transformed(martix);
}

void videowidget::showCountDownLabel(PRIVIEW_STATE state)
{
    qDebug() << "countDown" << countDown;
    QDateTime end_time;
    QTime m_time;
    QString str;
    switch (state) {
    case NORMALVIDEO:
        //no device found
        m_pCountItem->show();
        m_pTimeItem->hide();
        m_countdownLen = 50;
        setFont(m_pCountItem, 50, QString::number(countDown));
        //m_pNormalScene->addItem(m_pCountItem);
        //m_pNormalScene->addItem(m_pCountItem);
        break;
    case AUDIO:
        m_pTimeItem->show();
        m_pCountItem->hide();
        end_time = QDateTime::currentDateTime();             //获取或设置时间
        m_time.setHMS(0, 0, 0, 0);                                       //初始化数据，时 分 秒 毫秒
        str = m_time.addSecs(begin_time.secsTo(end_time)).toString("hh:mm:ss");//计算时间差(秒)，将时间差加入m_time，格式化输出
        setFont(m_pTimeItem, 20, str);
        //m_pNormalScene->addItem(m_pTimeItem);
        break;
    case SHOOT:
        m_pCountItem->show();
        m_pTimeItem->hide();
        str = QString::number(countDown);
        setFont(m_pCountItem, 50, str);
        //m_pNormalScene->addItem(m_pCountItem);
        break;
    default:
        m_pTimeItem->hide();
        m_pCountItem->hide();
        break;
    }
    resizeEvent(NULL);
}

void videowidget::setFont(QGraphicsTextItem *item, int size, QString str)
{
    if (item == nullptr)
        return;
    item->setFont(QFont("华文琥珀", size));
    item->setDefaultTextColor(QColor(255, 255, 255));
    //item->setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
    item->setPlainText(str);

}
void videowidget::hideCountDownLabel()
{
    //关闭
    m_pCountItem->hide();
    //m_pNormalScene->removeItem(m_pCountItem);
}

void videowidget::hideTimeLabel()
{
    m_pTimeItem->hide();
    //m_pNormalScene->removeItem(m_pTimeItem);
}

void videowidget::newEffectPreview()
{
    //创建九宫格布局
    for (int i = 0 ; i < 9; ++i) {
        QGraphicsView *view = new QGraphicsView();
        forbidScrollBar(view);
        m_VEffectPreview.push_back(view);
    }
    for (int j = 0 ; j < 9; ++j) {
        MyScene *scene = new MyScene;
        scene->setSceneName(NAME[j + EFFECT_PAGE * 9]);
        m_VEffectScene.push_back(scene);

// //by xxj
//        //加入布局
//        int x = j / 3;
//        int y = j % 3;
//        m_pGridLayout->addWidget(m_VEffectPreview[j], x, y);
// end
        //设置场景
        m_VEffectPreview[j]->setScene(scene);

        m_VEffectPreview[j]->setStyleSheet("background:rgb(255, 255, 255)");
// //by xxj
//        QRect rect = m_VEffectPreview[j]->rect();
//        scene->setSceneRect(rect);
// //end
        connect(scene, SIGNAL(selectEff(QString)), this, SLOT(effectChoose(QString)));
    }

    for (int k = 0; k < 9 ; ++k) {
        QGraphicsPixmapItem *item = new QGraphicsPixmapItem;
        m_VPixmapItem.push_back(item);
    }
    //加入名字
    for (int s = 0; s < 9; ++s) {
        QGraphicsTextItem *name = new QGraphicsTextItem();
        m_VEffectName.push_back(name);
    }
}

void videowidget::delEffectPreview()
{
    //释放九宫格布局
    for (int s = 0 ; s < m_VEffectName.length(); ++s) {
        delete m_VEffectName[s];
        m_VEffectName[s] = NULL;
        //m_VEffectName.pop_back();
    }
    for (int k = 0; k < m_VPixmapItem.length() ; ++k) {
        delete m_VPixmapItem[k];
        m_VPixmapItem[k] = NULL;
        //m_VPixmapItem.pop_back();
    }

    for (int j = 0 ; j < m_VEffectScene.length(); ++j) {
        delete m_VEffectScene[j];
        m_VEffectScene[j] = NULL;
        //m_VEffectScene.pop_back();
    }

    for (int i = 0 ; i < m_VEffectPreview.length(); ++i) {
        m_pGridLayout->removeWidget(m_VEffectPreview[i]);
        delete m_VEffectPreview[i];
        m_VEffectPreview[i] = NULL;
        //m_VEffectPreview.pop_back();
    }
    m_VEffectName.clear();
    m_VPixmapItem.clear();
    m_VEffectScene.clear();
    m_VEffectPreview.clear();
}

QImage videowidget::getCurrentImg()
{
    return *CURRENT_IMAGE;
}

void videowidget::showEvent(QShowEvent *event)
{
    if (isFindedDevice)
        ableButtons();
    else {
        disableButtons();
    }
}

void videowidget::resizePixMap()
{
    if (STATE != EFFECT) {
        //QRect rect = this->rect();
        QRect rect = m_pNormalView->rect();

        //m_pNormalView->setSceneRect(rect);
        m_pNormalScene->setSceneRect(rect);
        int x = m_pNormalView->x();
        int y = m_pNormalView->y();
        m_pNormalItem->setX( rect.width() / 2 - m_pNormalItem->pixmap().width() / 2 );
        m_pNormalItem->setY( rect.height() / 2 - m_pNormalItem->pixmap().height() / 2 );

        m_pCountItem->setX(x + rect.width() / 2 - m_countdownLen / 2);
        m_pCountItem->setY(y + rect.height() / 2 - 50); //设置高度的一半

        m_pTimeItem->setX(x + rect.width() / 2 - 50);
        m_pTimeItem->setY(y + rect.height() - 50);
        //int width = m_pNormalItem->pixmap().width();
        //int height = m_pNormalItem->pixmap().height();
    } else {
        //by xxj
        int x = m_VEffectPreview[0]->x();
        int y = m_VEffectPreview[0]->y();
        for (int i = 0 ; i < m_VEffectScene.length(); ++i) {
//            QRect rect = m_VEffectPreview[i]->rect();
            //m_VEffectPreview[i]->setSceneRect(m_VPixmapItem[i]->boundingRect());
            //int sencewidth = m_VPixmapItem[i]->
            //m_VEffectPreview[i]-

            m_VEffectName[i]->setX(x + m_VPixmapItem[i]->pixmap().width() / 2 - m_VEffectName[i]->textWidth() / 2); //- m_VEffectName[i]->textWidth() / 2
            m_VEffectName[i]->setY(y + m_VPixmapItem[i]->pixmap().height() - 40); //设置高度的一半

            m_VEffectName[i]->setFont(QFont("华文琥珀", 10));
            m_VEffectName[i]->setDefaultTextColor(QColor(255, 255, 255));

            m_VEffectName[i]->show();
        }
        //end
    }
}

void videowidget::resizeEvent(QResizeEvent *size)
{
    resizePixMap();
    return DWidget::resizeEvent(size);
}

void videowidget::onShowCountdown()
{
    qDebug() << "onShowCountdown";
    VIDEO_STATE = NORMALVIDEO;
    if (countTimer->isActive()) {//连续点击拍照
        countTimer->stop();
        countDown = 3;
    }

    countTimer->start(1000);
}

void videowidget::showCountdown()
{
    qDebug() << "showCountdown";
    //显示计时
    if (VIDEO_STATE == AUDIO) {
        showCountDownLabel(VIDEO_STATE);
    } else {
        //显示倒数，3s后结束，并拍照
        if (countDown == 0) {
            //发送就结束信号处理按钮状态
            countTimer->stop();
            countDown = 3;
            m_bTakePic = true;
            hideCountDownLabel();
            if (m_curTakePicTime <= 1) {
                finishTakedCamera();
            }
        } else {
            if (countTimer->isActive()) {
                showCountDownLabel(VIDEO_STATE);
                if (countDown % 3 == 1) {
                    if (flashTimer->isActive()) {//连续点击拍照
                        flashTimer->stop();
                    }
                    flashTimer->start(500);
                }
                countDown--;
            }
        }
    }
}

void videowidget::flash()
{
    labTimer.showFullScreen();
    labTimer.setWindowFlag(Qt::WindowType::ToolTip);
    flashTimer->stop();
}

void videowidget::changeDev()
{
    v4l2_dev_t *vd =  get_v4l2_device_handler();
    imageprocessthread->stop();
    QString str = QString(vd->videodevice);
    if (vd != nullptr) {
        close_v4l2_device_handler();
    }
    v4l2_device_list_t *devlist = get_device_list();
    if (devlist->num_devices == 2) {
        for (int i = 0 ; i < devlist->num_devices; i++) {
            QString str1 = QString(devlist->list_devices[i].device);
            if (QString::compare(str, str1) == false)
                camInit(devlist->list_devices[i].device);
        }
    } else {

        for (int i = 0 ; i < devlist->num_devices; i++) {
            if (QString::compare(str, QString(devlist->list_devices[i].device)) == true) {
                if (i == devlist->num_devices - 1) {
                    camInit(devlist->list_devices[0].device);
                } else {
                    camInit(devlist->list_devices[i + 1].device);
                }
            }
        }
    }
}

void videowidget::onShowThreeCountdown()
{
    qDebug() << "onShowThreeCountdown";
    if (countTimer->isActive()) {//连续点击拍照
        countTimer->stop();
        countDown = 3;
        hideCountDownLabel();
        //拍照保存
    } else {
        VIDEO_STATE = SHOOT;
        //显示倒数，3s后结束，并拍照
        countDown = 3;
        m_curTakePicTime = 3;
        onShowCountdown();
    }
}

void videowidget::onTShowTime()
{
    qDebug() << "onTShowTime";
    if (countTimer->isActive()) {//连续点击拍照
        begin_time = QDateTime::currentDateTime();
        showCountdown();
        //保存视频
        countTimer->stop();
    } else {
        VIDEO_STATE = AUDIO;
        begin_time = QDateTime::currentDateTime();
        countTimer->start(1000);
    }
}

void videowidget::onCancelThreeShots()
{
    qDebug() << "onCancelThreeShots";
    VIDEO_STATE = NORMALVIDEO;
    if (countTimer->isActive()) {
        countTimer->stop();
        countDown = 3;
        hideCountDownLabel();
    }
}

void videowidget::onTakeVideoOver()
{
    qDebug() << "onTakeVideoOver";
    VIDEO_STATE = NORMALVIDEO;
    countTimer->stop();
    hideTimeLabel();
}

void videowidget::onChooseEffect()
{
    qDebug() << "onChooseEffect";
    //by xxj
    if (m_pGridLayout->count() == 1) {
        STATE = EFFECT;
        showPreviewByState(STATE);
    } else {
        STATE = NORMALVIDEO;
        //delEffectPreview();
        showPreviewByState(STATE);
    }
    //end
}

void videowidget::onMoreEffectLeft()
{
    qDebug() << "onMoreEffectLeft";
    EFFECT_PAGE--;

    if (EFFECT_PAGE < 0)
        EFFECT_PAGE++;

    updateEffectName();
}

void videowidget::onMoreEffectRight()
{
    qDebug() << "onMoreEffectRight";
    EFFECT_PAGE++;
    if (EFFECT_PAGE > (EFFECTS_NUM ) / 9)
        EFFECT_PAGE--;
    updateEffectName();
}

void videowidget::effectChoose(QString name)
{
    qDebug() << name;
    //根据名字得带index
    int index = eff->FindEffIndexByName(name);
    EFFECT_INDEX = index;
    //返回预览界面
    STATE = NORMALVIDEO;
    //delEffectPreview();
    showPreviewByState(STATE);
    //返回特效选择结束信号
    finishEffectChoose();
}
void videowidget::onBtnVideo()
{
    hideCountDownLabel();
}
void videowidget::onBtnThreeShots()
{
    hideTimeLabel();
}
void videowidget::onBtnPhoto()
{
    hideTimeLabel();
}

void videowidget::forbidScrollBar(QGraphicsView *view)
{
    view->horizontalScrollBar()->blockSignals(true);
    view->verticalScrollBar()->blockSignals(true);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
