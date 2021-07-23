/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     tanlang <tanlang@uniontech.com>
*             
* Maintainer: tanlang <tanlang@uniontech.com>
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

#include "circlepushbutton.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QSvgRenderer>

circlePushButton::circlePushButton(QWidget *parent): QPushButton(parent), m_radius(20)
{
    m_color.setRgb(0, 0, 0, 102);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;
    m_disableSelect = false;

    m_normalSvg = nullptr;
    m_hoverSvg = nullptr;
    m_pressSvg = nullptr;
    setButtonRadius(m_radius);

}

circlePushButton::~circlePushButton()
{
    releaseSvg();
}

void circlePushButton::setbackground(QColor color)
{
    m_color = color;
}

void circlePushButton::setPixmap(QString normalPath, QString hoverPath, QString pressPath)
{
    releaseSvg();
    m_normalSvg = new QSvgRenderer(normalPath);
    m_hoverSvg = new QSvgRenderer(hoverPath);
    m_pressSvg = new QSvgRenderer(pressPath);
}

void circlePushButton::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

bool circlePushButton::getButtonState()
{
    return m_isSelected;
}

void circlePushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    int distance = 2;
    int tmpWidth = width();
    int tmpHeight = height();

    //绘制背景色
    QPainterPath path;
    path.addEllipse(rect());
    painter.fillPath(path, QBrush(QColor(m_color)));

    //绘制点击效果
    if (m_mousePress) {
        QRect grayRect = this->rect();
        grayRect.setTopLeft(QPoint(3, 3));
        grayRect.setSize(QSize(36, 36));
        path.addEllipse(grayRect);
        painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 0.4)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(grayRect);
    } else if (m_hover) {
        QRect focusBlue = this->rect();
        focusBlue.setTopLeft(QPoint(0, 0));
        focusBlue.setSize(QSize(tmpWidth, tmpHeight));
        painter.setPen(QPen(QColor(Qt::blue), distance));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(focusBlue);

        QRect focusWhite = this->rect();
        focusWhite.setTopLeft(QPoint(distance, distance));
        focusWhite.setSize(QSize(tmpWidth - 2 * distance, tmpHeight - 2 * distance));
        painter.setPen(QPen(QColor(Qt::white), distance));
        painter.drawEllipse(focusWhite);
    } else if (m_isSelected && !m_disableSelect) {
        QRect selectedRect = this->rect();
        selectedRect.setTopLeft(QPoint(2, 2));
        selectedRect.setSize(QSize(36, 36));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 255, 255, 0.4 * 255)));
        painter.drawEllipse(selectedRect);
    }
    
    if (m_disableSelect) {//禁用悬浮只绘制选中与正常
        if (m_mousePress)
            m_pressSvg->render(&painter);
        else
            m_normalSvg->render(&painter);

        return;
    }

    //设置绘制的图案
    if (!m_mousePress && m_isSelected) {
        m_hoverSvg->render(&painter);
    } else if (!m_mousePress && !m_isSelected) {
        m_normalSvg->render(&painter);
    } else {
        m_pressSvg->render(&painter);
    }
}

void circlePushButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void circlePushButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    m_mousePress = false;
    update();
}

void circlePushButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = true;
    setSelected(!m_isSelected);
    update();
    QPushButton::mousePressEvent(event);
}

void circlePushButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = false;
    update();
    QPushButton::mouseReleaseEvent(event);
}

void circlePushButton::setButtonRadius(int radius)
{
    if (radius <= 0)
        return;

    m_radius = radius;
    resize(2 * m_radius, 2 * m_radius);
}

void circlePushButton::copyPixmap(const circlePushButton &other)
{
    releaseSvg();

    m_normalSvg = other.m_normalSvg;
    m_hoverSvg = other.m_hoverSvg;
    m_pressSvg = other.m_pressSvg;
}

void circlePushButton::setDisableSelect(bool disable)
{
    m_disableSelect = disable;
}


void circlePushButton::releaseSvg()
{
    if (nullptr != m_normalSvg) {
        m_normalSvg->deleteLater();
        m_normalSvg = nullptr;
    }

    if (nullptr != m_hoverSvg) {
        m_hoverSvg->deleteLater();
        m_hoverSvg = nullptr;
    }

    if (nullptr != m_pressSvg) {
        m_pressSvg->deleteLater();
        m_pressSvg = nullptr;
    }
}
