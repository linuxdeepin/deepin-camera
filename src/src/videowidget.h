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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <DWidget>
#include <QDateTime>
#include "LPF_V4L2.h"
#include "majorimageprocessingthread.h"
#include "videoeffect.h"
#include <DLabel>

DWIDGET_USE_NAMESPACE

class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsTextItem;
class QGridLayout;
class MyScene;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;


//预览代理
//xxj
enum PRIVIEW_STATE {NORMALVIDEO, NODEVICE, EFFECT, AUDIO, SHOOT};
static PRIVIEW_STATE VIDEO_STATE = NORMALVIDEO;
static int EFFECT_INDEX = NO;
static QImage *CURRENT_IMAGE = new QImage;

class videowidget : public DWidget
{
    Q_OBJECT
public:
    explicit     videowidget(DWidget *parent = nullptr);
    void        showEvent(QShowEvent *event);
    QImage     getCurrentImg();

signals:
    void finishTakedCamera();//结束拍照或三连拍
    void finishEffectChoose();//结束特效选择
    void sigFlash();
    void disableButtons();
    void ableButtons();
public slots:
    void onShowCountdown();
    void onShowThreeCountdown();
    void onTShowTime();
    void onCancelThreeShots();
    void onTakeVideoOver();
    void onChooseEffect();
    void onMoreEffectLeft();
    void onMoreEffectRight();
    void onBtnPhoto();
    void onBtnThreeShots();
    void onBtnVideo();

    void changePicture(PRIVIEW_STATE state, QImage *img, int effectIndex);
    void showCountdown();
    void effectChoose(QString name);
    void changeDev();

private slots:
    void ReceiveMajorImage(QImage image, int result);
    void flash();

private:
    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;
    void resizePixMap();

    void transformImage(QImage *img);
    void resizeImage(QImage *img);
    void setFont(QGraphicsTextItem *item, int size, QString str); //自体

    void showCountDownLabel(PRIVIEW_STATE state);
    void hideCountDownLabel();
    void hideTimeLabel();
    void forbidScrollBar(QGraphicsView *view);

    void newPreViewByState(PRIVIEW_STATE state);//新建view
    void sceneAddItem();
    void updateEffectName();
    void newEffectPreview();//创建特效view
    void delEffectPreview();//释放view
    void showPreviewByState(PRIVIEW_STATE state);//展示view

    void init();
    void showCountDownLabel();


private:
    bool isFindedDevice = false;
    videoEffect *eff;
    DLabel labTimer;

    QGraphicsView *m_pNormalView;
    QGraphicsScene *m_pNormalScene;
    QGraphicsPixmapItem *m_pNormalItem;
    QGraphicsTextItem *m_pCountItem;
    QGraphicsTextItem *m_pTimeItem;

    QVector<QGraphicsTextItem *> m_VEffectName;
    QVector<QGraphicsPixmapItem *> m_VPixmapItem;
    QVector<QGraphicsView *> m_VEffectPreview;
    QVector<MyScene *> m_VEffectScene;
    QGridLayout *m_pGridLayout;

    int countDown;
    QTimer *countTimer;
    QTimer *flashTimer;
    QDateTime begin_time;

    PRIVIEW_STATE STATE = NORMALVIDEO;
    int EFFECT_PAGE = 0;

    int m_curTakePicTime;//当前连拍次数
    int m_countdownLen = 1;
    int err11, err19;
    MajorImageProcessingThread *imageprocessthread;
    bool m_bTakePic;

signals:
//    void finishTakedCamera();//结束拍照或三连拍
//    void finishEffectChoose();//结束特效选择
//    void sigFlash();
//    void disableButtons();
//    void ableButtons();
public slots:
//    void onShowCountdown();
//    void onShowThreeCountdown();
//    void onTShowTime();
//    void onCancelThreeShots();
//    void onTakeVideoOver();
//    void onChooseEffect();
//    void onMoreEffectLeft();
//    void onMoreEffectRight();
//    void onBtnPhoto();
//    void onBtnThreeShots();
//    void onBtnVideo();

//    void changePicture(PRIVIEW_STATE state, QImage *img, int effectIndex);
//    void showCountdown();
//    void effectChoose(QString name);


};

#endif // VIDEOWIDGET_H
