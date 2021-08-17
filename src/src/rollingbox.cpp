/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     fengli <fengli@uniontech.com>
*
* Maintainer: fengli <fengli@uniontech.com>
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

#include "rollingbox.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#define BTN_WIDTH 50
#define BTN_HEIGHT 26
#define BTN_SPACING 10


RollingBox::RollingBox(QWidget *parent) :
    QWidget(parent),
    m_currentIndex(0),
    m_isDragging(false),
    m_bFocus(false),
    m_deviation(0),
    m_textSize(15)  //暂定15,后续修改
{
    m_rangeMin = 0;

    setContentsMargins(0, 0, 0, 0);

    m_pressResetTimer = new QTimer(this);
    connect(m_pressResetTimer, &QTimer::timeout, this, [=](){
        m_isDragging = true;
    });

    m_homingAnimation = new QPropertyAnimation(this, "deviation", this);
    m_homingAnimation->setDuration(300);
    m_homingAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void RollingBox::setRange(int min,int max)
{
    m_rangeMin = min;
    m_rangeMax = max;

    if(m_currentIndex < min)
        m_currentIndex = min;
    if(m_currentIndex > max)
        m_currentIndex = max;

    update();
}

void RollingBox::setContentList(const QList<QString> &content)
{
    m_content = content;
    setRange(0, m_content.size() - 1);
}

int RollingBox::getCurrentValue()
{
    return m_currentIndex;
}

void RollingBox::mousePressEvent(QMouseEvent *e)
{
    if (m_pressResetTimer->isActive()) {
        m_isDragging = false;
        m_pressResetTimer->stop();
    }
    m_homingAnimation->stop();
    m_mouseSrcPos = e->pos().y();
    m_pressResetTimer->start(500);
}

void RollingBox::mouseMoveEvent(QMouseEvent *e)
{
    if (m_isDragging) {
        //数值到边界时，阻止继续往对应方向移动
        if((m_currentIndex == m_rangeMin && e->pos().y() >= m_mouseSrcPos)||
          (m_currentIndex == m_rangeMax && e->pos().y() <= m_mouseSrcPos))
            return;

        m_deviation = e->pos().y() - m_mouseSrcPos;

        //若移动速度过快时进行限制
        if(m_deviation > height() / 4)
            m_deviation = height() / 4;
        else if(m_deviation < -height() / 4)
            m_deviation = -height() / 4;

        update();
    }
}

void RollingBox::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_isDragging) {
        m_isDragging = false;
        homing();
    } else {
        m_deviation = this->height() / 2 - e->pos().y();
        homing();
    }

    QWidget::mouseReleaseEvent(e);
}

void RollingBox::wheelEvent(QWheelEvent *e)
{
    if(e->delta() / 90 > 0) {
        m_deviation = (this->height()) / 4;
    } else {
        m_deviation = -(this->height()) / 4;
    }

    homing();
    update();
}

void RollingBox::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}

void RollingBox::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void RollingBox::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_deviation >= height() / 4 && m_currentIndex > m_rangeMin) { //偏移量大于1/4的时候，数字减一
        m_mouseSrcPos += height() / 4;  //鼠标起始位置重新设置，即加上1/4的高度
        m_deviation -= height() / 4;    //偏移量重新设置，即减去1/4的高度
        m_currentIndex -= 1;
    }
    if (m_deviation <= -height() / 4 && m_currentIndex < m_rangeMax) {
        m_mouseSrcPos -= height() / 4;
        m_deviation += height() / 4;
        m_currentIndex += 1;
    }

    //中间文字背景
    QRect centerRect(0, (height() - BTN_HEIGHT) / 2, width(), BTN_HEIGHT);
    QRect tmpLeftRect(centerRect.topLeft(), QSize(BTN_HEIGHT, BTN_HEIGHT));
    QRect tmpRightRect(centerRect.topRight() - QPoint(BTN_HEIGHT, 0), QSize(BTN_HEIGHT, BTN_HEIGHT));
    QPainterPath centerPath;

    centerPath.moveTo(tmpLeftRect.topRight());
    centerPath.arcTo(tmpLeftRect, 90, 180);
    centerPath.lineTo(centerRect.width() - BTN_HEIGHT / 2, centerRect.y() + BTN_HEIGHT);
    centerPath.arcTo(tmpRightRect, 270, 180);
    centerPath.lineTo(tmpLeftRect.topRight());

    if (m_bFocus) {
        painter.setPen(QPen(QColor("#0081FF"), 2));
        painter.setBrush(QBrush(QColor(0, 0, 0, 0.4 * 255)));
        painter.drawPath(centerPath);
    } else {
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
}

void RollingBox::paintContent(QPainter &painter, int num, int deviation)
{
//    int size = (Height - qAbs(deviation)) / m_numSize;
//    int transparency = 255 - 510 * qAbs(deviation) / Height;
    int Height = height() / 2 - 3 * qAbs(deviation) / 5 +10;
    int y = height() / 2 + deviation * 1.3 - Height / 2;

    QFont font("SourceHanSansSC");
    font.setPixelSize(m_textSize);
    font.setWeight(QFont::Medium);
    painter.setFont(font);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(QRectF(0, y, width(), Height),
                     Qt::AlignCenter, m_content.at(num));
}

void RollingBox::homing()
{
    if(m_deviation > height() / 8) {
        m_homingAnimation->setStartValue((height() - 1) / 8 - m_deviation);
        m_homingAnimation->setEndValue(0);
        m_currentIndex--;
    } else if (m_deviation > -height() / 8) {
        m_homingAnimation->setStartValue(m_deviation);
        m_homingAnimation->setEndValue(0);
    } else if(m_deviation < -height() / 8) {
        m_homingAnimation->setStartValue(-(height() - 1) / 8 - m_deviation);
        m_homingAnimation->setEndValue(0);
        m_currentIndex++;
    }

    if (m_currentIndex < m_rangeMin)
        m_currentIndex = m_rangeMin;
    if (m_currentIndex > m_rangeMax)
        m_currentIndex = m_rangeMax;

    emit currentValueChanged(m_currentIndex);
    m_homingAnimation->start();
}

int RollingBox::readDeviation()
{
    return m_deviation;
}

void RollingBox::setDeviation(int n)
{
    m_deviation = n;
    update();
}

