/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
* Maintainer: hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
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

#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QCamera>
#include <QList>
#include <QCameraInfo>
#include <QMediaRecorder>
#include <QCameraImageCapture>

typedef struct _cam_config_t {
    int  width;      /*width*/
    int  height;     /*height*/
    char *device_name; /*device name*/
    char *device_location; /*device location*/
    char *video_path;
    char *video_name;
    char *photo_path;
    char *photo_name;
    int fps_num;
    int fps_denom;
} cam_config;
class VideoSurface;
class Camera : public QObject
{
    Q_OBJECT
public:

    static Camera *instance();

    /**
     * @brief initMember
     * 初始化参数
     */
    void initMember();

    /**
     * @brief getSupportResolutions 获取支持的分辨率列表
     * @return 当前摄像头分辨率列表
     */
    QStringList getSupportResolutions();

    /**
     * @brief setCameraResolution
     * 设置当前摄像头的分辨率
     */
    void setCameraResolution(QSize);

    /**
     * @brief setCameraResolution
     * 获取当前摄像头的分辨率
     */
    QSize getCameraResolution();

    /**
     * @brief startCamera 开始当前相机
     */
    void startCamera(const QString& devName);

    /**
     * @brief stopCamera 停止当前相机
     */
    void stopCamera();

    /**
     * @brief setCaptureImage 设置为捕获图像
     */
    void setCaptureImage();

    /**
     * @brief setCaptureVideo 设置为捕获视频
     */
    void setCaptureVideo();

    /**
     * @brief setVideoOutPutPath 设置视频保存路径
     * @param path 视频输出路径
     */
    void setVideoOutPutPath(QString& path);

    /**
     * @brief startRecoder 开始录制
     */
    void startRecoder();

    /**
     * @brief stopRecoder 停止录制
     */
    void stopRecoder();

    qint64 getRecoderTime();

    QMediaRecorder::State getRecoderState();

    int parseConfig();
    int saveConfig();

public slots:
    /**
     * @brief switchCamera 切换摄像头
     */
    void switchCamera();

    /**
     * @brief refreshCamera 刷新摄像头设备状态
     */
    void refreshCamera();

    /**
     * @brief restartCamera 重启摄像头
     */
    void restartCamera();

    /**
     * @brief refreshCamInfoList 刷新摄像头设备列表
     */
    void refreshCamDevList();

signals:
    // 发送摄像头每帧画面
    void presentImage(QImage&);

    // 当前摄像头断开链接信号
    void currentCameraDisConnected();

    // 摄像头切换信号
    void cameraSwitched(const QString&);

private:
    explicit Camera();

private:
    static Camera               *m_instance;
    QCamera                     *m_camera;
    QMediaRecorder              *m_mediaRecoder;
    QCameraImageCapture         *m_imageCapture;
    VideoSurface                *m_videoSurface;

    QString                     m_curDevName;          // 当前摄像头设备名
    QCameraViewfinderSettings   m_viewfinderSettings;
    QList<QString>              m_cameraDevList;       // 摄像头设备名列表

    QCamera::CaptureMode        m_captureMode;         //用于在断开摄像头时保存状态
};

#endif //CAMERA_H
