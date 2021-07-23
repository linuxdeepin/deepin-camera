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
    m_normalSvg = new QSvgRenderer(QString(":/images/icons/light/switch_camera.svg"));
    m_pressedSvg = new QSvgRenderer(QString(":/images/icons/light/switch_camera_press.svg"));
}

SwitchCameraBtn::~SwitchCameraBtn()
{
    if (nullptr != m_normalSvg) {
        m_normalSvg->deleteLater();
        m_normalSvg = nullptr;
    }

    if (nullptr != m_pressedSvg) {
        m_pressedSvg->deleteLater();
        m_pressedSvg = nullptr;
    }
}

void SwitchCameraBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    QPainterPath path;
    if (nullptr == m_normalSvg
        || nullptr == m_pressedSvg){
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
        rect2.setTopLeft(QPoint(rect1.x()+offset2/2, rect1.y()+offset2/2));
        rect2.setSize(QSize(rect1.width()-offset2, rect1.height()-offset2));
        painter.setPen(QPen(QColor(255,255,255,255), offset2/2));
        painter.drawEllipse(rect2);
        QRect rect3;
        rect3.setTopLeft(QPoint(rect2.x()+offset2/2, rect2.y()+offset2/2));
        rect3.setSize(QSize(rect2.width()-offset2, rect2.height()-offset2));
        path.addEllipse(rect3);
        painter.fillPath(path, QBrush(QColor(0,0,0,100)));
    }
    else{
        path.addEllipse(rect);
        painter.fillPath(path, QBrush(QColor(0,0,0,100)));
    }

    if (m_bPressed)
    {
        m_pressedSvg->render(&painter);
    }
    else {
        m_normalSvg->render(&painter);
    }
}

void SwitchCameraBtn::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
    QEnterEvent *enterEvent = static_cast<QEnterEvent*>(event);
    QToolTip::showText(enterEvent->globalPos(), toolTip(),this);
}

void SwitchCameraBtn::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void SwitchCameraBtn::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bPressed = true;
    update();
}

void SwitchCameraBtn::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bPressed= false;
    update();
    emit clicked();
}

void SwitchCameraBtn::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}
