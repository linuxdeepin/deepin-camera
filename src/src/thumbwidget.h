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

#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include <QObject>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ThumbWidget : public DLabel
{
    Q_OBJECT
public:
    ThumbWidget(QWidget *parent = 0, int outlinewidth = 2);

protected:

    /**
    * @brief focusInEvent　焦点进入事件
    * @arg event 事件
    */
    void focusInEvent(QFocusEvent *event)override;

    /**
    * @brief focusOutEvent　焦点离开事件
    * @arg event 事件
    */
    void focusOutEvent(QFocusEvent *event)override;

    /**
    * @brief paintEvent 绘制事件
    * @arg event 事件
    */
    void paintEvent(QPaintEvent *event)override;

private:
    /**
    * @arg m_tabFocusStatus tab焦点状态
    */
    bool m_tabFocusStatus;

    /**
    * @arg m_outLineWidth 矩形线框宽度
    */
    const int m_outLineWidth;
};

#endif // THUMBWIDGET_H
