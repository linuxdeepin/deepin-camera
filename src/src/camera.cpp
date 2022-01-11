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
#define TMPPATH "/tmp/tmp.jpg"

#include "camera.h"
#include "videosurface.h"

#include <QUrl>
#include <QtMultimediaWidgets/QCameraViewfinder>

Camera *Camera::m_instance = nullptr;

Camera *Camera::instance()
{
    if (m_instance == nullptr) {
        m_instance = new Camera;
    }
    return m_instance;
}

Camera::Camera()
{
    m_cameraInfoList = QCameraInfo::availableCameras();
    initMember();
}

void Camera::initMember()
{
    currentCamera = 0;
    m_camera = new QCamera(QCameraInfo::defaultCamera());
    m_imageCapture = new QCameraImageCapture(m_camera);
    m_mediaRecoder = new QMediaRecorder(m_camera);

    // 连接相机surface，能够发送每帧QImage数据到外部
    m_videoSurface = new VideoSurface(this);
    m_camera->setViewfinder(m_videoSurface);
    connect(m_videoSurface, &VideoSurface::presentImage, this, &Camera::presentImage);

    m_camera->start();

    QCameraViewfinderSettings viewfinderSettings;
    viewfinderSettings.setResolution(640, 480);
    m_camera->setViewfinderSettings(viewfinderSettings);

}

void Camera::switchCamera()
{
    currentCamera++;
    if (currentCamera > m_cameraInfoList.size())
        currentCamera = 0;
    m_camera->stop();
    m_camera = new QCamera(m_cameraInfoList.at(currentCamera));
}

QList<QSize> Camera::getSupportResolutions()
{
    if (!m_camera)
        return QList<QSize>();

    QList<QSize> supportResolutionList = m_camera->supportedViewfinderResolutions();
    if (supportResolutionList.isEmpty())
        qInfo() << "Support Resolution is Empty!";

    return supportResolutionList;
}

void Camera::setCameraResolution(QSize &size)
{
    if (m_camera) {
        QCameraViewfinderSettings set = m_camera->viewfinderSettings();
        set.setResolution(size);
        m_camera->setViewfinderSettings(set);
    }
}

void Camera::captureImage()
{
    if (!m_camera)
        return;

    if (m_imageCapture->isReadyForCapture()) {
        m_imageCapture->capture(TMPPATH);
    }
}

QCamera *Camera::getCamera()
{
    return m_camera;
}

