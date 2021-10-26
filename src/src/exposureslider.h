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
#ifndef EXPOSURESLIDER_H
#define EXPOSURESLIDER_H

#include <QObject>
#include <QWidget>
#include <QPainterPath>
#include <QWheelEvent>

#include <DSlider>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ExposureSlider : public QWidget
{
    Q_OBJECT
public:
    ExposureSlider(QWidget *parent = nullptr);
    ~ExposureSlider();

    /**
    * @brief moveToParentLeft 移动到父窗口左边
    */
    void moveToParentLeft();

    /**
    * @brief value 获取当前的曝光值
    */
    int value();

protected:
    /**
    * @brief paintEvent 绘制事件
    * @param  event 事件参数
    */
    void paintEvent(QPaintEvent *event) override;

    /**
    * @brief wheelEvent 鼠标滚轮事件
    * @param  event 事件参数
    */
    void wheelEvent(QWheelEvent *event) override;

    /**
    * @brief keyReleaseEvent 尺寸大小改变事件
    * @param  event 事件参数
    */
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    /**
    * @brief onValueChanged 曝光值改变槽函数
    * @param  value 曝光值
    */
    void onValueChanged(int value);

signals:
    /**
    * @brief valueChanged 曝光值改变信号
    * @param  value 曝光值
    */
    void valueChanged(int value);

private:
    DLabel *m_pLabShowValue;    //曝光值显示
    DSlider *m_slider;          //滑动条
    int m_valueMax;             //最大值
    int m_valueMin;            //最小值
    int m_curValue;            //当前值

};

#endif // EXPOSURESLIDER_H
