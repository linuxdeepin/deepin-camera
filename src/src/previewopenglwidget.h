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

class PreviewOpenglWidget:public QOpenGLWidget, protected QOpenGLFunctions
{
     Q_OBJECT
public:
    PreviewOpenglWidget(QWidget *parent = nullptr);
    ~PreviewOpenglWidget()Q_DECL_OVERRIDE;

public slots:
    void slotShowYuv(uchar *ptr,uint width,uint height); //显示一帧Yuv图像

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

    uint videoW;
    uint videoH;

    uchar *yuvPtr = nullptr;
};

#endif // PREVIEWOPENGLWIDGET_H
