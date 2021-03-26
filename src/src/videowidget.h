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
#include <QSvgRenderer>

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
class QGraphicsSvgItem;

#define FLASH_TIME 500//拍照闪光时间，500毫秒
/**
* @brief PRIVIEW_ENUM_STATE　枚举拍照，无设备，录像
*/
enum PRIVIEW_ENUM_STATE {PICTRUE, NODEVICE, VIDEO};

/**
* @brief videowidget　完成拍照，录像相关工作
* 连拍，延时拍照录像，闪光灯，文件保存等
*/
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

    /**
    * @brief sigFlash　闪光灯信号
    */
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
    * @param status 按钮状态(enable/disable)
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
    * @param strFilename 文件名
    */
    void filename(QString strFilename);

public:
    /**
    * @brief setCapStatus　设置相机状态
    * @param status 拍照状态
    */
    void setCapStatus(bool status)
    {
        m_bActive = status;
    }

    /**
    * @brief getCapStatus　获取相机录制状态
    */
    bool getCapStatus()
    {
        return m_bActive;
    }

    /**
     * @brief stopCountTimeRecord 取消录制
     */
    void takeVideoCancel();

    /**
    * @brief setSaveVdFolder　设置保存视频文件夹
    * @param strVdFolder 视频文件夹路径
    */
    void setSaveVdFolder(QString& strVdFolder)
    {
        m_saveVdFolder = strVdFolder;
    }

    /**
    * @brief setSavePicFolder　设置保存照片文件夹
    * @param strPicFolder 照片文件夹路径
    */
    void setSavePicFolder(QString& strPicFolder)
    {
        m_savePicFolder = strPicFolder;
    }

    /**
    * @brief setInterval　设置倒计时
    * @param nInterval 拍照延时
    */
    void setInterval(int nInterval)
    {
        m_nMaxInterval = nInterval;
        m_Maxinterval = nInterval;
    }

    /**
    * @brief setContinuous　设置连拍次数
    * @param nContinuous 连拍次数
    */
    void setContinuous(int nContinuous)
    {
        m_nMaxContinuous = m_curTakePicTime = nContinuous;
    }

    /**
    * @brief setthumbnail　设置缩略图
    * @param thumb
    */
    void setThumbnail(ThumbnailsBar *thumb);

    /**
    * @brief delayInit　延迟加载
    */
    void delayInit();

public slots:
    /**
    * @brief onTakePic　拍照事件响应
    * @param bTrue
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
    * @brief onEndBtnClicked　点击结束按钮
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
    * @brief ReceiveOpenGLstatus　接收openGL状态
    */
    void ReceiveOpenGLstatus(bool);

#endif

    /**
    * @brief onReachMaxDelayedFrames　达到视频帧最大延迟
    */
    void onReachMaxDelayedFrames();

    /**
    * @brief flash　闪光
    */
    void flash();

    /**
    * @brief slotresolutionchanged　分辨率改变槽函数
    * @param 分辨率字符串(如：1920*1080)
    */
    void slotresolutionchanged(const QString &);

private:
    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;

    /**
     * @brief recoverTabWidget 恢复窗体的tab框选
     */
    void recoverTabWidget();

    /**
    * @brief showCountDownLabel　显示录制和拍照倒计时
    * @param state 当前状态（拍照，没有设备，视频）
    */
    void showCountDownLabel(PRIVIEW_ENUM_STATE state);

    /**
    * @brief forbidScrollBar　禁止滚轮显示区域
    * @param view
    */
    void forbidScrollBar(QGraphicsView *view);

    /**
    * @brief showCountDownLabel　显示设备被占用或者拔掉的图片的槽函数
    */
    void showCamUsed();

    /**
    * @brief showNocam　显示没有设备的图片的槽函数
    */
    void showNocam();

    /**
    * @brief startTakeVideo　开始录像
    */
    void startTakeVideo();

    /**
    * @brief itemPosChange　item位置改变
    */
    void itemPosChange();

    /**
    * @brief itemPosChange　结束所有操作
    */
    void stopEverything();

public:
    MajorImageProcessingThread *m_imgPrcThread;
private:
    /**
     * @brief m_bActive //是否录制中
     */
    bool                       m_bActive;
    int                        m_nMaxContinuous;    //最大连拍数：0,4,10
    int                        m_curTakePicTime;    //当前连拍次数
    int                        m_Maxinterval;       //最大间隔
    int                        m_nMaxInterval;      //最大间隔：0,3,6
    int                        m_nInterval;         //当前间隔时间,初始化为0,按钮响应时赋值
    int                        m_nCount;            //录制计时
    int                        m_nFileID;           //文件id
    int                        m_nFastClick;        //快速点击次数，小于200ms计入

    ThumbnailsBar              *m_thumbnail;        //缩略图

    DLabel                     *m_flashLabel;       //闪光灯Label
    DLabel                     *m_dLabel;           //倒计时
    DBlurEffectWidget          *m_recordingTimeWidget;//录制时长窗口
    DLabel                     *m_recordingTime;    //录制时长
    DPushButton                *m_endBtn;           //结束按钮
    DFloatingWidget            *m_fWgtCountdown;    //显示倒计时

    QSound                     *m_takePicSound;     //拍照声音
    QPixmap                    m_framePixmap;       //帧图片
    QSvgRenderer               m_svg;
    QString                    m_savePicFolder;     //图片文件夹路径
    QString                    m_saveVdFolder;      //视频文件夹路径
    QTimer                     *m_countTimer;       //倒计时定时器
    QTimer                     *m_flashTimer;       //闪光灯定时器
    QTimer                     *m_recordingTimer;   //录制3秒后，每200ms设置一次时间
    QDateTime                  m_btnClickTime;      //按钮点击时间
    QGridLayout                *m_pGridLayout;
    QGraphicsView              *m_pNormalView;
    QGraphicsScene             *m_pNormalScene;
    QGraphicsSvgItem           *m_pSvgItem;
#ifndef __mips__
    PreviewOpenglWidget        *m_openglwidget;     //opengl渲染窗口
#else
    QGraphicsPixmapItem        *m_pNormalItem;
#endif
    QGraphicsTextItem          *m_pCamErrItem;      //摄像头异常提示
};

#endif // VIDEOWIDGET_H
