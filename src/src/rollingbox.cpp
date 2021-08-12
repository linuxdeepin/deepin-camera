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
    m_currentValue(0),
    m_isDragging(false),
    m_deviation(0),
    m_textSize(15)  //暂定15,后续修改
{
    m_rangeMin = 0;

    m_pressResetTimer = new QTimer(this);
    connect(m_pressResetTimer, &QTimer::timeout, this, [=](){
        m_isDragging = true;
    });

    m_homingAnimation = new QPropertyAnimation(this,"deviation");
    m_homingAnimation->setDuration(300);
    m_homingAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void RollingBox::setRange(int min,int max)
{
    m_rangeMin = min;
    m_rangeMax = max;

    if(m_currentValue < min)
        m_currentValue = min;
    if(m_currentValue > max)
        m_currentValue = max;

    update();
}

void RollingBox::setContentList(const QList<QString> &content)
{
    m_content = content;
    setRange(0, m_content.size() - 1);
}

int RollingBox::getCurrentValue()
{
    return m_currentValue;
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
        if((m_currentValue == m_rangeMin && e->pos().y() >= m_mouseSrcPos)||
          (m_currentValue == m_rangeMax && e->pos().y() <= m_mouseSrcPos))
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

void RollingBox::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    int Width = width();
    int Height = height();

    painter.setRenderHint(QPainter::Antialiasing, true);

    if (m_currentValue < m_rangeMin)
        m_currentValue = m_rangeMin;
    if (m_currentValue > m_rangeMax)
        m_currentValue = m_rangeMax;

    if (m_deviation >= Height / 4 && m_currentValue > m_rangeMin) { //偏移量大于1/4的时候，数字减一
        m_mouseSrcPos += Height / 4;  //鼠标起始位置重新设置，即加上1/4的高度
        m_deviation -= Height / 4;    //偏移量重新设置，即减去1/4的高度
        m_currentValue -= 1;
    }
    if (m_deviation <= -Height / 4 && m_currentValue < m_rangeMax) {
        m_mouseSrcPos -= Height / 4;
        m_deviation += Height / 4;
        m_currentValue += 1;
    }

    //中间文字背景
    QRect centerRect(0, (Height - BTN_HEIGHT) / 2, Width, BTN_HEIGHT);
    QRect tmpLeftRect(centerRect.topLeft(), QSize(BTN_HEIGHT, BTN_HEIGHT));
    QRect tmpRightRect(centerRect.topRight() - QPoint(BTN_HEIGHT, 0), QSize(BTN_HEIGHT, BTN_HEIGHT));
    QPainterPath centerPath;

    centerPath.moveTo(tmpLeftRect.topRight());
    centerPath.arcTo(tmpLeftRect, 90, 180);
    centerPath.lineTo(centerRect.width() - BTN_HEIGHT / 2, centerRect.y() + BTN_HEIGHT);
    centerPath.arcTo(tmpRightRect, 270, 180);
    centerPath.lineTo(tmpLeftRect.topRight());

    painter.setPen(Qt::NoPen);
    painter.fillPath(centerPath, QBrush(QColor(0, 0, 0, 0.4 * 255)));


    if (m_currentValue >= 0 && m_currentValue <= m_rangeMax)
        paintContent(painter, m_currentValue, m_deviation);

    //两侧内容1
    //选中的内容不是最小，不是最大，那么就有两侧内容，然后画出两侧内容
    if (m_currentValue != m_rangeMin && m_currentValue >= 0 && m_currentValue <= m_rangeMax)
        paintContent(painter, m_currentValue - 1, m_deviation - Height / 4);
    if (m_currentValue < m_rangeMax && m_currentValue >= 0)
        paintContent(painter, m_currentValue + 1, m_deviation + Height / 4);

    //两侧内容2,超出则不显示
    if(m_deviation >= 0 && m_currentValue - 2 > m_rangeMin && m_currentValue+2 <= m_rangeMax)
        paintContent(painter, m_currentValue - 2, m_deviation - Height / 2);
    if(m_deviation <= 0 && m_currentValue+2 < m_rangeMax && m_currentValue+2 >= 0)
        paintContent(painter, m_currentValue + 2, m_deviation + Height / 2);
}

void RollingBox::paintContent(QPainter &painter,int num,int deviation)
{
    int Width = this->width();
    int Height = this->height();
//    int size = (Height - qAbs(deviation)) / m_numSize;        //qAbs 返回输入参数对应类型的绝对值
//    int transparency = 255 - 510 * qAbs(deviation) / Height;
    int height = Height / 2 - 3 * qAbs(deviation) / 5;
    int y = Height / 2 + deviation - height / 2;

    QFont font("SourceHanSansSC");
    font.setPixelSize(m_textSize);
    font.setWeight(QFont::Medium);
    painter.setFont(font);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(QRectF(0, y, Width, height),
                     Qt::AlignCenter, m_content.at(num));
}

void RollingBox::homing()
{
    if(m_deviation > height() / 8) {
        m_homingAnimation->setStartValue((height() - 1) / 8 - m_deviation);
        m_homingAnimation->setEndValue(0);
        m_currentValue--;
    } else if (m_deviation > -height() / 8) {
        m_homingAnimation->setStartValue(m_deviation);
        m_homingAnimation->setEndValue(0);
    } else if(m_deviation < -height() / 8) {
        m_homingAnimation->setStartValue(-(height() - 1) / 8 - m_deviation);
        m_homingAnimation->setEndValue(0);
        m_currentValue++;
    }
    emit currentValueChanged(m_currentValue);
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

