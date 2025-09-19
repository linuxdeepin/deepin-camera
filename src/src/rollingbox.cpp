// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "rollingbox.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QGraphicsDropShadowEffect>
#include <QDebug>

#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#else
#include <DGuiApplicationHelper>
#endif

#define BTN_WIDTH 50
#define BTN_HEIGHT 26
#define BTN_SPACING 10


RollingBox::RollingBox(QWidget *parent) :
    QWidget(parent),
    m_rangeMin(0),
    m_currentIndex(0),
    m_isDragging(false),
    m_bFocus(false),
    m_deviation(0),
    m_textSize(15)  //暂定15,后续修改
{
    // qDebug() << "Initializing RollingBox";
    setContentsMargins(0, 0, 0, 0);

    m_pressResetTimer = new QTimer(this);
    connect(m_pressResetTimer, &QTimer::timeout, this, [=](){
        m_isDragging = true;
    });

    m_homingAnimation = new QPropertyAnimation(this, "deviation", this);
    m_homingAnimation->setDuration(300);
    m_homingAnimation->setEasingCurve(QEasingCurve::OutQuad);

    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0, 0);
    shadowEffect->setColor(QColor(33, 33, 33, 125));
    shadowEffect->setBlurRadius(8);
    this->setGraphicsEffect(shadowEffect);
    // qDebug() << "Function completed: RollingBox constructor";
}

void RollingBox::setRange(int min, int max)
{
    // qDebug() << "Setting range:" << min << "to" << max;
    m_rangeMin = min;
    m_rangeMax = max;

    if(m_currentIndex < min) {
        // qDebug() << "Adjusting current index from" << m_currentIndex << "to minimum" << min;
        m_currentIndex = min;
    }
    if(m_currentIndex > max) {
        // qDebug() << "Adjusting current index from" << m_currentIndex << "to maximum" << max;
        m_currentIndex = max;
    }

    update();
    // qDebug() << "Function completed: setRange";
}

void RollingBox::setContentList(const QList<QString> &content)
{
    // qDebug() << "Setting content list with" << content.size() << "items";
    m_content = content;
    setRange(0, m_content.size() - 1);
    // qDebug() << "Function completed: setContentList";
}

int RollingBox::getCurrentValue()
{
    // qDebug() << "Getting current value:" << m_currentIndex;
    return m_currentIndex;
}

void RollingBox::mousePressEvent(QMouseEvent *e)
{
    // qDebug() << "Function started: RollingBox::mousePressEvent";
    if (m_pressResetTimer->isActive()) {
        // qDebug() << "RollingBox::mousePressEvent: Enter if branch (timer active, stopping drag)";
        m_isDragging = false;
        m_pressResetTimer->stop();
    }
    m_homingAnimation->stop();
    m_mouseSrcPos = e->pos().y();
    m_pressResetTimer->start(500);
    // qDebug() << "Function completed: RollingBox::mousePressEvent";
}

void RollingBox::mouseMoveEvent(QMouseEvent *e)
{
    // qDebug() << "Function started: RollingBox::mouseMoveEvent";
    if (m_isDragging) {
        // qDebug() << "RollingBox::mouseMoveEvent: Enter if branch (is dragging)";
        //数值到边界时，阻止继续往对应方向移动
        if((m_currentIndex == m_rangeMin && e->pos().y() >= m_mouseSrcPos)||
          (m_currentIndex == m_rangeMax && e->pos().y() <= m_mouseSrcPos)) {
            // qDebug() << "RollingBox::mouseMoveEvent: Enter if branch (at boundary, returning early)";
            return;
        }

        m_deviation = e->pos().y() - m_mouseSrcPos;

        //若移动速度过快时进行限制
        if(m_deviation > height() / 4) {
            // qDebug() << "RollingBox::mouseMoveEvent: Enter if branch (deviation too high, limiting)";
            m_deviation = height() / 4;
        } else if(m_deviation < -height() / 4) {
            // qDebug() << "RollingBox::mouseMoveEvent: Enter else if branch (deviation too low, limiting)";
            m_deviation = -height() / 4;
        }

        update();
    }
    // qDebug() << "Function completed: RollingBox::mouseMoveEvent";
}

