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
#include <DLabel>
#include "LPF_V4L2.h"
#include "majorimageprocessingthread.h"
#include "videoeffect.h"

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

class videowidget : public DWidget
{
    Q_OBJECT
public:
    explicit     videowidget(DWidget *parent = nullptr);
    //    void        showEvent(QShowEvent *event);

signals:
    void sigFlash();
    void takePicDone();
    void takeVdCancel(); //录制倒计时期间取消了

public:
    void setSaveFolder(QString strFolder) { m_strFolder = strFolder; }
    void setInterval(int nInterval)
    { /*m_nInterval = */
        m_nMaxInterval = nInterval;
    }
    void setContinuous(int nContinuous) { m_curTakePicTime = m_nMaxContinuous = nContinuous; }
public slots:
    void onTakePic();
    void onTakeVideo();
    void onTakeVideoOver();
    void showCountdown();
    void changeDev();

private slots:
    void ReceiveMajorImage(QImage image, int result);
    void onReachMaxDelayedFrames();
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
    void showPreviewByState(PRIVIEW_STATE state);//展示view

    void init();
    void showCountDownLabel();
    void showNocam();
    void showCamUsed();
    void startTakeVideo();

private:
    bool isFindedDevice = false;
    videoEffect *eff;
    DLabel m_flashLabel;

    QGraphicsView *m_pNormalView;
    QGraphicsScene *m_pNormalScene;
    QGraphicsPixmapItem *m_pNormalItem;
    QGraphicsTextItem *m_pCountItem; //拍照倒计时显示控件
    QGraphicsTextItem *m_pTimeItem;

    QGridLayout *m_pGridLayout;

    QTimer *countTimer;
    QTimer *flashTimer;
    QDateTime begin_time;
    QDateTime m_btnClickTime; //按钮点击时间
    int m_nFastClick; //快速点击次数，小于200ms计入
    //bool m_bCountDownOver;

    PRIVIEW_STATE STATE = NORMALVIDEO;
    int EFFECT_PAGE = 0;

    int m_countdownLen = 1;
    int err11, err19;
    MajorImageProcessingThread *m_imgPrcThread;
    QImage m_img;
    QPixmap m_pixmap;

    int m_nFileID;
    QString m_strFolder;

    int m_nMaxContinuous; //最大连拍数：0,4,10
    int m_curTakePicTime; //当前连拍次数
    int m_nMaxInterval; //最大间隔：0,3,6
    int m_nInterval; //当前间隔时间,初始化为0,按钮响应时赋值

    bool is_active;
    QString m_strFileName;
};

#endif // VIDEOWIDGET_H
