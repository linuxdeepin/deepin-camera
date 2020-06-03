/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
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

#ifndef TOOLBAR_H
#define TOOLBAR_H

#include <DWidget>
#include <DPushButton>
#include <QToolButton>
#include <QHBoxLayout>
#include <DButtonBox>
#include <DIconButton>
#include <QStringList>

DWIDGET_USE_NAMESPACE



//sct
//工具栏 添加 拍照 录像 连拍 等操作按钮 界面展示 具体功能调用在actiontoken

enum CAMERA_STATE {NORMAL = 0, PHOTO, VIDEO, SHOOTS}; //照片，视频，连拍

class ToolBar : public DWidget
{
    Q_OBJECT
public:
    explicit ToolBar(DWidget *parent = nullptr);

signals:
    void sltPhoto();
    void sltVideo();
    void sltThreeShot();
    void sltEffect();
    void sltCamera();

    void takepic();
    void threeShots();
    void takeVideo();
    void chooseEffect();
    void moreEffectLeft();
    void moreEffectRight();
public slots:
    void set_btn_state_no_dev();
    void set_btn_state_wth_dev();
    void set_btn_state_enable();
    void set_btn_state_disable();


private slots:
    void set_photo();
    void set_video();
    void set_shoots();
    void set_tackpic();
    void set_effects();
    void set_effects_left();
    void set_effects_right();
    void take_photo_btn_click();
    void get_pic_frm_dev();

    void onFinishTakedCamera();
    void onFinishEffectChoose();

private:
    void initUI();
    void initConnection();
    int get_curstate();

    void createfile();
    void createfile1();

    void set_btn_state_taked();//xxj，被占用时屏蔽按钮
    void set_btn_state_effect();




private:
    DButtonBox *pDButtonBox;
    DButtonBox *pDButtonBox1;
    DButtonBoxButton *m_pphoto;
    DButtonBoxButton *m_pvideo;
    DButtonBoxButton *m_pphotos;
    DButtonBoxButton *m_change_camera;
    DButtonBoxButton *m_ptake_photo;
    DButtonBoxButton *m_pleft_arrow;
    DButtonBoxButton *m_peffects;
    DButtonBoxButton *m_pright_arrow;

    bool isCameraClicked = true;
    bool isEffectClicked = true;
    static int c;

};

#endif // TOOLBAR_H
