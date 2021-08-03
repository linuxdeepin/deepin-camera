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
#ifndef TAKEPHOTOSETTINGAREAWIDGET_H
#define TAKEPHOTOSETTINGAREAWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

class QPalette;
class circlePushButton;

class takePhotoSettingAreaWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int opacity READ getOpacity WRITE setOpacity)//自定义不透明度属性
public:
    explicit takePhotoSettingAreaWidget(QWidget *parent = nullptr);
    /**
    * @brief init 初始化
    */
    void init();
    /**
    * @brief showFold 显示收起按钮
    * @param bShow 是否显示
    */
    void showFold(bool bShow);
    /**
    * @brief showUnfold 显示展开
    * @param bShow 是否显示
    */
    void showUnfold(bool bShow);
    /**
    * @brief showDelayButtons 显示延时拍照按钮
    * @param bShow 是否显示
    */
    void showDelayButtons(bool bShow);
    /**
    * @brief showFlashlights 显示闪光灯按钮
    * @param bShow 是否显示
    */
    void showFlashlights(bool bShow);
    /**
    * @brief setDelayTime 设置延时拍照时间
    * @param delayTime 延迟的时间
    */
    void setDelayTime(int delayTime);
    /**
    * @brief setDelayTime 设置是否打开了闪光灯
    * @param bFlashOn 是否打开闪光
    */
    void setFlashlight(bool bFlashOn);

    /**
    * @brief setDelayTime 获取闪光灯状态
    */
    bool flashLight();
    /**
    * @brief moveToParentLeft 移动到父窗口左边
    */
    void moveToParentLeft();

    /**
    * @brief closeAllGroup 关闭所有的按钮弹出组
    */
    void closeAllGroup();

    /**
    * @brief getOpacity 获取当前不透明度
    */
    int getOpacity() { return m_opacity; };

    /**
    * @brief setOpacity 设置不透明度 动画会通过这个函数输入插值，修改透明度。
    * @param  opacity 不透明度 0～255
    */
    void setOpacity(int opacity);

private:
    /**
    * @brief initButtons 初始化所有的按钮
    */
    void initButtons();
    /**
    * @brief initLayout 按钮位置布局
    */
    void initLayout();
    /**
    * @brief hideAll 隐藏所有按钮
    */
    void hideAll();

protected:
    /**
    * @brief paintEvent 绘制事件
    * @param  event 事件参数
    */
    void paintEvent(QPaintEvent *event) override;
    /**
    * @brief resizeEvent 尺寸大小改变事件
    * @param  event 事件参数
    */
    void resizeEvent(QResizeEvent *event);
signals:
    void sngSetDelayTakePhoto(int delayTime);
    void sngSetFlashlight(bool bFlashlightOn);
public slots:
    /**
    * @brief foldBtnClicked 折叠按钮点击槽函数
    */
    void foldBtnClicked();
    /**
    * @brief unfoldBtnClicked 展开按钮点击槽函数
    */
    void unfoldBtnClicked();
    /**
    * @brief flashlightFoldBtnClicked 闪光灯折叠按钮点击槽函数
    */
    void flashlightFoldBtnClicked();
    /**
    * @brief flashlightFoldBtnClicked 闪光灯展开按钮点击槽函数
    */
    void flashlightUnfoldBtnClicked();
    /**
    * @brief delayUnfoldBtnClicked 延时展开按钮点击槽函数
    */
    void delayUnfoldBtnClicked();
    /**
    * @brief delayfoldBtnClicked 延时折叠按钮点击槽函数
    */
    void delayfoldBtnClicked();
    /**
    * @brief onDelayBtnsClicked 延时拍照按钮点击槽函数
    */
    void onDelayBtnsClicked();
    /**
    * @brief onDelayBtnsClicked 闪光开关按钮点击槽函数
    */
    void onFlashlightBtnsClicked();
private:
    circlePushButton        *m_foldBtn;//折叠按钮
    circlePushButton        *m_unfoldBtn;//展开按钮

    circlePushButton        *m_flashlightFoldBtn;//折叠闪光按钮
    circlePushButton        *m_flashlightUnfoldBtn;//展开闪光
    circlePushButton        *m_flashlightOnBtn;//闪光开启按钮
    circlePushButton        *m_flashlightOffBtn;//闪光关闭按钮

    circlePushButton        *m_delayFoldBtn;//折叠延迟拍照
    circlePushButton        *m_delayUnfoldBtn;//展开延迟拍照
    circlePushButton        *m_noDelayBtn;//无延迟
    circlePushButton        *m_delay3SecondBtn;//延迟3s
    circlePushButton        *m_delay6SecondBtn;//延迟6s

    int                      m_btnHeightOffset;//按钮之间的间隔

    QColor                   m_buttonGroupColor;//按钮组展开后背景颜色

    bool                     m_delayGroupDisplay;//延迟按钮组是否显示
    bool                     m_flashGroupDisplay;//闪光灯按钮组是否显示
    int                      m_opacity = 102;    //不透明度 默认值0.4 * 255z
};

#endif // TAKEPHOTOSETTINGAREAWIDGET_H
