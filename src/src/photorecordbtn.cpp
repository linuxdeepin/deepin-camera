// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "photorecordbtn.h"

#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#else
#include <DGuiApplicationHelper>
#endif

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QMouseEvent>

photoRecordBtn::photoRecordBtn(QWidget *parent/* = nullptr*/)
    :QWidget (parent),
      m_bPhoto(true),
      m_bFocus(false),
      m_bPress(false),
      m_recordState(Normal)
{
    // qDebug() << "Function started: photoRecordBtn constructor";
}



void photoRecordBtn::paintEvent(QPaintEvent *event)
{
    // qDebug() << "Function started: paintEvent";
    Q_UNUSED(event);
    QPainter painter(this);
#if QT_VERSION_MAJOR <= 5
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
#else
    painter.setRenderHint(QPainter::Antialiasing, true);
#endif
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    QRectF rect = this->rect();

    QColor color = m_bFocus ? Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color() : QColor(0, 0, 0, 0.1 * 255);
    painter.setPen(QPen(color, 2));
    painter.setBrush(Qt::NoBrush);
    rect.setTopLeft(QPoint(1, 1));
    rect.setSize(QSize(62, 62));
    painter.drawEllipse(rect);

    rect.setTopLeft(QPoint(4, 4));
    rect.setSize(QSize(56, 56));
    painter.setPen(QPen(Qt::white, 4));
    painter.drawEllipse(rect);

    rect.setTopLeft(QPoint(6, 6));
    rect.setSize(QSize(52, 52));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255, 0.1 * 255)));
    painter.drawEllipse(rect);

    if (!m_bPhoto)
    {
        // qDebug() << "Entering if (!m_bPhoto)";
        if (Normal == m_recordState){
            // qDebug() << "Entering if (Normal == m_recordState)";
            rect.setTopLeft(QPoint(11, 11));
            rect.setSize(QSize(42, 42));
            if (m_bPress) {
                // qDebug() << "Entering if (m_bPress)";
                rect.setTopLeft(QPoint(13, 13));
                rect.setSize(QSize(38,38));
            }
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(0xff,0x57,0x36,0xff)));
            painter.drawEllipse(rect);
        } else if (preRecord == m_recordState){
            // qDebug() << "Entering if (preRecord == m_recordState)";
            rect.setTopLeft(QPoint(20, 20));
            rect.setSize(QSize(24, 24));
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 255 * 0.08));
            painter.drawRoundedRect(rect, 5, 5);

            rect.setTopLeft(QPoint(22, 22));
            rect.setSize(QSize(20,20));
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(255,255,255,0xff)));
            painter.drawRoundedRect(rect,4,4);
        } else if (Recording == m_recordState){
            // qDebug() << "Entering if (Recording == m_recordState)";
            rect.setTopLeft(QPoint(11, 11));
            rect.setSize(QSize(42, 42));
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(255, 255, 255)));
            painter.drawEllipse(rect);

            rect.setTopLeft(QPoint(23, 23));
            rect.setSize(QSize(18,18));
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(0xff,0x57,0x36,0xff)));
            painter.drawRoundedRect(rect, 4, 4);
        }

    } else {
        // qDebug() << "Entering else";
        rect.setTopLeft(QPoint(11, 11));
        rect.setSize(QSize(42, 42));
        if (m_bPress) {
            // qDebug() << "Entering if (m_bPress)";
            rect.setTopLeft(QPoint(13, 13));
            rect.setSize(QSize(38,38));
        }
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(Qt::white)));
        painter.drawEllipse(rect);
    }
    // qDebug() << "Function completed: photoRecordBtn::paintEvent";
}

#if QT_VERSION_MAJOR > 5
void photoRecordBtn::enterEvent(QEnterEvent *event)
#else
void photoRecordBtn::enterEvent(QEvent *event)
#endif
{
    // qDebug() << "Function started: photoRecordBtn::enterEvent";
    Q_UNUSED(event);
    m_bFocus = true;
    update();
    // qDebug() << "Function completed: photoRecordBtn::enterEvent";
}

void photoRecordBtn::leaveEvent(QEvent *event)
{
    // qDebug() << "Function started: photoRecordBtn::leaveEvent";
    Q_UNUSED(event);
    m_bFocus = false;
    update();
    // qDebug() << "Function completed: photoRecordBtn::leaveEvent";
}

void photoRecordBtn::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: photoRecordBtn::mousePressEvent";
    if (event->type() == QEvent::MouseButtonDblClick) {
        // qDebug() << "photoRecordBtn::mousePressEvent: Enter if branch (mouse double click, returning early)";
        return; //不响应双击事件
    }

    // qDebug() << "photoRecordBtn::mousePressEvent: Setting focus to false and press to true";
    m_bFocus = false;

    m_bPress = true;

    update();
    // qDebug() << "Function completed: photoRecordBtn::mousePressEvent";
}

void photoRecordBtn::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: photoRecordBtn::mouseMoveEvent";
    //解决bug 在按钮中可拖动相机界面，bug 100647
    Q_UNUSED(event);
    // qDebug() << "Function completed: photoRecordBtn::mouseMoveEvent";
    return;
}

void photoRecordBtn::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: photoRecordBtn::mouseReleaseEvent";
    m_bPress = false;

    if (event->type() == QEvent::MouseButtonDblClick) {
        // qDebug() << "photoRecordBtn::mouseReleaseEvent: Enter if branch (mouse double click, returning early)";
        return; //不响应双击事件
    }

    if (!rect().contains(event->pos())) {
        // qDebug() << "photoRecordBtn::mouseReleaseEvent: Enter if branch (position not in rect, returning early)";
        return;
    }

    // qDebug() << "photoRecordBtn::mouseReleaseEvent: Setting focus to true and emitting clicked signal";
    m_bFocus= true;
    update();
    emit clicked();
    // qDebug() << "Function completed: photoRecordBtn::mouseReleaseEvent";
}

void photoRecordBtn::focusInEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: photoRecordBtn::focusInEvent";
    Q_UNUSED(event);
    m_bFocus= true;
    update();
    // qDebug() << "Function completed: photoRecordBtn::focusInEvent";
}

void photoRecordBtn::focusOutEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: photoRecordBtn::focusOutEvent";
    Q_UNUSED(event);
    m_bFocus= false;
    update();
    // qDebug() << "Function completed: photoRecordBtn::focusOutEvent";
}
