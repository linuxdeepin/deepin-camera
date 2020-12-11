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

#ifndef PREVIEWOPENGLWIDGET_H
#define PREVIEWOPENGLWIDGET_H

#include <QObject>
#include <QWidget>
#include <QMutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>


#include "camview.h"


QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class PreviewOpenglWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    PreviewOpenglWidget(QWidget *parent = nullptr);
    ~PreviewOpenglWidget()Q_DECL_OVERRIDE;
    int getFrameHeight();
    int getFrameWidth();

public slots:
    void slotShowYuv(uchar *ptr, uint width, uint height); //显示一帧Yuv图像

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
private:
    QMutex m_Rendermutex;

    QOpenGLShaderProgram *program;
    QOpenGLBuffer vbo;

    GLuint textureUniformY;
    GLuint textureUniformU;
    GLuint textureUniformV; //opengl中y、u、v分量位置

    QOpenGLTexture *textureY = nullptr;
    QOpenGLTexture *textureU = nullptr;
    QOpenGLTexture *textureV = nullptr;

    GLuint idY;
    GLuint idU;
    GLuint idV; //自己创建的纹理对象ID，创建错误返回0

    uint m_videoWidth;
    uint m_videoHeight;

    uchar *m_yuvPtr = nullptr;
};

#endif // PREVIEWOPENGLWIDGET_H
