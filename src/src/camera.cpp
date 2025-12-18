// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#define TMPPATH "/tmp/tmp.jpg"

#include "camera.h"
#if QT_VERSION_MAJOR > 5
#else
#include "videosurface.h"
#endif
#include "datamanager.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "core_io.h"
#include "v4l2_devices.h"
#ifdef __cplusplus
}
#endif

#include <sys/stat.h>
#include <unistd.h>

#include <QUrl>
#if QT_VERSION_MAJOR <= 5
#include <QtMultimediaWidgets/QCameraViewfinder>
#endif
#include <QDir>

#define MAXLINE 100

Camera *Camera::m_instance = nullptr;

static _cam_config_t camConfig = {1920, 1080, NULL, NULL, NULL, NULL, NULL, NULL, 1, 30};
Camera *Camera::instance()
{
    // qDebug() << "Function started: Camera::instance";
    if (m_instance == nullptr) {
        // qDebug() << "Condition check: Creating new Camera instance";
        m_instance = new Camera;
    }
    // qDebug() << "Function completed: Camera::instance";
    return m_instance;
}

void Camera::release()
{
    // qDebug() << "Releasing Camera instance";
    delete m_instance;
    m_instance = nullptr;
}

Camera::~Camera()
{
    // qDebug() << "Destroying Camera instance";
    if (camConfig.device_name) {
        // qDebug() << "Condition check: Freeing device_name";
        free(camConfig.device_name);
        camConfig.device_name = nullptr;
    }
    if (camConfig.device_location) {
        // qDebug() << "Condition check: Freeing device_location";
        free(camConfig.device_location);
        camConfig.device_location = nullptr;
    }
    if (camConfig.video_path) {
        // qDebug() << "Condition check: Freeing video_path";
        free(camConfig.video_path);
        camConfig.video_path = nullptr;
    }
    if (camConfig.video_name) {
        // qDebug() << "Condition check: Freeing video_name";
        free(camConfig.video_name);
        camConfig.video_name = nullptr;
    }
    if (camConfig.photo_path) {
        // qDebug() << "Condition check: Freeing photo_path";
        free(camConfig.photo_path);
        camConfig.photo_path = nullptr;
    }
    if (camConfig.photo_name) {
        // qDebug() << "Condition check: Freeing photo_name";
        free(camConfig.photo_name);
        camConfig.photo_name = nullptr;
    }
    // qDebug() << "Camera instance destroyed";
}

Camera::Camera()
    : m_camera(nullptr)
    , m_mediaRecoder(nullptr)
#if QT_VERSION_MAJOR > 5
#else
    , m_imageCapture(nullptr)
    , m_videoSurface(nullptr)
    , m_curDevName("")
#endif
    , m_bReadyRecord(false)
    , m_bRecording(false)
{
    qDebug() << "Initializing Camera";
    parseConfig();
    initMember();
}

void Camera::initMember()
{
    // 连接相机surface，能够发送每帧QImage数据到外部
    qDebug() << "Initializing camera members";
#if QT_VERSION_MAJOR > 5
    qDebug() << "Condition check: Qt6 detected, using QVideoSink";
#else
    qDebug() << "Condition check: Qt5 detected, using VideoSurface";
    m_videoSurface = new VideoSurface(this);
    connect(m_videoSurface, &VideoSurface::presentImage, this, &Camera::presentImage);
#endif

    // 初始设备名为空，默认启动config中的摄像头设备，若config设备名为空，则启动defaultCamera
    switchCamera();
    qDebug() << "Function completed: Camera::initMember";
}

