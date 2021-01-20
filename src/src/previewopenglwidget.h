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

#include "camview.h"

#include <QObject>
#include <QWidget>
#include <QMutex>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

/**
* @brief PreviewOpenglWidget　Opengl用于图像显示
*/
class PreviewOpenglWidget: public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    PreviewOpenglWidget(QWidget *parent = nullptr);

    ~PreviewOpenglWidget()Q_DECL_OVERRIDE;
    /**
    * @brief getFrameHeight　获取宽度
    */
    int getFrameHeight();

    /**
    * @brief getFrameWidth　获取高度
    */
    int getFrameWidth();

public slots:
    /**
    * @brief slotShowYuv　显示一帧Yuv图像
    * @param ptr 数据
    * @param width 宽度
    * @param height 高度
    */
    void slotShowYuv(uchar *ptr, uint width, uint height);

protected:
    /**
    * @brief initializeGL　初始化openGL
    */
    void initializeGL() Q_DECL_OVERRIDE;

    /**
    * @brief resizeGL　重新定义openGL大小
    * @param w 宽度
    * @param h 高度
    */
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    /**
    * @brief paintGL　绘图
    */
    void paintGL() Q_DECL_OVERRIDE;

private:
    QMutex               m_Rendermutex;
    QOpenGLBuffer        m_vbo;
    QOpenGLShaderProgram *m_program;


    //opengl中y、u、v分量位置
    GLuint m_textureUniformY;
    GLuint m_textureUniformU;
    GLuint m_textureUniformV;

    QOpenGLTexture *m_textureY;
    QOpenGLTexture *m_textureU;
    QOpenGLTexture *m_textureV;

    //自己创建的纹理对象ID，创建错误返回0
    GLuint m_idY;
    GLuint m_idU;
    GLuint m_idV;

    uint m_videoWidth;
    uint m_videoHeight;

    uchar *m_yuvPtr;
};

#endif // PREVIEWOPENGLWIDGET_H
