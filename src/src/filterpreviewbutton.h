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

#ifndef FILTERPREVIEWBUTTON_H
#define FILTERPREVIEWBUTTON_H

#include <QLabel>
#include <DWidget>

enum efilterType
{
    filter_Normal, //标准
    filter_Vivid,  //鲜明
    filter_Warm,   //暖色
    filter_Cold,   //冷色
    filter_Retro,  //复古
    filter_Film,   //冲印
    filter_Gray,   //灰度
    filter_BAndW,  //黑白
    filter_Count
};

class QHBoxLayout;
class QLabel;
class QColor;
//滤镜预览按钮
class filterPreviewButton : public Dtk::Widget::DWidget
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ getOpacity WRITE setOpacity)//自定义不透明度属性
    Q_PROPERTY(int rotate READ getRotate WRITE setRotate)//自定义不透明度属性
public:
    explicit filterPreviewButton(QWidget *parent = nullptr, efilterType = filter_Normal);
    ~filterPreviewButton();

    static QString filterName(efilterType type);
    static QString filterName_CUBE(efilterType type);

    /**
    * @brief setbackground 设置背景色
    * @param color 背景色
    */
    void setbackground(QColor color);

    /**
    * @brief setImage 设置滤镜预览图片
    * @param img 滤镜预览图片
    */
    void setImage(QImage* img);

    /**
    * @brief setSelected 设置选中
    * @param selected 是否选择
    */
    void setSelected(bool selected);

    /**
    * @brief getButtonState 获取按钮状态
    * @return  按钮是否选择
    */
    bool isSelected();

    /**
    * @brief setButtonRadius 设置按钮半径
    * @param  radius 按钮半径
    */
    void setButtonRadius(int radius);

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

    /**
    * @brief getFiltertype 获取滤镜类型
    */
    efilterType getFiltertype();

signals:
    void clicked(bool isShortcut = false);

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

    QHBoxLayout* m_mainLayout;
    QLabel* m_label;//用来显示滤镜预览图像

    bool m_hover;//是否悬浮
    bool m_mousePress;//是否点击
    bool m_isSelected;//是否选中

    QColor m_color;//背景色

    int           m_radius;//半径
    bool          m_disableSelect;//禁用选中
    int           m_opacity = 102;    //不透明度 默认值102
    int           m_rotate = 0; //旋转角度

    efilterType    m_filterType = filter_Normal;
};

typedef QList<filterPreviewButton*> filterPreviewBtnList;

#endif // FILTERPREVIEWBUTTON_H