void Camera::switchCamera()
{
    qDebug() << "Switching camera";
    refreshCamDevList();

#if QT_VERSION_MAJOR > 5
    QCameraDevice nextDevice;

    if (!m_cameraDeviceList.isEmpty()) {
        qDebug() << "Condition check: Camera device list not empty";
        int nCurIndex = m_cameraDeviceList.indexOf(m_curDevice);
        nCurIndex = (nCurIndex + 1) % m_cameraDeviceList.size();
        nextDevice = m_cameraDeviceList[nCurIndex];
        qDebug() << "Switching to next camera device:" << nextDevice.description();
    }
    nextDevice = QMediaDevices::defaultVideoInput();
    startCamera(nextDevice);
#else
    QString nextDevName;

    if (!m_cameraDevList.isEmpty()) {
        qDebug() << "Condition check: Camera device list not empty";
        int nCurIndex = m_cameraDevList.indexOf(m_curDevName);
        nCurIndex = (nCurIndex + 1) % m_cameraDevList.size();
        nextDevName = m_cameraDevList[nCurIndex];
        qDebug() << "Switching to next camera device:" << nextDevName;
    }

    if (nextDevName.isEmpty()) {
        qDebug() << "Condition check: No camera device selected, using config";
        nextDevName = camConfig.device_location;
    }

    if (nextDevName.isEmpty()) {
        qDebug() << "Condition check: No device in config, using default";
        nextDevName = QCameraInfo::defaultCamera().deviceName();
        qDebug() << "Using default camera device:" << nextDevName;
    }
    startCamera(nextDevName);
#endif
    qDebug() << "Function completed: Camera::switchCamera";
}

void Camera::refreshCamera()
{
    qDebug() << "Refreshing camera";
    refreshCamDevList();

    // 设备链表找不到当前设备，发送设备断开信号
#if QT_VERSION_MAJOR > 5
    if (!m_curDevice.isNull() && m_cameraDeviceList.indexOf(m_curDevice) == -1) {
        qWarning() << "Current camera device disconnected";
        m_curDevice = QCameraDevice();
        emit currentCameraDisConnected();
    }
#else
    if (!m_curDevName.isEmpty() && m_cameraDevList.indexOf(m_curDevName) == -1) {
        qWarning() << "Current camera device disconnected:" << m_curDevName;
        m_curDevName = "";
        emit currentCameraDisConnected();
    }
#endif

    // 若没有摄像头工作，则重启摄像头
    restartCamera();
    qDebug() << "Function completed: Camera::refreshCamera";
}

