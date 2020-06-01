/*
 * (c) 2020, Uniontech Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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

    void takepic();
    void threeShots();
    void takeVideo();
    void chooseEffect();
    void moreEffectLeft();
    void moreEffectRight();
public slots:
    void set_btn_state_no_dev();
    void set_btn_state_wth_dev();


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
