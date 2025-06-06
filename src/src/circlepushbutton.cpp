// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "circlepushbutton.h"
#include "ac-deepin-camera-define.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QSvgRenderer>
#include <QApplication>

circlePushButton::circlePushButton(QWidget *parent): QPushButton(parent), m_radius(20)
{
    qDebug() << "Initializing CirclePushButton with radius:" << m_radius;
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
    qDebug() << "Setting button pixmaps - Normal:" << normalPath << "Hover:" << hoverPath << "Press:" << pressPath;
    m_normalSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(normalPath));
    m_hoverSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(hoverPath));
    m_pressSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(pressPath));
}

void circlePushButton::setSelected(bool selected)
{
    qDebug() << "Setting button selected state to:" << selected;
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
#if QT_VERSION_MAJOR > 5
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#else
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#endif

    int distance = 2;
    int tmpWidth = width();
    int tmpHeight = height();

    QRectF rect = this->rect();

    //绘制背景色
    QPainterPath path;
    path.addEllipse(rect);
    painter.fillPath(path, QBrush(QColor(m_color)));

    QColor highlight = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color();
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
        if (this->objectName() == FILTERS_FOLD_BTN
                || this->objectName() == FILTERS_CLOSE_BTN)
            highlight = Qt::white;
        painter.setPen(QPen(highlight, distance));
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
    
    if (m_isNewNotification) {
        rect.setTopLeft(QPoint(rect.width() - 9, 1));
        rect.setSize(QSize(8, 8));
        painter.setPen(QPen(Qt::white, 1));
        painter.setBrush(highlight);
        painter.drawEllipse(rect);
    }

    painter.save();
    painter.translate(width() / 2, height() / 2);
    painter.rotate(m_rotate);
    painter.translate(-width() / 2, -height() / 2);
    if (m_disableSelect) {//禁用悬浮只绘制选中与正常
//        painter.setViewport(QRect(-width() / 2, -height() / 2, width(), height()));
        if (m_mousePress) {
            painter.setOpacity(m_iconOpacity);
            m_pressSvg->render(&painter);
        } else {
            painter.setOpacity(m_iconOpacity);
            m_normalSvg->render(&painter);
        }
        painter.restore();
        return;
    }

    //设置绘制的图案
    if (!m_mousePress && m_isSelected) {
        painter.setOpacity(m_iconOpacity);
        m_hoverSvg->render(&painter);
    } else if (!m_mousePress && !m_isSelected) {
        painter.setOpacity(m_iconOpacity);
        m_normalSvg->render(&painter);
    } else {
        painter.setOpacity(m_iconOpacity);
        m_pressSvg->render(&painter);
    }
    painter.restore();
}

#if QT_VERSION_MAJOR > 5
void circlePushButton::enterEvent(QEnterEvent *event)
#else
void circlePushButton::enterEvent(QEvent *event)
#endif
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

void circlePushButton::hideEvent(QHideEvent *event)
{
    QWidget* fw = QApplication::focusWidget();
    if (fw == this)
        this->clearFocus();

    QPushButton::hideEvent(event);
}

bool circlePushButton::focusNextPrevChild(bool next)
{
    Q_UNUSED(next);
    return QPushButton::focusNextPrevChild(next);
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

void circlePushButton::mouseMoveEvent(QMouseEvent *event)
{
    //解决bug 在按钮中可拖动相机界面，bug 100647
    Q_UNUSED(event);
    return;
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
    if (radius <= 0) {
        qWarning() << "Invalid button radius:" << radius;
        return;
    }

    qDebug() << "Setting button radius to:" << radius;
    m_radius = radius;
    resize(2 * m_radius, 2 * m_radius);
}

void circlePushButton::copyPixmap(const circlePushButton &other)
{
    qDebug() << "Copying pixmaps from another button";
    m_normalSvg = other.m_normalSvg;
    m_hoverSvg = other.m_hoverSvg;
    m_pressSvg = other.m_pressSvg;
}

void circlePushButton::setDisableSelect(bool disable)
{
    qDebug() << "Setting disable select to:" << disable;
    m_disableSelect = disable;
}

void circlePushButton::setOpacity(int opacity)
{
    qDebug() << "Setting button opacity to:" << opacity;
    m_opacity = opacity;
    m_color.setAlpha(opacity);
    update();
}

void circlePushButton::setIconOpacity(qreal opacity)
{
    qDebug() << "Setting icon opacity to:" << opacity;
    m_iconOpacity = opacity;
    update();
}

void circlePushButton::setRotate(int rotate)
{
    qDebug() << "Setting button rotation to:" << rotate;
    m_rotate = rotate;
    update();
}

void circlePushButton::setNewNotification(const bool set_new)
{
    qDebug() << "Setting new notification to:" << set_new;
    m_isNewNotification = set_new;
}

circlePushButton::~circlePushButton()
{
    qDebug() << "Destroying CirclePushButton";
}
