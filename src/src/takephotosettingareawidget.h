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

class QPalette;
class circlePushButton;

class takePhotoSettingAreaWidget : public QWidget
{
    Q_OBJECT
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

signals:

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

    int                      m_butHeightOffset;//按钮之间的间隔


};

#endif // TAKEPHOTOSETTINGAREAWIDGET_H
