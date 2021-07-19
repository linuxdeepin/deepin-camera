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

#ifndef CIRCLEPUSHBUTTON_H
#define CIRCLEPUSHBUTTON_H

#include <QPushButton>

class QColor;
class QSvgRenderer;

//自定义圆形按钮
class circlePushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit circlePushButton(QWidget *parent = nullptr);
    /**
    * @brief setbackground 设置背景色
    * @param color 背景色
    */
    void setbackground(QColor color);
    /**
    * @brief setPixmap 设置正常显示图片、悬浮图片、点击图片
    * @param normalPath 正常显示图片
    * @param hoverPath 悬浮图片
    * @param pressPath 点击图片
    */
    void setPixmap(QString normalPath, QString hoverPath, QString pressPath);
    /**
    * @brief setSelected 设置选中
    * @param selected 是否选择
    */
    void setSelected(bool selected);
    /**
    * @brief getButtonState 获取按钮状态
    * @return  按钮是否选择
    */
    bool getButtonState();
    /**
    * @brief setButtonRadius 设置按钮半径
    * @param  radius 按钮半径
    */
    void setButtonRadius(int radius);

signals:

public slots:

protected:
    /**
    * @brief paintEvent 绘制事件
    * @param  event 事件参数
    */
    void paintEvent(QPaintEvent *event) override;
    /**
    * @brief enterEvent 鼠标进入事件
    * @param  event 事件参数
    */
    void enterEvent(QEvent *event) override;
    /**
    * @brief leaveEvent 鼠标离开事件
    * @param  event 事件参数
    */
    void leaveEvent(QEvent *event) override;
    /**
    * @brief mousePressEvent 鼠标点击事件
    * @param  event 事件参数
    */
    void mousePressEvent(QMouseEvent *event) override;
    /**
    * @brief mouseReleaseEvent 鼠标释放事件
    * @param  event 事件参数
    */
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_hover;//是否悬浮
    bool m_mousePress;//是否点击
    bool m_isSelected;//是否选中

    QColor m_color;//背景色

    QSvgRenderer *m_normalSvg;//正常SVG
    QSvgRenderer *m_hoverSvg;//悬浮SVG
    QSvgRenderer *m_pressSvg;//点击SVG

    int           m_radius;//半径
};

#endif // CIRCLEPUSHBUTTON_H