#include <QtMultimedia>
// 重启摄像头
void Camera::restartCamera()
{
    qDebug() << "Restarting camera";
#if QT_VERSION_MAJOR > 5
    if (!m_camera) {
        qDebug() << "Condition check: No camera instance exists";
        QList<QCameraDevice> availableCams = QMediaDevices::videoInputs();
        if ((availableCams.isEmpty() || (m_camera && availableCams.indexOf(m_camera->cameraDevice()) != -1))
#if QT_VERSION_MAJOR > 5
            && !m_cameraDeviceList.isEmpty()) {
#else
            && !m_cameraDevList.isEmpty()) {
#endif
            qWarning() << "No available cameras found";
            emit cameraCannotUsed();
            return;
        }
#else
    if (!m_camera || m_camera->status() == QCamera::UnloadedStatus) {
        qDebug() << "Condition check: Camera needs restart";
        QList<QCameraInfo> availableCams = QCameraInfo::availableCameras();
        if ((availableCams.isEmpty() || (m_camera && availableCams.indexOf(QCameraInfo(*m_camera)) != -1))
            && !m_cameraDevList.isEmpty()) {
            qWarning() << "No available cameras found";
            emit cameraCannotUsed();
            return;
        }
#endif
    }

    if (DataManager::instance()->getdevStatus() == CAM_CANUSE) {
        qDebug() << "Camera is already in use";
        return;
    }

#if QT_VERSION_MAJOR > 5
    startCamera(QMediaDevices::defaultVideoInput());
#else
    startCamera(QCameraInfo::defaultCamera().deviceName());
#endif

    DataManager::instance()->setdevStatus(CAM_CANUSE);
    qInfo() << "Camera restarted successfully";
    emit cameraDevRestarted();
    qDebug() << "Function completed: Camera::restartCamera";
}

void Camera::refreshCamDevList()
{
    qDebug() << "Refreshing camera device list";
#if QT_VERSION_MAJOR > 5
    m_cameraDeviceList.clear();
    m_cameraDeviceList = QMediaDevices::videoInputs();
    qDebug() << "Found" << m_cameraDeviceList.size() << "camera devices";
#else
    m_cameraDevList.clear();

    v4l2_device_list_t *devlist = get_device_list();
    for (int i = 0; i < devlist->num_devices; i++)
        m_cameraDevList.push_back(devlist->list_devices[i].device);
    qDebug() << "Found" << m_cameraDevList.size() << "camera devices";
#endif
}

#if QT_VERSION_MAJOR <= 5
void Camera::onCameraStatusChanged(QCamera::Status status)
{
    qDebug() << "Camera status changed to:" << status;
    if (m_mediaRecoder) {
        qDebug() << "Media recorder exists, checking status";
        if (m_bReadyRecord && status == QCamera::ActiveStatus) {
            m_mediaRecoder->record();
            m_bReadyRecord = false;
        }
    }
}
#endif

QStringList Camera::getSupportResolutions()
{
    qDebug() << "Getting supported resolutions";
    QStringList resolutionsList;

    QList<QSize> supportResolutionList = getSupportResolutionsSize();
    if (supportResolutionList.isEmpty())
        qWarning() << "No supported resolutions found";

    int size = supportResolutionList.size();
    resolutionsList.clear();
    for (int i = 0; i < size; i++) {
        QString resol = QString("%1x%2").arg(supportResolutionList[size - 1 -i].width()).arg(supportResolutionList[size - 1 -i].height());
        resolutionsList.append(resol);
    }

    qDebug() << "Supported resolutions:" << resolutionsList;
    return resolutionsList;
}

QList<QSize> Camera::getSupportResolutionsSize()
{
    qDebug() << "Function started: Camera::getSupportResolutionsSize";
    QList<QSize> resolutions;
#if QT_VERSION_MAJOR > 5
    if (m_camera) {
        qDebug() << "Condition check: Camera instance exists";
        resolutions = m_camera->cameraDevice().photoResolutions();
    }
#else
    if (m_imageCapture)
        resolutions = m_imageCapture->supportedResolutions();
#endif

    qDebug() << "Function completed: Camera::getSupportResolutionsSize";
    return resolutions;
}

// 设置相机分辨率
void Camera::setCameraResolution(QSize size)
{
    qDebug() << "Setting camera resolution to:" << size;
#if QT_VERSION_MAJOR > 5
    //TODO
#else
    // 设置图片捕获器分辨率到相机
    if (m_imageCapture) {
        qDebug() << "Condition check: Setting image capture resolution";
        QImageEncoderSettings imageSettings = m_imageCapture->encodingSettings();
        imageSettings.setResolution(size);
        m_imageCapture->setEncodingSettings(imageSettings);
    }

           // 设置视频分辨率到相机
    if (m_mediaRecoder) {
        qDebug() << "Condition check: Setting media recorder resolution";
        QAudioEncoderSettings audioSettings = m_mediaRecoder->audioSettings();
        QVideoEncoderSettings videoSettings = m_mediaRecoder->videoSettings();
        videoSettings.setCodec("video/x-vp8");
        videoSettings.setResolution(size);
        m_mediaRecoder->setEncodingSettings(audioSettings, videoSettings, "video/webm");
    }

           // 设置分辨率到相机
    if (m_camera) {
        qDebug() << "Condition check: Setting camera viewfinder resolution";
        QCameraViewfinderSettings viewfinderSettings = m_camera->viewfinderSettings();
        viewfinderSettings.setResolution(size);
        m_camera->setViewfinderSettings(viewfinderSettings);
    }
#endif

    // 同步有变更的分辨率到config
    camConfig.width = size.rwidth();
    camConfig.height = size.rheight();
    saveConfig();
    qDebug() << "Camera resolution set successfully";
}

QSize Camera::getCameraResolution()
{
    qDebug() << "Getting camera resolution";
    return QSize(camConfig.width,camConfig.height);
}

#if QT_VERSION_MAJOR > 5
void Camera::startCamera(const QCameraDevice &device)
#else
void Camera::startCamera(const QString &devName)
#endif
{
    // 存在上一相机，先停止
    qDebug() << "Starting camera";
    if (m_camera) {
        qDebug() << "Stopping existing camera";
        stopCamera();
    }

#if QT_VERSION_MAJOR > 5
    m_camera = new QCamera(device);
    m_videoSink = new QVideoSink(this);
    m_curDevice = device;
    qDebug() << "Started camera device:" << device.description();
#else
    m_camera = new QCamera(devName.toStdString().c_str());
    QCameraInfo cameraInfo(*m_camera);
    connect(m_camera, SIGNAL(statusChanged(QCamera::Status)), this, SLOT(onCameraStatusChanged(QCamera::Status)));
    m_curDevName = devName;
    qDebug() << "Started camera device:" << m_curDevName;

    m_imageCapture = new QCameraImageCapture(m_camera);
    m_mediaRecoder = new QMediaRecorder(m_camera);

    QVideoEncoderSettings videoSettings = m_mediaRecoder->videoSettings();
    videoSettings.setCodec("video/x-vp8");
    QAudioEncoderSettings audioSettings = m_mediaRecoder->audioSettings();
    m_mediaRecoder->setEncodingSettings(audioSettings, videoSettings, "video/webm");

    m_camera->setCaptureMode(QCamera::CaptureStillImage);
    m_camera->setViewfinder(m_videoSurface);

    m_camera->start();

           // 同步设备名称到config
    if (camConfig.device_name) {
        free(camConfig.device_name);
        camConfig.device_name = nullptr;
    }
    if (camConfig.device_location) {
        free(camConfig.device_location);
        camConfig.device_location = nullptr;
    }
    camConfig.device_name = strdup(cameraInfo.description().toStdString().c_str());
    camConfig.device_location = strdup(cameraInfo.deviceName().toStdString().c_str());

    QList<QSize> supportList = getSupportResolutionsSize();

    // 当前设备与config设备名不同，重置分辨率到最大值，并同步到config文件
    QString configDevName = QString(camConfig.device_location);
    if (m_curDevName != configDevName
        || (m_curDevName.isEmpty() && m_curDevName == configDevName)) {
        if (!supportList.isEmpty()) {
            qDebug() << "Resetting resolution to maximum supported:" << supportList.last();
            camConfig.width = supportList.last().rwidth();
            camConfig.height = supportList.last().rheight();
        }
    }

           // 若当前摄像头不支持该分辨率，重置为当前摄像头最大分辨率
    if (!supportList.isEmpty()) {
        bool bResetResolution = true;
        for (int i = 0; i < supportList.size(); i++) {
            if (supportList[i].width() == camConfig.width &&
                supportList[i].height() == camConfig.height) {
                bResetResolution = false;
                break;
            }
        }
        if (bResetResolution) {
            qDebug() << "Current resolution not supported, resetting to:" << supportList.last();
            camConfig.width = supportList.last().width();
            camConfig.height = supportList.last().height();
        }
    }

           // 同步config分辨率到相机，并保存到文件
    setCameraResolution(QSize(camConfig.width, camConfig.height));

           // 发送相机名称变更信号
    emit cameraSwitched(cameraInfo.description());
#endif
}

void Camera::stopCamera()
{
    qDebug() << "Stopping camera";
    if (m_camera) {
#if QT_VERSION_MAJOR <= 5
        connect(m_camera, SIGNAL(statusChanged(QCamera::Status)), this, SLOT(onCameraStatusChanged(QCamera::Status)));
#endif
        m_camera->stop();
        m_camera->deleteLater();
        m_camera = nullptr;
    }

#if QT_VERSION_MAJOR > 5
#else
    if (m_imageCapture) {
        qDebug() << "Condition check: Image capture instance exists";
        m_imageCapture->deleteLater();
        m_imageCapture = nullptr;
    }
#endif

    if (m_mediaRecoder) {
        qDebug() << "Condition check: Media recorder instance exists";
        m_mediaRecoder->deleteLater();
        m_mediaRecoder = nullptr;
    }
    qDebug() << "Camera stopped successfully";
}

void Camera::setVideoOutPutPath(QString &path)
{
    qDebug() << "Setting video output path:" << path;
    if (m_mediaRecoder)
        m_mediaRecoder->setOutputLocation(QUrl::fromLocalFile(path));
}

void Camera::startRecoder()
{
    qDebug() << "Starting recorder";
#if QT_VERSION_MAJOR > 5
#else
    m_bReadyRecord = true;
    if (m_camera && m_camera->captureMode() != QCamera::CaptureVideo) {
        qDebug() << "Condition check: Setting camera to video capture mode";
        m_camera->setCaptureMode(QCamera::CaptureVideo);
    }

    m_bRecording = true;
    qDebug() << "Recorder started successfully";
#endif
}

void Camera::stopRecoder()
{
    qDebug() << "Stopping recorder";
#if QT_VERSION_MAJOR > 5
#else
    if (m_mediaRecoder)
        m_mediaRecoder->stop();
    if (m_camera && m_camera->captureMode() != QCamera::CaptureStillImage)
        m_camera->setCaptureMode(QCamera::CaptureStillImage);

    m_bRecording = false;
    qDebug() << "Recorder stopped successfully";
#endif
}

qint64 Camera::getRecoderTime()
{
    qDebug() << "Function started: getRecoderTime";
    qint64 nRecTime = 0;
    if (m_mediaRecoder)
        nRecTime = m_mediaRecoder->duration();

    qDebug() << "Function completed: getRecoderTime:" << nRecTime;
    return nRecTime;
}

#if QT_VERSION_MAJOR > 5
QMediaRecorder::RecorderState Camera::getRecoderState()
{
    qDebug() << "Function started: Camera::getRecoderState";
    QMediaRecorder::RecorderState recState = QMediaRecorder::StoppedState;
    if (m_mediaRecoder)
        recState = m_mediaRecoder->recorderState();

    qDebug() << "Function completed: Camera::getRecoderState";
    return recState;
}
#else
QMediaRecorder::State Camera::getRecoderState()
{
    qDebug() << "Function started: Camera::getRecoderState";
    QMediaRecorder::State recState = QMediaRecorder::StoppedState;
    if (m_mediaRecoder)
        recState = m_mediaRecoder->state();

    qDebug() << "Function completed: Camera::getRecoderState";
    return recState;
}
#endif

bool Camera::isRecording()
{
    // qDebug() << "Function started: Camera::isRecording";
    return m_bRecording;
}

bool Camera::isReadyRecord()
{
    // qDebug() << "Function started: Camera::isReadyRecord";
    return m_bReadyRecord;
}

int Camera::parseConfig()
{
    // qDebug() << "Parsing camera configuration";
    char *config_path = smart_cat(getenv("HOME"), '/', ".config/deepin/deepin-camera");
    mkdir(config_path, 0777);
    char *config_file = smart_cat(config_path, '/', "deepin-camera");
    /*释放字符串内存*/
    free(config_path);

    FILE *fp;
    char bufr[MAXLINE];
    int line = 0;

    /*open file for read*/
    if((fp = fopen(config_file,"r")) == NULL)
    {
        // qDebug() << "Condition check: Failed to open config file";
        free(config_file);
        qWarning() << "Could not open config file for reading:" << strerror(errno);
        return -1;
    }

    while(fgets(bufr, MAXLINE, fp) != NULL)
    {
        line++;
        char *bufp = bufr;

        /*trim leading and trailing spaces and newline*/
        trim_leading_wspaces(bufp);
        trim_trailing_wspaces(bufp);

        /*skip empty or commented lines */
        int size = strlen(bufp);
        if(size < 1 || *bufp == '#')
        {
            // qDebug() << "Condition check: Skipping empty or commented line" << line;
            continue;
        }

        char *token = NULL;
        char *value = NULL;

        char *sp = strrchr(bufp, '=');

        if(sp)
        {
            long size = sp - bufp;
            token = strndup(bufp, (ulong)size);
            trim_leading_wspaces(token);
            trim_trailing_wspaces(token);

            value = strdup(sp + 1);
            trim_leading_wspaces(value);
            trim_trailing_wspaces(value);
        }

        /*skip invalid lines */
        if(!token || !value || strlen(token) < 1 || strlen(value) < 1)
        {
            qWarning() << "Skipping invalid config entry at line" << line;
            if(token)
                free(token);
            if(value)
                free(value);
            continue;
        }

        /*check tokens*/
        if(strcmp(token, "width") == 0) {
            qDebug() << "Condition check: Setting width";
            camConfig.width = (int) strtoul(value, NULL, 10);
        } else if(strcmp(token, "height") == 0) {
            qDebug() << "Condition check: Setting height";
            camConfig.height = (int) strtoul(value, NULL, 10);
        } else if(strcmp(token, "device_name") == 0 && strlen(value) > 0) {
            qDebug() << "Condition check: Setting device_name";
            if(camConfig.device_name)
                free(camConfig.device_name);
            camConfig.device_name = strdup(value);
        } else if(strcmp(token, "device_location") == 0 && strlen(value) > 0) {
            qDebug() << "Condition check: Setting device_location";
            if(camConfig.device_location)
                free(camConfig.device_location);
            camConfig.device_location = strdup(value);
        }

        if(token)
            free(token);
        if(value)
            free(value);
    }

    free(config_file);
    fclose(fp);
    qDebug() << "Configuration parsed successfully";
    return 0;
}

int Camera::saveConfig()
{
    qDebug() << "Saving camera configuration";
    QString config_file = QString(getenv("HOME")) + QDir::separator() + QString(".config") + QDir::separator() + QString("deepin") +
                          QDir::separator() + QString("deepin-camera") + QDir::separator() + QString("deepin-camera");

    char *filename = strdup(config_file.toLatin1().data());
    FILE *fp;

    /*open file for write*/
    if((fp = fopen(filename,"w")) == NULL)
    {
        qWarning() << "Could not open config file for writing:" << strerror(errno);
        free(filename);
        return -1;
    }

    /* use c locale - make sure floats are writen with a "." and not a "," */
    setlocale(LC_NUMERIC, "C");

    /*write config data*/
    fprintf(fp, "\n");
    fprintf(fp, "#video input width\n");
    fprintf(fp, "width=%i\n", camConfig.width);
    fprintf(fp, "#video input height\n");
    fprintf(fp, "height=%i\n", camConfig.height);
    fprintf(fp, "#device name\n");
    fprintf(fp, "device_name=%s\n",camConfig.device_name);
    fprintf(fp, "#device location\n");
    fprintf(fp, "device_location=%s\n",camConfig.device_location);
    fprintf(fp, "#video name\n");
    fprintf(fp, "video_name=%s\n", camConfig.video_name);
    fprintf(fp, "#video path\n");
    fprintf(fp, "video_path=%s\n", camConfig.video_path);
    fprintf(fp, "#photo name\n");
    fprintf(fp, "photo_name=%s\n", camConfig.photo_name);
    fprintf(fp, "#photo path\n");
    fprintf(fp, "photo_path=%s\n", camConfig.photo_path);
    fprintf(fp, "#fps numerator (def. 1)\n");
    fprintf(fp, "fps_num=%i\n", camConfig.fps_num);
    fprintf(fp, "#fps denominator (def. 25)\n");
    fprintf(fp, "fps_denom=%i\n", camConfig.fps_denom);
    fprintf(fp, "#audio device index (-1 - api default)\n");

    /* return to system locale */
    setlocale(LC_NUMERIC, "");

    /* flush stream buffers to filesystem */
    fflush(fp);
    free(filename);

    /* close file after fsync (sync file data to disk) */
    if (fsync(fileno(fp)) || fclose(fp))
    {
        qWarning() << "Error writing configuration data to file:" << strerror(errno);
        return -1;
    }

    qDebug() << "Configuration saved successfully";
    return 0;
}
