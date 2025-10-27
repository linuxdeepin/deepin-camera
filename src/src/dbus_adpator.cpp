// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbus_adpator.h"
#include "videowidget.h"
#include "photorecordbtn.h"
#include "datamanager.h"

ApplicationAdaptor::ApplicationAdaptor(CMainWindow *mw)
    : QDBusAbstractAdaptor(mw), m_mw(mw)
{
    if (m_mw) {
        connect(m_mw->getVideoWidget(), &videowidget::reflushSnapshotLabel, [=](const QString &filePath) {
            QMetaObject::invokeMethod(this, "emitPhotoTaken", Qt::QueuedConnection, Q_ARG(QString, filePath));
        });

        connect(m_mw->getVideoWidget(), &videowidget::updateRecordState, [=](int state) {
            if (state == 2) {
                QString fileName = DataManager::instance()->getstrFileName();
                if (!fileName.isEmpty()) {
                    QString videoPath = m_mw->getVideoWidget()->getSaveVdFolder();
                    QString filePath = videoPath + "/" + fileName;
                    QMetaObject::invokeMethod(this, "emitRecordingStarted", Qt::QueuedConnection, Q_ARG(QString, filePath));
                }
            }
            else if (state == 0) {
                QString fileName = DataManager::instance()->getstrFileName();
                if (!fileName.isEmpty()) {
                    QString videoPath = m_mw->getVideoWidget()->getSaveVdFolder();
                    QString filePath = videoPath + "/" + fileName;
                    QMetaObject::invokeMethod(this, "emitRecordingStopped", Qt::QueuedConnection, Q_ARG(QString, filePath));
                }
            }
        });
    }
}

bool ApplicationAdaptor::showWindow()
{
    if (m_mw) {
        m_mw->showNormal();
        m_mw->activateWindow();
        return true;
    }
    return false;
}

bool ApplicationAdaptor::hideWindow()
{
    if (m_mw) {
        m_mw->hide();
        return true;
    }
    return false;
}

bool ApplicationAdaptor::raiseWindow()
{
    if (m_mw) {
        m_mw->showNormal();
        m_mw->raise();
        m_mw->activateWindow();
        return true;
    }
    return false;
}

bool ApplicationAdaptor::quitWindow()
{
    if (m_mw) {
        m_mw->close();
        return true;
    }
    return false;
}

bool ApplicationAdaptor::takePhoto()
{
    if (m_mw) {
        m_mw->onSwitchPhotoBtnClked();
        videowidget *videoWidget = m_mw->getVideoWidget();
        if (videoWidget) {
            videoWidget->onTakePic(true);
            return true;
        }
    }
    return false;
}

bool ApplicationAdaptor::startRecording()
{
    if (m_mw) {
        m_mw->onSwitchRecordBtnClked();
        videowidget *videoWidget = m_mw->getVideoWidget();
        if (videoWidget) {
            if (!videoWidget->getCapStatus()) {
                videoWidget->onTakeVideo();
                return true;
            }
        }
    }
    return false;
}

bool ApplicationAdaptor::stopRecording()
{
    if (m_mw) {
        videowidget *videoWidget = m_mw->getVideoWidget();
        if (videoWidget) {
            if (videoWidget->getCapStatus()) {
                videoWidget->onEndBtnClicked();
                return true;
            }
        }
    }
    return false;
}

bool ApplicationAdaptor::getRecordingStatus()
{
    if (m_mw) {
        videowidget *videoWidget = m_mw->getVideoWidget();
        if (videoWidget) {
            return videoWidget->getCapStatus();
        }
    }
    return false;
}

void ApplicationAdaptor::emitPhotoTaken(const QString &filePath)
{
    emit photoTaken(filePath);
}

void ApplicationAdaptor::emitRecordingStarted(const QString &filePath)
{
    emit recordingStarted(filePath);
}

void ApplicationAdaptor::emitRecordingStopped(const QString &filePath)
{
    emit recordingStopped(filePath);
}

ApplicationAdaptor::~ApplicationAdaptor()
{
    if (m_mw) {
        m_mw->deleteLater();
        m_mw = nullptr;
    }
}
