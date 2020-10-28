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

#include <QtMultimedia/QSound>
#include <DWidget>
#include <QDateTime>
#include <DFloatingWidget>
#include <DLabel>
#include <DFontSizeManager>
#include <DPushButton>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include "LPF_V4L2.h"
#include "majorimageprocessingthread.h"
#include "thumbnailsbar.h"

DWIDGET_USE_NAMESPACE
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsTextItem;
class QGridLayout;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;
class QSound;

#define FLASH_TIME 500//拍照闪光时间，500毫秒
enum PRIVIEW_STATE {NORMALVIDEO, NODEVICE, AUDIO};
enum DeviceStatus {NOCAM, CAM_CANNOT_USE, CAM_CANUSE}; // 定义枚举类型设备状态，无摄像头、有无法使用的摄像头、有可用摄像头

class videowidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit videowidget(DWidget *parent = nullptr);
    ~videowidget();
    //未发现摄像头
    void showNocam();

signals:
    void sigDeviceChange();
    void sigFlash();
    void takePicCancel();
    void takePicDone();//拍照结束信号
    void takePicOnce();//多连拍时，除最后一次，每次提交一个信号
    void takeVdCancel(); //录制倒计时期间取消了
    void takeVdDone();
    void setBtnStatues(bool status);
    void updateBlockSystem(bool bTrue);
    void noCam();
    void noCamAvailable();
    void filename(QString strFilename);

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

    DPushButton* getEndBtn()
    {
        return m_endBtn;
    }

    void setthumbnail(ThumbnailsBar *thumb);
public slots:
    void onTakePic(bool bTrue);
    void onTakeVideo();
    void showCountdown();
    //录制3秒后，每200ms读取时间并显示
    void showRecTime();
    void changeDev();
    void endBtnClicked();
    void manualClicked();
    void restartDevices();


private slots:
    void ReceiveMajorImage(QImage *image, int result);
    void onReachMaxDelayedFrames();
    void flash();
    void slotresolutionchanged(const QString &);

    /*
     * openGL override
     * start
     */
    void slotShowYuv(uchar *ptr,uint width,uint height); //显示一帧Yuv图像
protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    /*
     * openGL overriede
     * end
     */

private:
    void init();

    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;

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

    //结束所有操作
    void stopEverything();
public:
    MajorImageProcessingThread *m_imgPrcThread;

private:
    bool m_bActive;//是否录制中

    DLabel                  *m_flashLabel;

    QGraphicsView           *m_pNormalView;
    QGraphicsScene          *m_pNormalScene;
    QGraphicsPixmapItem     *m_pNormalItem;
    QGraphicsTextItem       *m_pCamErrItem; //摄像头异常提示

    QSound                  *m_takePicSound;

    QGridLayout             *m_pGridLayout;

    DFloatingWidget         *m_fWgtCountdown; //显示倒计时
    //浮动窗口添加磨砂窗口和结束按钮
    DLabel                  *m_dLabel;
    DPushButton             *m_btnVdTime; //录制屏显时长

    DPushButton             *m_endBtn;

    QTimer                  *countTimer;
    QTimer                  *flashTimer;
    QTimer                  *recordingTimer;//录制3秒后，每200ms设置一次时间
    QDateTime               m_btnClickTime; //按钮点击时间
    int                     m_nFastClick; //快速点击次数，小于200ms计入

    PRIVIEW_STATE STATE = NORMALVIDEO;
    ThumbnailsBar         *m_thumbnail;
    QPixmap               m_pixmap;
    int                     m_nFileID;
    QString                 m_strFolder;
    int                     m_nMaxContinuous; //最大连拍数：0,4,10
    int                     m_curTakePicTime; //当前连拍次数
    int                     m_nMaxInterval; //最大间隔：0,3,6
    int                     m_nInterval; //当前间隔时间,初始化为0,按钮响应时赋值
    int                     m_nCount; //录制计时

private:
    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;
    GLuint textureUniformY,textureUniformU,textureUniformV; //opengl中y、u、v分量位置
    QOpenGLTexture *textureY = nullptr,*textureU = nullptr,*textureV = nullptr;
    GLuint idY,idU,idV; //自己创建的纹理对象ID，创建错误返回0
    uint videoW,videoH;
    uchar *yuvPtr = nullptr;
};

#endif // VIDEOWIDGET_H
