// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "previewopenglwidget.h"

#if QT_VERSION_MAJOR > 5
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLTexture>
#else
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#endif

#include <malloc.h>

namespace {

constexpr const char *kCompatDefs =
    "#if __VERSION__ < 130\n"
    "  #define IN attribute\n"
    "  #define OUT varying\n"
    "  #define FRAG_COLOR gl_FragColor\n"
    "  #define TEXTURE texture2D\n"
    "#else\n"
    "  #define IN in\n"
    "  #define OUT out\n"
    "  #define FRAG_COLOR fragColor\n"
    "  #define TEXTURE texture\n"
    "#endif\n";

constexpr const char *kVsrcLegacy =
    "attribute vec4 vertexIn;   "
    "attribute vec2 textureIn;  "
    "varying vec2 textureOut;   "
    "void main(void)            "
    "{                          "
    "    gl_Position = vertexIn;"
    "    textureOut = textureIn;"
    "}";

constexpr const char *kVsrcModern =
    "#version 150 core\n"
    "in vec4 vertexIn;\n"
    "in vec2 textureIn;\n"
    "out vec2 textureOut;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = vertexIn;\n"
    "    textureOut = textureIn;\n"
    "}\n";

constexpr const char *kFsrcBody =
    "uniform sampler2D tex_y;                       \n"
    "uniform sampler2D tex_u;                       \n"
    "uniform sampler2D tex_v;                       \n"
    "void main(void)                                \n"
    "{                                              \n"
    "    vec3 yuv;                                  \n"
    "    vec3 rgb;                                  \n"
    "    yuv.x = TEXTURE(tex_y, textureOut).r;       \n"
    "    yuv.y = TEXTURE(tex_u, textureOut).r - 0.5;\n"
    "    yuv.z = TEXTURE(tex_v, textureOut).r - 0.5;\n"
    "    rgb = mat3( 1,       1,         1,         \n"
    "                0,       -0.39465,  2.03211,   \n"
    "                1.13983, -0.58060,  0) * yuv;  \n"
    "    FRAG_COLOR = vec4(rgb, 1);                 \n"
    "}\n";

QByteArray makeLegacyFragmentShader(bool gles)
{
    QByteArray src;
    src.reserve(512);
    if (gles) {
        src.append("precision mediump float;\n");
    }
    src.append(kCompatDefs);
    src.append("varying vec2 textureOut;\n");
    src.append(kFsrcBody);
    return src;
}

QByteArray makeModernFragmentShader()
{
    QByteArray src;
    src.reserve(640);
    src.append("#version 150 core\n");
    src.append("in vec2 textureOut;\n");
    src.append("out vec4 fragColor;\n");
    src.append(kFsrcBody);
    return src;
}

} // namespace

PreviewOpenglWidget::PreviewOpenglWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    qDebug() << "Function started: PreviewOpenglWidget constructor";
    m_textureY = nullptr;
    m_textureU = nullptr;
    m_textureV = nullptr;
    m_program = nullptr;
    m_videoWidth = 0;
    m_videoHeight = 0;
    qDebug() << "Function completed: PreviewOpenglWidget constructor";
}

int PreviewOpenglWidget::getFrameHeight()
{
    // qDebug() << "Function started: getFrameHeight";
    return static_cast<int>(m_videoHeight);
}

int PreviewOpenglWidget::getFrameWidth()
{
    // qDebug() << "Function started: getFrameWidth";
    return static_cast<int>(m_videoWidth);
}

#ifndef __mips__
void PreviewOpenglWidget::slotShowYuv(std::shared_ptr<uchar[]> frame, uint width, uint height)
{
    if (!frame) {
        return;
    }
    {
        QMutexLocker locker(&m_Rendermutex);
        m_videoWidth = width;
        m_videoHeight = height;
        m_yuvFrame = std::move(frame);
    }
    update();
}
#endif

