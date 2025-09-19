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
    // qDebug() << "Initializing CirclePushButton with radius:" << m_radius;
    m_color.setRgb(0, 0, 0, m_opacity);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;
    m_disableSelect = false;

    m_normalSvg = nullptr;
    m_hoverSvg = nullptr;
    m_pressSvg = nullptr;
    setButtonRadius(m_radius);
    // qDebug() << "Function completed: circlePushButton constructor";
}

void circlePushButton::setbackground(QColor color)
{
    // qDebug() << "Setting background color to:" << color;
    m_color = color;
}

void circlePushButton::setPixmap(QString normalPath, QString hoverPath, QString pressPath)
{
    // qDebug() << "Setting button pixmaps - Normal:" << normalPath << "Hover:" << hoverPath << "Press:" << pressPath;
    m_normalSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(normalPath));
    m_hoverSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(hoverPath));
    m_pressSvg = QSharedPointer<QSvgRenderer>(new QSvgRenderer(pressPath));
    // qDebug() << "Function completed: setPixmap";
}

void circlePushButton::setSelected(bool selected)
{
    // qDebug() << "Setting button selected state to:" << selected;
    m_isSelected = selected;
    update();
    // qDebug() << "Function completed: setSelected";
}

bool circlePushButton::getButtonState()
{
    // qDebug() << "Function started: getButtonState";
    return m_isSelected;
}

void circlePushButton::paintEvent(QPaintEvent *event)
{
    // qDebug() << "Function started: paintEvent";
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
        // qDebug() << "Drawing gray rectangle";
        QRect grayRect = this->rect();
        grayRect.setTopLeft(QPoint(2, 2));
        grayRect.setSize(QSize(36, 36));
        path.addEllipse(grayRect);
        painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 0.4)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(grayRect);
    } else if (m_hover) {
        // qDebug() << "Drawing focus blue rectangle";
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
        // qDebug() << "Drawing selected rectangle";
        QRect selectedRect = this->rect();
        selectedRect.setTopLeft(QPoint(2, 2));
        selectedRect.setSize(QSize(36, 36));
        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(QColor(255, 255, 255, 0.1 * 255)));
        painter.drawEllipse(selectedRect);
    }
    
    if (m_isNewNotification) {
        // qDebug() << "Drawing new notification rectangle";
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
        // qDebug() << "Drawing disable select rectangle";
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
        // qDebug() << "Drawing hover selected rectangle";
        painter.setOpacity(m_iconOpacity);
        m_hoverSvg->render(&painter);
    } else if (!m_mousePress && !m_isSelected) {
        // qDebug() << "Drawing normal rectangle";
        painter.setOpacity(m_iconOpacity);
        m_normalSvg->render(&painter);
    } else {
        // qDebug() << "Drawing press rectangle";
        painter.setOpacity(m_iconOpacity);
        m_pressSvg->render(&painter);
    }
    painter.restore();
    // qDebug() << "Function completed: paintEvent";
}

#if QT_VERSION_MAJOR > 5
void circlePushButton::enterEvent(QEnterEvent *event)
#else
void circlePushButton::enterEvent(QEvent *event)
#endif
{
    // qDebug() << "Function started: enterEvent";
    Q_UNUSED(event);
    // qDebug() << "Setting hover state to true";
    m_hover = true;
    update();
    // qDebug() << "Function completed: enterEvent";
}

void circlePushButton::focusInEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: focusInEvent";
    Q_UNUSED(event);
    // qDebug() << "Setting hover state to true (focus in)";
    m_hover = true;
    update();
    // qDebug() << "Function completed: focusInEvent";
}

void circlePushButton::focusOutEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: focusOutEvent";
    Q_UNUSED(event);
    // qDebug() << "Setting hover state to false (focus out)";
    m_hover = false;
    update();
    // qDebug() << "Function completed: focusOutEvent";
}

void circlePushButton::hideEvent(QHideEvent *event)
{
    // qDebug() << "Function started: hideEvent";
    QWidget* fw = QApplication::focusWidget();
    // qDebug() << "Current focus widget:" << (fw ? "exists" : "null");
    if (fw == this) {
        // qDebug() << "Condition check: Widget has focus, clearing focus";
        this->clearFocus();
    }

    QPushButton::hideEvent(event);
    // qDebug() << "Function completed: hideEvent";
}

bool circlePushButton::focusNextPrevChild(bool next)
{
    // qDebug() << "Function started: focusNextPrevChild";
    Q_UNUSED(next);
    // qDebug() << "Calling parent focusNextPrevChild";
    bool result = QPushButton::focusNextPrevChild(next);
    // qDebug() << "Function completed: focusNextPrevChild";
    return result;
}

void circlePushButton::leaveEvent(QEvent *event)
{
    // qDebug() << "Function started: leaveEvent";
    Q_UNUSED(event);
    // qDebug() << "Setting hover state to false (mouse left)";
    m_hover = false;
    m_mousePress = false;
    update();
    // qDebug() << "Function completed: leaveEvent";
}

void circlePushButton::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: mousePressEvent";
    Q_UNUSED(event);
    // qDebug() << "Setting mouse press state to true";
    m_mousePress = true;
    // qDebug() << "Toggling selection state";
    setSelected(!m_isSelected);
    update();
    QPushButton::mousePressEvent(event);
    // qDebug() << "Function completed: mousePressEvent";
}

void circlePushButton::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: mouseMoveEvent";
    //解决bug 在按钮中可拖动相机界面，bug 100647
    Q_UNUSED(event);
    // qDebug() << "Ignoring mouse move event to prevent dragging";
    // qDebug() << "Function completed: mouseMoveEvent";
    return;
}

void circlePushButton::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: mouseReleaseEvent";
    Q_UNUSED(event);
    // qDebug() << "Setting mouse press state to false";
    m_mousePress = false;
    update();
    QPushButton::mouseReleaseEvent(event);
    // qDebug() << "Function completed: mouseReleaseEvent";
}

void circlePushButton::setButtonRadius(int radius)
{
    // qDebug() << "Function started: setButtonRadius";
    if (radius <= 0) {
        qWarning() << "Invalid button radius:" << radius;
        return;
    }

    // qDebug() << "Setting button radius to:" << radius;
    m_radius = radius;
    resize(2 * m_radius, 2 * m_radius);
}

void circlePushButton::copyPixmap(const circlePushButton &other)
{
    // qDebug() << "Copying pixmaps from another button";
    m_normalSvg = other.m_normalSvg;
    m_hoverSvg = other.m_hoverSvg;
    m_pressSvg = other.m_pressSvg;
}

void circlePushButton::setDisableSelect(bool disable)
{
    // qDebug() << "Setting disable select to:" << disable;
    m_disableSelect = disable;
}

void circlePushButton::setOpacity(int opacity)
{
    // qDebug() << "Setting button opacity to:" << opacity;
    m_opacity = opacity;
    m_color.setAlpha(opacity);
    update();
}

void circlePushButton::setIconOpacity(qreal opacity)
{
    // qDebug() << "Setting icon opacity to:" << opacity;
    m_iconOpacity = opacity;
    update();
}

void circlePushButton::setRotate(int rotate)
{
    // qDebug() << "Setting button rotation to:" << rotate;
    m_rotate = rotate;
    update();
}

void circlePushButton::setNewNotification(const bool set_new)
{
    // qDebug() << "Setting new notification to:" << set_new;
    m_isNewNotification = set_new;
}

circlePushButton::~circlePushButton()
{
    // qDebug() << "Destroying CirclePushButton";
}
