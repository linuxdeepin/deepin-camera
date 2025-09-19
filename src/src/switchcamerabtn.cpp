// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "switchcamerabtn.h"
#include <QEnterEvent>
#include <QToolTip>
#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QSvgRenderer>

SwitchCameraBtn::SwitchCameraBtn(QWidget *parent/* = nullptr*/)
    :QWidget (parent),
      m_bFocus(false),
      m_bPressed(false)
{
    // qDebug() << "Function started: SwitchCameraBtn constructor";
    m_normalSvg = new QSvgRenderer(QString(":/images/icons/light/switch_camera.svg"));
    m_pressedSvg = new QSvgRenderer(QString(":/images/icons/light/switch_camera_press.svg"));
    // qDebug() << "Function completed: SwitchCameraBtn constructor";
}

void SwitchCameraBtn::paintEvent(QPaintEvent *event)
{
    // qDebug() << "Function started: SwitchCameraBtn::paintEvent";
    Q_UNUSED(event);
    QPainter painter(this);
#if QT_VERSION_MAJOR <= 5
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
#endif

    QPainterPath path;
    if (nullptr == m_normalSvg
        || nullptr == m_pressedSvg){
        // qDebug() << "normalSvg or pressedSvg is nullptr, return";
        return;
    }
    QRect rect = this->rect();
    int offset1 = 2;
    int offset2 = 4;
    QRect rect1,rect2;
    QColor color = QColor(0,0,0,10);
    if (m_bFocus && ! m_bPressed) {
       color = QColor(0,0x81,0xff,255);
    }
    painter.setPen(QPen(color, offset1));
    rect1.setTopLeft(QPoint(rect.top()+offset1/2, rect.left()+offset1/2));
    rect1.setSize(QSize(rect.width()-offset1, rect.height()-offset1));
    painter.drawEllipse(rect1);

    if (m_bFocus && ! m_bPressed) {
        // qDebug() << "Entering if (m_bFocus && ! m_bPressed)";
        rect2.setTopLeft(QPoint(rect1.x()+offset2/2, rect1.y()+offset2/2));
        rect2.setSize(QSize(rect1.width()-offset2, rect1.height()-offset2));
        painter.setPen(QPen(QColor(255,255,255,255), offset2/2));
        painter.drawEllipse(rect2);
        QRect rect3;
        rect3.setTopLeft(QPoint(rect2.x()+offset2/2, rect2.y()+offset2/2));
        rect3.setSize(QSize(rect2.width()-offset2, rect2.height()-offset2));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(0,0,0,100)));
        painter.drawEllipse(rect3);
    } else {
        // qDebug() << "Entering else";
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(0,0,0,100)));
        painter.drawEllipse(rect);
    }

    if (m_bPressed) {
        // qDebug() << "Entering if (m_bPressed)";
        m_pressedSvg->render(&painter);
    } else {
        // qDebug() << "Entering else";
        m_normalSvg->render(&painter);
    }
    // qDebug() << "Function completed: SwitchCameraBtn::paintEvent";
}

#if QT_VERSION_MAJOR > 5
void SwitchCameraBtn::enterEvent(QEnterEvent *event)
#else
void SwitchCameraBtn::enterEvent(QEvent *event)
#endif
{
    // qDebug() << "Function started: SwitchCameraBtn::enterEvent";
    Q_UNUSED(event);
    m_bFocus = true;
    update();
    QEnterEvent *enterEvent = static_cast<QEnterEvent*>(event);
    QToolTip::showText(enterEvent->globalPos(), toolTip(),this);
    // qDebug() << "Function completed: SwitchCameraBtn::enterEvent";
}

void SwitchCameraBtn::leaveEvent(QEvent *event)
{
    // qDebug() << "Function started: SwitchCameraBtn::leaveEvent";
    Q_UNUSED(event);
    m_bFocus = false;
    update();
    // qDebug() << "Function completed: SwitchCameraBtn::leaveEvent";
}

void SwitchCameraBtn::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: SwitchCameraBtn::mousePressEvent";
    Q_UNUSED(event);
    m_bPressed = true;
    update();
    // qDebug() << "Function completed: SwitchCameraBtn::mousePressEvent";
}

void SwitchCameraBtn::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: SwitchCameraBtn::mouseReleaseEvent";
    Q_UNUSED(event);
    m_bPressed= false;
    update();
    emit clicked();
    // qDebug() << "Function completed: SwitchCameraBtn::mouseReleaseEvent";
}

void SwitchCameraBtn::focusInEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: SwitchCameraBtn::focusInEvent";
    Q_UNUSED(event);
    m_bFocus= true;
    update();
    // qDebug() << "Function completed: SwitchCameraBtn::focusInEvent";
}

void SwitchCameraBtn::focusOutEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: SwitchCameraBtn::focusOutEvent";
    Q_UNUSED(event);
    m_bFocus= false;
    update();
    // qDebug() << "Function completed: SwitchCameraBtn::focusOutEvent";
}

SwitchCameraBtn::~SwitchCameraBtn()
{
    // qDebug() << "Function started: SwitchCameraBtn destructor";
    if (nullptr != m_normalSvg) {
        // qDebug() << "SwitchCameraBtn destructor: Enter if branch (deleting normal svg)";
        m_normalSvg->deleteLater();
        m_normalSvg = nullptr;
    }

    if (nullptr != m_pressedSvg) {
        // qDebug() << "SwitchCameraBtn destructor: Enter if branch (deleting pressed svg)";
        m_pressedSvg->deleteLater();
        m_pressedSvg = nullptr;
    }
    // qDebug() << "Function completed: SwitchCameraBtn destructor";
}
