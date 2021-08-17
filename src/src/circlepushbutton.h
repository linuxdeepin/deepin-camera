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
    Q_PROPERTY(int opacity READ getOpacity WRITE setOpacity)//自定义不透明度属性
    Q_PROPERTY(int rotate READ getRotate WRITE setRotate)//自定义不透明度属性
public:
    explicit circlePushButton(QWidget *parent = nullptr);
    ~circlePushButton();
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
    /**
    * @brief copyPixmap 拷贝另一个按钮的图标
    * @param  other 另外一个按钮
    */
    void copyPixmap(const circlePushButton &other);
    /**
    * @brief copyPixmap 禁用选中
    * @param  disable 是否禁用选中
    */
    void setDisableSelect(bool disable);

    /**
    * @brief getOpacity 获取当前不透明度
    */
    int getOpacity() { return m_opacity; };

    /**
    * @brief getRotate 获取当前旋转角度
    */
    int getRotate() { return m_rotate; };

    /**
    * @brief setOpacity 设置不透明度 动画会通过这个函数输入插值，修改透明度。
    * @param  opacity 不透明度 0～255
    */
    void setOpacity(int opacity);

    /**
    * @brief setRotate 设置旋转角度
    * @param  rotate 角度值 0～360
    */
    void setRotate(int rotate);

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
    * @brief focusInEvent 焦点进入事件
    * @param  event 事件参数
    */
    void focusInEvent(QFocusEvent *event) override;

    /**
    * @brief focusOutEvent 焦点离开事件
    * @param  event 事件参数
    */
    void focusOutEvent(QFocusEvent *event) override;

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

    QSharedPointer<QSvgRenderer> m_normalSvg;//正常SVG
    QSharedPointer<QSvgRenderer> m_hoverSvg;//悬浮SVG
    QSharedPointer<QSvgRenderer> m_pressSvg;//点击SVG

    int           m_radius;//半径
    bool          m_disableSelect;//禁用选中
    int           m_opacity = 102;    //不透明度 默认值102
    int           m_rotate = 0; //旋转角度
};

#endif // CIRCLEPUSHBUTTON_H
