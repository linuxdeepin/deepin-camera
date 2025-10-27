// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DBUS_ADAPTOR
#define DBUS_ADAPTOR

#include "mainwindow.h"

#include <QtDBus>

/**
* @brief CloseDialog　窗口大小适配
* 窗口最小化后双击桌面图标弹出相应大小的窗口
*/
class ApplicationAdaptor: public QDBusAbstractAdaptor
{
    Q_OBJECT

public:
    explicit ApplicationAdaptor(CMainWindow *mw = nullptr);
    Q_CLASSINFO("D-Bus Interface", "com.deepin.Camera")

    ~ApplicationAdaptor();

public slots:
    /**
    * @brief showWindow 显示窗口
    */
    bool showWindow();

    /**
    * @brief hideWindow 隐藏窗口
    */
    bool hideWindow();

    /**
    * @brief raiseWindow 提升窗口
    */
    bool raiseWindow();

    /**
    * @brief quitWindow 退出窗口
    */
    bool quitWindow();

    /**
    * @brief takePhoto 拍照
    */
    bool takePhoto();

    /**
    * @brief startRecording 开始录像
    */
    bool startRecording();

    /**
    * @brief stopRecording 停止录像
    */
    bool stopRecording();

    /**
    * @brief getRecordingStatus 获取录像状态
    */
    bool getRecordingStatus();

Q_SIGNALS:
    /**
    * @brief recordingStatusChanged 录像状态改变信号
    */
    void recordingStatusChanged(bool isRecording);

    /**
    * @brief photoTaken 拍照完成信号
    */
    void photoTaken(const QString &filePath);

    /**
    * @brief recordingStarted 录像开始信号
    */
    void recordingStarted(const QString &filePath);

    /**
    * @brief recordingStopped 录像停止信号
    */
    void recordingStopped(const QString &filePath);

private slots:
    /**
     * @brief emitPhotoTaken Emit photoTaken signal in the main thread
     * @param filePath The path of the taken photo
     */
    void emitPhotoTaken(const QString &filePath);

    void emitRecordingStarted(const QString &filePath);

    void emitRecordingStopped(const QString &filePath);

private:
    CMainWindow *m_mw;
};


#endif /* ifndef DBUS_ADAPTOR */

