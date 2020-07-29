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

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <DWidget>
#include <QDateTime>
#include <DFloatingWidget>
#include <DLabel>
#include <DFontSizeManager>
#include <DPushButton>
#include "LPF_V4L2.h"
#include "majorimageprocessingthread.h"

DWIDGET_USE_NAMESPACE

class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsTextItem;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;

enum PRIVIEW_STATE {NORMALVIDEO, NODEVICE, AUDIO};


class videowidget : public DWidget
{
    Q_OBJECT
public:
    explicit videowidget(DWidget *parent = nullptr);
    ~videowidget();
    //未发现摄像头
    void showNocam();

signals:
    void sigFlash();
    void takePicCancel();
    void takePicDone();//拍照结束信号
    void takePicOnce();//多连拍时，除最后一次，每次提交一个信号
    void takeVdCancel(); //录制倒计时期间取消了
    void takeVdDone();
    void setBtnStatues(bool status);
    void updateBlockSystem(bool bTrue);
public:
    QString getFolder()
    {
        return m_strFolder;
    }
    void setSaveFolder(QString strFolder)
    {
        m_strFolder = strFolder;
    }
    void setInterval(int nInterval)
    {
        m_nMaxInterval = nInterval;
    }
    void setContinuous(int nContinuous)
    {
        m_nMaxContinuous = m_curTakePicTime = nContinuous;
    }

    void setCapstatus(bool status)
    {
        m_bActive = status;
    }

    bool getCapstatus()
    {
        return m_bActive;
    }
public slots:
    void onTakePic(bool bTrue);
    void onTakeVideo();
    void showCountdown();
    void changeDev();
    void endBtnClicked();
    void restartDevices();


private slots:
    void ReceiveMajorImage(QPixmap image, int result);
    void onReachMaxDelayedFrames();
    void flash();

private:
    void init();

    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;
    void resizePixMap();

    //设置字体
    void setFont(QGraphicsTextItem *item, int size, QString str);
    void showCountDownLabel(PRIVIEW_STATE state);
    void hideCountDownLabel();
    void hideTimeLabel();

    //关闭滚轮显示区域
    void forbidScrollBar(QGraphicsView *view);

    //新建view
    void newPreViewByState(PRIVIEW_STATE state);

    //显示录制和拍照倒计时
    void showCountDownLabel();

    //摄像头被占用
    void showCamUsed();

    //开始录像
    void startTakeVideo();

    //item位置处理
    void itemPosChange();
public:
    MajorImageProcessingThread *m_imgPrcThread;

private:
    bool m_bActive;//是否录制中

    DLabel               *m_flashLabel;

    QGraphicsView        *m_pNormalView;
    QGraphicsScene       *m_pNormalScene;
    QGraphicsPixmapItem *m_pNormalItem;
    QGraphicsTextItem    *m_pCamErrItem; //摄像头异常提示
    QGraphicsTextItem    *m_pTimeItem;

    QGridLayout          *m_pGridLayout;

    DFloatingWidget         *m_fWgtCountdown; //显示倒计时
    //浮动窗口添加磨砂窗口和结束按钮
    DLabel                  *m_dLabel;
    DPushButton             *m_btnVdTime; //录制屏显时长

    DPushButton             *m_endBtn;

    QTimer                  *countTimer;
    QTimer                  *flashTimer;

    QDateTime               m_btnClickTime; //按钮点击时间
    int                     m_nFastClick; //快速点击次数，小于200ms计入

    PRIVIEW_STATE STATE = NORMALVIDEO;

    int                     m_countdownLen = 1;

    QPixmap              m_pixmap;
    int                     m_nFileID;
    QString                 m_strFolder;
    int                     m_nMaxContinuous; //最大连拍数：0,4,10
    int                     m_curTakePicTime; //当前连拍次数
    int                     m_nMaxInterval; //最大间隔：0,3,6
    int                     m_nInterval; //当前间隔时间,初始化为0,按钮响应时赋值
    QTime                m_time;
    int m_nCount; //录制计时
};

#endif // VIDEOWIDGET_H
