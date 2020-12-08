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
#include <DFloatingWidget>
#include <DLabel>
#include <DFontSizeManager>
#include <DPushButton>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QtMultimedia/QSound>
#include <QDateTime>

#include "LPF_V4L2.h"
#include "majorimageprocessingthread.h"
#include "thumbnailsbar.h"
#include "previewopenglwidget.h"


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
enum PRIVIEW_ENUM_STATE {PICTRUE, NODEVICE, VIDEO};

class videowidget : public DWidget
{
    Q_OBJECT
public:
    explicit videowidget(DWidget *parent = nullptr);
    ~videowidget();

signals:
    /**
    * @brief sigDeviceChange　相机改变信号
    */
    void sigDeviceChange();

    void sigFlash();
    /**
    * @brief takePicCancel　取消拍照
    */
    void takePicCancel();
    /**
    * @brief takePicDone　拍照结束信号
    */
    void takePicDone();
    /**
    * @brief takePicOnce　多连拍时，除最后一次，每次提交一个信号
    */
    void takePicOnce();
    /**
    * @brief takeVdCancel　录制倒计时期间取消
    */
    void takeVdCancel();
    /**
    * @brief takeVdDone　录制完成
    */
    void takeVdDone();
    /**
    * @brief setBtnStatues　设置按钮状态
    */
    void setBtnStatues(bool status);
    /**
    * @brief updateBlockSystem　阻塞更新
    * @param bTrue
    */
    void updateBlockSystem(bool bTrue);
    /**
    * @brief noCam　无相机
    */
    void noCam();
    /**
    * @brief noCamAvailable　无相机可获取
    */
    void noCamAvailable();
    /**
    * @brief filename　文件名
    * @param bTrue
    */
    void filename(QString strFilename);

public:
    /**
    * @brief setCapStatus　设置相机状态
    */
    void setCapStatus(bool status)
    {
        m_bActive = status;
    }

    /**
    * @brief getCapStatus　获取相机状态
    */
    bool getCapStatus()
    {
        return m_bActive;
    }

    /**
    * @brief getFolder　获取保存文件夹路径
    */
    QString getFolder()
    {
        return m_strFolder;
    }

    /**
    * @brief setSaveFolder　设置保存文件夹
    */
    void setSaveFolder(QString strFolder)
    {
        m_strFolder = strFolder;
    }

    /**
    * @brief setInterval　设置倒计时
    */
    void setInterval(int nInterval)
    {
        m_nMaxInterval = nInterval;
        m_Maxinterval = nInterval;
    }

    /**
    * @brief setContinuous　连拍次数
    */
    void setContinuous(int nContinuous)
    {
        m_nMaxContinuous = m_curTakePicTime = nContinuous;
    }

    /**
    * @brief setthumbnail　延迟加载
    */
    void setThumbnail(ThumbnailsBar *thumb);

    /**
    * @brief delayInit　延迟加载
    */
    void delayInit();
public slots:
    /**
    * @brief onTakePic　拍照事件响应
    */
    void onTakePic(bool bTrue);

    /**
    * @brief onTakeVideo　录像事件响应
    */
    void onTakeVideo();

    /**
    * @brief showCountdown　倒计时
    */
    void showCountdown();

    /**
    * @brief showRecTime　录制3秒后，每200ms读取时间并显示
    */
    void showRecTime();

    /**
    * @brief onChangeDev　设备切换
    */
    void onChangeDev();

    /**
    * @brief onEndBtnClicked　结束按钮click事件响应
    */
    void onEndBtnClicked();

    /**
    * @brief restartDevices　重启设备
    */
    void onRestartDevices();


private slots:
#ifdef __mips__
    /**
    * @brief ReceiveMajorImage　处理视频帧
    */
    void ReceiveMajorImage(QImage *image, int result);
#else

    /**
    * @brief ReceiveOpenGLstatus　openGL状态
    */
    void ReceiveOpenGLstatus(bool);
#endif

    /**
    * @brief onReachMaxDelayedFrames　达到视频帧最大延迟
    */
    void onReachMaxDelayedFrames();

    /**
    * @brief flash　闪光恢复
    */
    void flash();
    /**
    * @brief slotresolutionchanged　分辨率改变
    */
    void slotresolutionchanged(const QString &);

private:
    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;

    /**
    * @brief showCountDownLabel　显示录制和拍照倒计时
    */
    void showCountDownLabel(PRIVIEW_ENUM_STATE state);

    /**
    * @brief forbidScrollBar　关闭滚轮显示区域
    * @param view
    */
    void forbidScrollBar(QGraphicsView *view);

    /**
    * @brief newPreViewByState　新建view
    * @param state
    */
    void newPreViewByState(PRIVIEW_ENUM_STATE state);

    /**
    * @brief showCountDownLabel　摄像头被占用
    */
    void showCamUsed();

    /**
    * @brief showNocam　未发现摄像头
    */
    void showNocam();

    /**
    * @brief startTakeVideo　开始录像
    */
    void startTakeVideo();

    /**
    * @brief itemPosChange　item位置处理
    */
    void itemPosChange();

    /**
    * @brief itemPosChange　结束所有操作
    */
    void stopEverything();
public:
    MajorImageProcessingThread *m_imgPrcThread;

private:
    bool m_bActive = false;//是否录制中
#ifndef __mips__
    PreviewOpenglWidget     *m_openglwidget;//opengl渲染窗口
#endif
    DLabel                  *m_flashLabel;  //闪光灯Label
    QGraphicsView           *m_pNormalView;
    QGraphicsScene          *m_pNormalScene;
    QGraphicsPixmapItem     *m_pNormalItem;
    QGraphicsTextItem       *m_pCamErrItem; //摄像头异常提示
    QSound                  *m_takePicSound;//拍照声音
    QGridLayout             *m_pGridLayout;
    DFloatingWidget         *m_fWgtCountdown; //显示倒计时
    //浮动窗口添加磨砂窗口和结束按钮
    DLabel                  *m_dLabel;  //倒计时
    DPushButton             *m_btnVdTime; //录制屏显时长
    DPushButton             *m_endBtn;      //结束按钮
    QTimer                  *countTimer;     //倒计时定时器
    QTimer                  *flashTimer;     //闪光灯定时器
    QTimer                  *recordingTimer;//录制3秒后，每200ms设置一次时间
    QDateTime               m_btnClickTime; //按钮点击时间
    int                     m_nFastClick; //快速点击次数，小于200ms计入
    PRIVIEW_ENUM_STATE      m_priview_state = PICTRUE;//当前预览状态
    ThumbnailsBar           *m_thumbnail;       //缩略图
    QPixmap                 m_pixmap;         //
    int                     m_nFileID;        //文件id
    QString                 m_strFolder;      //文件路径（视频,图片）
    int                     m_nMaxContinuous; //最大连拍数：0,4,10
    int                     m_curTakePicTime; //当前连拍次数
    int                     m_Maxinterval;     //最大间隔
    int                     m_nMaxInterval; //最大间隔：0,3,6
    int                     m_nInterval; //当前间隔时间,初始化为0,按钮响应时赋值
    int                     m_nCount; //录制计时
};

#endif // VIDEOWIDGET_H