void RollingBox::mouseReleaseEvent(QMouseEvent *e)
{
    // qDebug() << "Function started: RollingBox::mouseReleaseEvent";
    if(m_isDragging) {
        // qDebug() << "RollingBox::mouseReleaseEvent: Enter if branch (is dragging)";
        m_isDragging = false;
        homing();
    } else {
        // qDebug() << "RollingBox::mouseReleaseEvent: Enter else branch (not dragging)";
        m_deviation = this->height() / 2 - e->pos().y();

        if (qAbs(m_deviation) <= this->height() / 6 ||
                qAbs(m_deviation) >= this->height() / 2 - 10) {//点击当前选项、上下边界
            // qDebug() << "Click too close to current selection or boundary, ignoring";
            m_deviation = 0;
            return;
        }

        if (m_deviation < 0 && m_currentIndex >= m_rangeMax) {
            if (qAbs(m_deviation) >= this->height() / 6) {
                // qDebug() << "Cannot move down at maximum index";
                m_deviation = 0;
                return;
            }
        }

        if (m_deviation > 0 && m_currentIndex <= m_rangeMin) {
            if (qAbs(m_deviation) >= this->height() / 6) {
                // qDebug() << "Cannot move up at minimum index";
                m_deviation = 0;
                return;
            }
        }

        homing();
    }

    QWidget::mouseReleaseEvent(e);
    // qDebug() << "Function completed: RollingBox::mouseReleaseEvent";
}

void RollingBox::wheelEvent(QWheelEvent *e)
{
    // qDebug() << "Function started: RollingBox::wheelEvent";
#if QT_VERSION_MAJOR > 5
    if (e->angleDelta().y() > 0 && m_currentIndex <= m_rangeMin) {
        // qDebug() << "RollingBox::wheelEvent: Enter if branch (Qt6, at min index, returning early)";
        return;
    }
    if (e->angleDelta().y() < 0 && m_currentIndex >= m_rangeMax) {
        // qDebug() << "RollingBox::wheelEvent: Enter if branch (Qt6, at max index, returning early)";
        return;
    }

    if(e->angleDelta().y() / 90 > 0) {
        // qDebug() << "RollingBox::wheelEvent: Enter if branch (Qt6, wheel up)";
        m_deviation = (this->height()) / 4;
    } else {
        // qDebug() << "RollingBox::wheelEvent: Enter else branch (Qt6, wheel down)";
        m_deviation = -(this->height()) / 4;
    }
#else
    if (e->delta() > 0 && m_currentIndex <= m_rangeMin) {
        // qDebug() << "RollingBox::wheelEvent: Enter if branch (Qt5, at min index, returning early)";
        return;
    }
    if (e->delta() < 0 && m_currentIndex >= m_rangeMax) {
        // qDebug() << "RollingBox::wheelEvent: Enter if branch (Qt5, at max index, returning early)";
        return;
    }

    if(e->delta() / 90 > 0) {
        // qDebug() << "RollingBox::wheelEvent: Enter if branch (Qt5, wheel up)";
        m_deviation = (this->height()) / 4;
    } else {
        // qDebug() << "RollingBox::wheelEvent: Enter else branch (Qt5, wheel down)";
        m_deviation = -(this->height()) / 4;
    }
#endif

    homing();
    update();
    // qDebug() << "Function completed: RollingBox::wheelEvent";
}

void RollingBox::focusInEvent(QFocusEvent *event)
{
    // qDebug() << "RollingBox::focusInEvent";
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}

