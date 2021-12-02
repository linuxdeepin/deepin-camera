/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     houchengqiu <houchengqiu@uniontech.com>
*
* Maintainer: houchengqiu <houchengqiu@uniontech.com>
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

#ifndef GRIDLINEWIDGET_H
#define GRIDLINEWIDGET_H

#include "datamanager.h"

#include <QWidget>
#include <QGraphicsItem>

//网格线图元类，用于在QGraphicsScene画布中显示网格线，与网格线部件互斥显示
class GridLineItem : public QGraphicsItem {
public:
    explicit GridLineItem(QGraphicsItem* parent = nullptr);
    void setGridType(GridType type);

    virtual QRectF boundingRect() const override;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    GridType m_gridType;
    QRectF m_boundingRect;
};

class videowidget;
//网格线部件类，用于在OpenGL窗口中显示网格线，与网格线图元互斥显示
class GridLineWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GridLineWidget(QWidget *parent = nullptr);
    void setGridType(GridType type);

protected:
    virtual void paintEvent(QPaintEvent * e) override;

private:
    GridType m_gridType;
    videowidget *m_videowidget;
};

#endif // GRIDLINEWIDGET_H
