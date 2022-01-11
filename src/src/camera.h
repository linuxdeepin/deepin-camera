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

class Camera : public QObject
        {
    Q_OBJECT
public:
    explicit Camera();
    /**
     * @brief initMember
     * 初始化参数
     */
    void initMember();
    /**
     * @brief switchCamera 切换摄像头
     */
    void switchCamera();

    /**
     * @brief getSupportResolutions 获取支持的分辨率列表
     * @return 当前摄像头分辨率列表
     */
    QList<QSize> getSupportResolutions();
    /**
     * @brief setCameraResolution
     * 设置当前摄像头的分辨率
     */
    void setCameraResolution(QSize&);

    /**
     * @brief stopCamera 停止当前相机
     */
    void stopCamera();

    /**
     * @brief startCamera 开始当前相机
     */
    void startCamera();

    /**
     * @brief getCamera 提供直接操作摄像头接口
     * @return QCamera指针
     */
    QCamera* getCamera();

public slots:
    /**
     * @brief captureImage 抓取当前图像
     */
    void captureImage();

signals:
    void imageCapture(int, QImage);

private:
    QCamera                 *m_camera;
    QMediaRecorder          *m_mediaRecoder;
    QCameraImageCapture     *m_imageCapture;

    QList<QCameraInfo>      m_cameraInfoList;      ///摄像头信息列表
    int currentCamera;                             ///当前的摄像头
};

#endif //CAMERA_H
