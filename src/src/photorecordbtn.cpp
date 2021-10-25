/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     wuzhigang <wuzhigang@uniontech.com>
*             
* Maintainer: wuzhigang <wuzhigang@uniontech.com>
*             
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

#include "photorecordbtn.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QSvgRenderer>

#include <DApplicationHelper>

photoRecordBtn::photoRecordBtn(QWidget *parent/* = nullptr*/)
    :QWidget (parent),
      m_bPhoto(true),
      m_bFocus(false),
      m_bPress(false),
      m_recordState(Normal)
{

}



void photoRecordBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
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
        if (Normal == m_recordState){
            rect.setTopLeft(QPoint(11, 11));
            rect.setSize(QSize(42, 42));
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(QColor(0xff,0x57,0x36,0xff)));
            painter.drawEllipse(rect);
        } else if (preRecord == m_recordState){
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
        rect.setTopLeft(QPoint(11, 11));
        rect.setSize(QSize(42, 42));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(Qt::white)));
        painter.drawEllipse(rect);
    }
}

void photoRecordBtn::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}

void photoRecordBtn::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void photoRecordBtn::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void photoRecordBtn::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bFocus= true;
    update();
    emit clicked();
}

void photoRecordBtn::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus= true;
    update();
}

void photoRecordBtn::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus= false;
    update();
}
