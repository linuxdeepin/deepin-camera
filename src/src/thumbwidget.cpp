/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
* Maintainer: hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
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

#include "thumbwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <DGuiApplicationHelper>

ThumbWidget::ThumbWidget(QWidget *parent, int outlinewidth): DLabel(parent), m_outLineWidth(outlinewidth)
{
    m_tabFocusStatus = false;
    setContentsMargins(5, 0, 5, 0);
}

void ThumbWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_tabFocusStatus = true;
    update();
}

void ThumbWidget::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_tabFocusStatus = false;
    update();
}

void ThumbWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (m_tabFocusStatus) {
        //获取painter
        QPainter painter(this);
        /**
         * @brief setRenderHints
         * 设置渲染器标志
         * @arg QPainter::Antialiasing
         * 边缘抗锯齿
         */
        painter.setRenderHints(QPainter::Antialiasing);
        //获取当前widget的矩形框
        QRect LabelWidgetRect = rect();
        //初始化圆角矩形框的绘制路径
        QPainterPath roundRectPath;
        //调节圆角的大小
        qreal cornerSize = 30;
        //圆角半径
        qreal arcR = cornerSize / 2.0;
        //将圆角矩形框的长度和宽度缩小2个线框宽度
        QPoint topleft = LabelWidgetRect.topLeft();
        QPoint bottomRight = LabelWidgetRect.bottomRight();

        topleft.setX(topleft.x() + m_outLineWidth);
        topleft.setY(topleft.y() + m_outLineWidth);
        bottomRight.setX(bottomRight.x() - m_outLineWidth);
        bottomRight.setY(bottomRight.y() - m_outLineWidth);
        LabelWidgetRect.setTopLeft(topleft);
        LabelWidgetRect.setBottomRight(bottomRight);

        //绘制圆角矩形框
        roundRectPath.moveTo(LabelWidgetRect.left() + arcR, LabelWidgetRect.top());
        roundRectPath.arcTo(LabelWidgetRect.left(), LabelWidgetRect.top(), cornerSize, cornerSize, 90.0f, 90.0f);
        roundRectPath.lineTo(LabelWidgetRect.left(), LabelWidgetRect.bottom() - arcR);
        roundRectPath.arcTo(LabelWidgetRect.left(), LabelWidgetRect.bottom() - cornerSize, cornerSize, cornerSize, 180.0f, 90.0f);
        roundRectPath.lineTo(LabelWidgetRect.right() - arcR, LabelWidgetRect.bottom());
        roundRectPath.arcTo(LabelWidgetRect.right() - cornerSize, LabelWidgetRect.bottom() - cornerSize, cornerSize, cornerSize, 270.0f, 90.0f);
        roundRectPath.lineTo(LabelWidgetRect.right(), LabelWidgetRect.top() + arcR);
        roundRectPath.arcTo(LabelWidgetRect.right() - cornerSize, LabelWidgetRect.top(), cornerSize, cornerSize, 0.0f, 90.0f);
        roundRectPath.closeSubpath();

        //设置绘笔的颜色，线宽，线型，结束线类型，折角平滑
        painter.setPen(QPen(QBrush(DGuiApplicationHelper::instance()->applicationPalette().highlight().color()), m_outLineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        painter.drawPath(roundRectPath);

        update();
    }

}