void PreviewOpenglWidget::initializeGL()
{
    qDebug() << "Function started: initializeGL";

    makeCurrent();
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    static const GLfloat vertices[] {
        //顶点坐标
        -1.0f, -1.0f,
            -1.0f, +1.0f,
            +1.0f, +1.0f,
            +1.0f, -1.0f,
            //纹理坐标
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
        };

    m_vao.create();
    m_vao.bind();
    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));

    m_program = new QOpenGLShaderProgram(this);
    m_textureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_textureV = new QOpenGLTexture(QOpenGLTexture::Target2D);

    auto bindAttribs = [this] {
        m_program->bindAttributeLocation("vertexIn", VERTEXIN);
        m_program->bindAttributeLocation("textureIn", TEXTUREIN);
    };
    auto tryLink = [&](const char *vsrc, const QByteArray &fsrc) {
        bindAttribs();
        return m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vsrc)
            && m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fsrc)
            && m_program->link();
    };

    bool programOk = false;
    if (1 == is_forceGles()) {
        programOk = tryLink(kVsrcLegacy, makeLegacyFragmentShader(true));
        if (!programOk) {
            qWarning() << "PreviewOpenglWidget: GLES shader failed, fallback to legacy GLSL:" << m_program->log();
            delete m_program;
            m_program = new QOpenGLShaderProgram(this);
            programOk = tryLink(kVsrcLegacy, makeLegacyFragmentShader(false));
        }
    } else {
        programOk = tryLink(kVsrcModern, makeModernFragmentShader());
        if (!programOk) {
            qWarning() << "PreviewOpenglWidget: core-profile shader failed, fallback to legacy GLSL:" << m_program->log();
            delete m_program;
            m_program = new QOpenGLShaderProgram(this);
            programOk = tryLink(kVsrcLegacy, makeLegacyFragmentShader(false));
        }
    }
    if (!programOk) {
        qWarning() << "PreviewOpenglWidget: shader program link failed:" << m_program->log();
    }

    m_program->bind();
    m_program->enableAttributeArray(VERTEXIN);
    m_program->enableAttributeArray(TEXTUREIN);
    constexpr int kVertexOffset = 0;
    constexpr int kTexOffset = 8 * sizeof(GLfloat);
    m_program->setAttributeBuffer(VERTEXIN, GL_FLOAT, kVertexOffset, 2, 2 * sizeof(GLfloat));
    m_program->setAttributeBuffer(TEXTUREIN, GL_FLOAT, kTexOffset, 2, 2 * sizeof(GLfloat));

    m_textureUniformY = static_cast<uint>(m_program->uniformLocation("tex_y"));
    m_textureUniformU = static_cast<uint>(m_program->uniformLocation("tex_u"));
    m_textureUniformV = static_cast<uint>(m_program->uniformLocation("tex_v"));

    m_textureY->create();
    m_textureU->create();
    m_textureV->create();
    m_idY = m_textureY->textureId();
    m_idU = m_textureU->textureId();
    m_idV = m_textureV->textureId();

    //纹理过滤与环绕参数只需在初始化时设置一次，避免每帧重复下发造成卡顿
    GLuint texIds[] = {m_idY, m_idU, m_idV};
    for (GLuint id : texIds) {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    qDebug() << "Function completed: initializeGL";
}

void PreviewOpenglWidget::resizeGL(int w, int h)
{
    // qDebug() << "Function started: resizeGL";
    glViewport(0, 0, w, h);
    // qDebug() << "Function completed: resizeGL";
}

void PreviewOpenglWidget::paintGL()
{
    std::shared_ptr<uchar[]> localFrame;
    uint localWidth = 0;
    uint localHeight = 0;
    {
        QMutexLocker locker(&m_Rendermutex);
        if (!m_yuvFrame) {
            return;
        }
        localFrame = m_yuvFrame;
        localWidth = m_videoWidth;
        localHeight = m_videoHeight;
    }
    uchar *localYuvPtr = localFrame.get();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bool bReBuildImage = false;
    if(m_imgSize != QSize(localWidth, localHeight)) {
        bReBuildImage = true;
        m_imgSize = QSize(localWidth, localHeight);
    }

    m_vao.bind();
    m_program->bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_idY);

    if( bReBuildImage ) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<int>(localWidth), static_cast<int>(localHeight), 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, static_cast<int>(localWidth), static_cast<int>(localHeight), GL_RED, GL_UNSIGNED_BYTE, localYuvPtr);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_idU);

    if( bReBuildImage ) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, localWidth >> 1, localHeight >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0,  0, 0, localWidth >> 1, localHeight >> 1, GL_RED, GL_UNSIGNED_BYTE, localYuvPtr + localWidth * localHeight);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_idV);

    if( bReBuildImage ) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, localWidth >> 1, localHeight >> 1, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0,  0, 0, localWidth >> 1, localHeight >> 1, GL_RED, GL_UNSIGNED_BYTE, localYuvPtr + localWidth * localHeight * 5 / 4);

    glUniform1i(static_cast<int>(m_textureUniformY), 0);
    glUniform1i(static_cast<int>(m_textureUniformU), 1);
    glUniform1i(static_cast<int>(m_textureUniformV), 2);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


PreviewOpenglWidget::~PreviewOpenglWidget()
{
    makeCurrent();
    m_vao.destroy();
    m_vbo.destroy();

    if (m_textureY) {
        m_textureY->destroy();
        delete m_textureY;
        m_textureY = nullptr;
    }

    if (m_textureU) {
        m_textureU->destroy();
        delete m_textureU;
        m_textureU = nullptr;
    }

    if (m_textureV) {
        m_textureV->destroy();
        delete m_textureV;
        m_textureV = nullptr;
    }

    if (m_program) {
        delete m_program;
        m_program = nullptr;
    }
    doneCurrent();
}
