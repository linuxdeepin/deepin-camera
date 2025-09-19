// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "videosurface.h"

#include <QtWidgets>
#include <qabstractvideosurface.h>
#include <qvideosurfaceformat.h>
#include <QVideoSurfaceFormat>

VideoSurface::VideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , imageFormat_(QImage::Format_Invalid)
{
    // qDebug() << "Function started: VideoSurface constructor";
}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    // qDebug() << "Function started: supportedPixelFormats";
    QList<QVideoFrame::PixelFormat> result;
    if (handleType == QAbstractVideoBuffer::NoHandle) {
        // qDebug() << "VideoSurface::supportedPixelFormats: Enter if branch (NoHandle)";
        result = QList<QVideoFrame::PixelFormat>()<< QVideoFrame::Format_RGB32<< QVideoFrame::Format_ARGB32<< QVideoFrame::Format_ARGB32_Premultiplied<< QVideoFrame::Format_RGB565<< QVideoFrame::Format_RGB555;
    } else {
        // qDebug() << "VideoSurface::supportedPixelFormats: Enter else branch (other handle type)";
        result = QList<QVideoFrame::PixelFormat>();
    }
    // qDebug() << "Function completed: supportedPixelFormats";
    return result;
}

bool VideoSurface::isFormatSupported(const QVideoSurfaceFormat & format) const
{
    // qDebug() << "Function started: isFormatSupported";
    bool result = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat()) != QImage::Format_Invalid && !format.frameSize().isEmpty() && format.handleType() == QAbstractVideoBuffer::NoHandle;
    // qDebug() << "Function completed: isFormatSupported";
    return result;
}

bool VideoSurface::start(const QVideoSurfaceFormat &format)
{
    // qDebug() << "Function started: start";
    const QImage::Format imageFormat_ = QVideoFrame::imageFormatFromPixelFormat(format.pixelFormat());
    const QSize size = format.frameSize();
    if (imageFormat_ != QImage::Format_Invalid && !size.isEmpty()) {
        // qDebug() << "VideoSurface::start: Enter if branch (valid format and size)";
        this->imageFormat_ = imageFormat_;
        QAbstractVideoSurface::start(format);
        return true;
    }
    // qDebug() << "Function completed: start, return false";
    return false;
}

void VideoSurface::stop()
{
    // qDebug() << "Function started: stop";
    currentFrame_ = QVideoFrame();
    QAbstractVideoSurface::stop();
    // qDebug() << "Function completed: stop";
}

bool VideoSurface::present(const QVideoFrame &frame) //每一帧摄像头的数据，都会经过这里
{
    // qDebug() << "Function started: present";
    if (surfaceFormat().pixelFormat() != frame.pixelFormat() || surfaceFormat().frameSize() != frame.size()) {
        // qDebug() << "VideoSurface::present: Enter if branch (format mismatch, stopping)";
        setError(IncorrectFormatError);
        stop();
        return false;
    }
    currentFrame_ = frame;

    if (currentFrame_.map(QAbstractVideoBuffer::ReadOnly)) {
        // qDebug() << "VideoSurface::present: Enter if branch (frame mapped successfully)";
        //img就是转换的数据了
        QImage img = QImage(currentFrame_.bits(),currentFrame_.width(),currentFrame_.height(),currentFrame_.bytesPerLine(),imageFormat_).mirrored(true,false);
        emit presentImage(img);
        currentFrame_.unmap();
    }

    // qDebug() << "Function completed: present";
    return true;
}