void RollingBox::focusOutEvent(QFocusEvent *event)
{
    // qDebug() << "RollingBox::focusOutEvent";
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void RollingBox::keyReleaseEvent(QKeyEvent *event)
{
    // qDebug() << "RollingBox::keyReleaseEvent";
    switch (event->key()) {
    case Qt::Key_Down:
        if (m_currentIndex < m_rangeMax) {
            m_currentIndex++;
            homing();
        } else {
            return;
        }
        break;
    case Qt::Key_Up:
        if (m_currentIndex > m_rangeMin) {
            m_currentIndex--;
            homing();
        } else {
            return;
        }
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void RollingBox::paintEvent(QPaintEvent *e)
{
    // qDebug() << "Function started: RollingBox::paintEvent";
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    //中间文字背景
    QRect centerRect(1, (height() - BTN_HEIGHT) / 2, width() - 1, BTN_HEIGHT);
    QRect tmpLeftRect(centerRect.topLeft(), QSize(BTN_HEIGHT, BTN_HEIGHT));
    QRect tmpRightRect(centerRect.topRight() - QPoint(BTN_HEIGHT, 0), QSize(BTN_HEIGHT, BTN_HEIGHT));
    QPainterPath centerPath;

//    painter.setPen(QColor(255, 0, 0));
//    painter.drawRect(this->rect());

    centerPath.moveTo(tmpLeftRect.topRight());
    centerPath.arcTo(tmpLeftRect, 90, 180);
    centerPath.lineTo(centerRect.width() - BTN_HEIGHT / 2, centerRect.y() + BTN_HEIGHT);
    centerPath.arcTo(tmpRightRect, 270, 180);
    centerPath.lineTo(tmpLeftRect.topRight());

    if (m_bFocus) {
        // qDebug() << "RollingBox::focusInEvent";
        painter.setPen(QPen(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color(), 2));
        painter.setBrush(QBrush(QColor(0, 0, 0, 0.4 * 255)));
        painter.drawPath(centerPath);

        QRect focusWhite = centerRect;
        focusWhite.setTopLeft(centerRect.topLeft() + QPoint(2, 2));
        focusWhite.setSize(QSize(width() - 1 - 4, BTN_HEIGHT - 4));

        QRect leftRect(focusWhite.topLeft(), QSize(BTN_HEIGHT - 4, BTN_HEIGHT - 4));
        QRect rightRect(focusWhite.topRight() - QPoint(BTN_HEIGHT - 4, 0), QSize(BTN_HEIGHT - 4, BTN_HEIGHT - 4));

        QPainterPath path;
        path.moveTo(leftRect.topRight());
        path.arcTo(leftRect, 90, 180);
        path.lineTo(focusWhite.width() - (BTN_HEIGHT - 4) / 2, focusWhite.y() + BTN_HEIGHT - 4);
        path.arcTo(rightRect, 270, 180);
        path.lineTo(leftRect.topRight());

        painter.setPen(Qt::white);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);
    } else {
        // qDebug() << "RollingBox::focusOutEvent";
        painter.setPen(Qt::NoPen);
        painter.fillPath(centerPath, QBrush(QColor(0, 0, 0, 0.4 * 255)));
    }

    if (m_currentIndex >= 0 && m_currentIndex <= m_rangeMax)
        paintContent(painter, m_currentIndex, m_deviation);

    //两侧内容
    //选中的内容不是最小，不是最大，那么就有两侧内容，然后画出两侧内容
    if (m_currentIndex != m_rangeMin && m_currentIndex >= 0 && m_currentIndex <= m_rangeMax)
        paintContent(painter, m_currentIndex - 1, m_deviation - height() / 4);
    if (m_currentIndex < m_rangeMax && m_currentIndex >= 0)
        paintContent(painter, m_currentIndex + 1, m_deviation + height() / 4);

    if (m_deviation >= 0 && m_currentIndex - 2 >= m_rangeMin)
        paintContent(painter, m_currentIndex - 2, m_deviation - height() / 2);
    if (m_deviation <= 0 && m_currentIndex + 2 <= m_rangeMax)
        paintContent(painter, m_currentIndex + 2, m_deviation + height() / 2);
    // qDebug() << "Function completed: RollingBox::paintEvent";
}

void RollingBox::paintContent(QPainter &painter, int num, int deviation)
{
    // qDebug() << "RollingBox::paintContent";
//    int transparency = 255 - 510 * qAbs(deviation) / height();
//    int Height = height() / 2 - qAbs(deviation) / 6;
    double y = height() / 2 + deviation * 1.3 - BTN_HEIGHT / 2;

    QFont font("SourceHanSansSC");
    font.setPixelSize(m_textSize);
    font.setWeight(QFont::Medium);
    painter.setFont(font);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(QRectF(0, y, width(), BTN_HEIGHT),
                     Qt::AlignCenter, m_content.at(num));
}

void RollingBox::homing()
{
    // qDebug() << "Starting homing animation from deviation:" << m_deviation;
    if(m_deviation > height() / 6) {
        // qDebug() << "m_deviation > height() / 6";
        m_homingAnimation->setStartValue(height() / 6 - m_deviation);
        m_homingAnimation->setEndValue(0);
        m_currentIndex--;
    } else if (m_deviation > -height() / 6) {
        // qDebug() << "m_deviation > -height() / 6";
        m_homingAnimation->setStartValue(m_deviation);
        m_homingAnimation->setEndValue(0);
    } else if(m_deviation < -height() / 6) {
        // qDebug() << "m_deviation < -height() / 6";
        m_homingAnimation->setStartValue(-height() / 6 - m_deviation);
        m_homingAnimation->setEndValue(0);
        m_currentIndex++;
    }

    if (m_currentIndex < m_rangeMin)
        m_currentIndex = m_rangeMin;
    if (m_currentIndex > m_rangeMax)
        m_currentIndex = m_rangeMax;

    emit currentValueChanged(m_currentIndex);
    m_homingAnimation->start();
    // qDebug() << "Function completed: homing";
}

int RollingBox::readDeviation()
{
    // qWarning() << "Invalid deviation value";
    return m_deviation;
}

void RollingBox::setDeviation(int n)
{
    // qWarning() << "Invalid deviation value";
    m_deviation = n;
    update();
}

