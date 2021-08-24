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
    m_color.setRgb(0, 0, 0, m_opacity);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;
    m_disableSelect = false;

    m_normalSvg = nullptr;
    m_hoverSvg = nullptr;
    m_pressSvg = nullptr;
    setButtonRadius(m_radius);
}

void circlePushButton::setbackground(QColor color)
{
    m_color = color;
}

void circlePushButton::setPixmap(QString normalPath, QString hoverPath, QString pressPath)
{
    m_normalSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(normalPath));
    m_hoverSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(hoverPath));
    m_pressSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(pressPath));
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
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

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
        grayRect.setTopLeft(QPoint(2, 2));
        grayRect.setSize(QSize(36, 36));
        path.addEllipse(grayRect);
        painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 0.4)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(grayRect);
    } else if (m_hover) {
        QRect focusBlue = this->rect();
        focusBlue.setTopLeft(QPoint(1, 1));
        focusBlue.setSize(QSize(tmpWidth - 2, tmpHeight - 2));
        painter.setPen(QPen(QColor("#0081FF"), distance));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(focusBlue);

        QRect focusWhite = this->rect();
        focusWhite.setTopLeft(QPoint(3, 3));
        focusWhite.setSize(QSize(tmpWidth - 6, tmpHeight - 6));
        painter.setPen(QPen(QColor(Qt::white)));
        painter.drawEllipse(focusWhite);
    } else if (m_isSelected && !m_disableSelect) {
        QRect selectedRect = this->rect();
        selectedRect.setTopLeft(QPoint(2, 2));
        selectedRect.setSize(QSize(36, 36));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 255, 255, 0.1 * 255)));
        painter.drawEllipse(selectedRect);
    }
    
    painter.save();
    painter.translate(width() / 2, height() / 2);
    painter.rotate(m_rotate);
    painter.translate(-width() / 2, -height() / 2);
    if (m_disableSelect) {//禁用悬浮只绘制选中与正常
//        painter.setViewport(QRect(-width() / 2, -height() / 2, width(), height()));
        if (m_mousePress) {
            m_pressSvg->render(&painter);
        } else {
            m_normalSvg->render(&painter);
        }
        painter.restore();
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
    painter.restore();
}

void circlePushButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void circlePushButton::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void circlePushButton::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
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
    m_normalSvg = other.m_normalSvg;
    m_hoverSvg = other.m_hoverSvg;
    m_pressSvg = other.m_pressSvg;
}

void circlePushButton::setDisableSelect(bool disable)
{
    m_disableSelect = disable;
}

void circlePushButton::setOpacity(int opacity)
{
    m_opacity = opacity;
    m_color.setAlpha(opacity);
    update();
}

void circlePushButton::setRotate(int rotate)
{
    m_rotate = rotate;
    update();
}

circlePushButton::~circlePushButton()
{

}
