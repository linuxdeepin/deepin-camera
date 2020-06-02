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

#include "toolbar.h"
#include <DPushButton>
#include <DButtonBox>
#include <QToolButton>
#include <DIconButton>
#include <DStyle>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

int ToolBar::c = NORMAL;

ToolBar::ToolBar(DWidget *parent) : DWidget(parent)
{
    initUI();
    initConnection();
    //set_btn_state_wth_dev();
}

void ToolBar::initUI()
{
    QHBoxLayout *pHBoxLayout = new QHBoxLayout();
    pHBoxLayout->addStretch();
    pDButtonBox = new DButtonBox();
    pDButtonBox->setFixedHeight(30);
    QList<DButtonBoxButton *> listBtnBox1;
    m_pphoto = new DButtonBoxButton("照片");
    m_pvideo = new DButtonBoxButton("视频");
    m_pphotos = new DButtonBoxButton("连拍");
    listBtnBox1.append(m_pphoto);
    listBtnBox1.append(m_pvideo);
    listBtnBox1.append(m_pphotos);
    pDButtonBox->setButtonList(listBtnBox1, true);
    pHBoxLayout->addWidget(pDButtonBox);
    pHBoxLayout->addStretch();

    m_ptake_photo = new DButtonBoxButton(DStyle::SP_MediaVolumeHighElement);
    m_change_camera = new DButtonBoxButton(DStyle::SP_MediaVolumeHighElement);

    QHBoxLayout *pHBoxlayout1 = new QHBoxLayout();
    pHBoxlayout1->addStretch();
    pDButtonBox1 = new DButtonBox();
    pDButtonBox1->setFixedHeight(30);
    QList<DButtonBoxButton *> listBtnBox2;
    m_pleft_arrow = new DButtonBoxButton(DStyle::SP_ArrowPrev);
    m_peffects = new DButtonBoxButton("特效");
    m_pright_arrow = new DButtonBoxButton(DStyle::SP_ArrowNext);
    listBtnBox2.append(m_pleft_arrow);
    //listBtnBox2.append()

    m_pphoto->setFixedSize(50, 30);
    m_pvideo->setFixedSize(50, 30);
    m_pphotos->setFixedSize(50, 30);
    m_ptake_photo->setFixedSize(100, 30);
    m_pleft_arrow->setFixedSize(30, 30);
    m_peffects->setFixedSize(50, 30);
    m_pright_arrow->setFixedSize(30, 30);
    m_change_camera->setFixedSize(30, 30);

    //m_pleft_arrow->setArrowType(Qt::LeftArrow);
    //m_pright_arrow->setArrowType(Qt::RightArrow);

    QHBoxLayout *pHBoxLayout1 = new QHBoxLayout();
    QHBoxLayout *pHBoxLayout2 = new QHBoxLayout();
    QHBoxLayout *pHBoxLayout3 = new QHBoxLayout();

    pHBoxLayout2->addWidget(m_pphoto);
    pHBoxLayout2->addWidget(m_pvideo);
    pHBoxLayout2->addWidget(m_pphotos);
    pHBoxLayout2->addWidget(m_change_camera);
    pHBoxLayout2->setSpacing(1);

    pHBoxLayout1->addLayout(pHBoxLayout2);
    pHBoxLayout3->setSpacing(10);
    pHBoxLayout1->addWidget(m_ptake_photo);
    pHBoxLayout3->addWidget(m_pleft_arrow);
    pHBoxLayout3->addWidget(m_peffects);
    pHBoxLayout3->addWidget(m_pright_arrow);
    pHBoxLayout1->addLayout(pHBoxLayout3);
    this->setLayout(pHBoxLayout1);


}

void ToolBar::initConnection()
{
    connect(m_ptake_photo, SIGNAL(clicked()), this, SLOT(take_photo_btn_click()));
    connect(m_pphoto, SIGNAL(clicked()), this, SLOT(set_photo()));
    connect(m_pvideo, SIGNAL(clicked()), this, SLOT(set_video()));
    connect(m_pphotos, SIGNAL(clicked()), this, SLOT(set_shoots()));
    connect(m_peffects, SIGNAL(clicked()), this, SLOT(set_effects()));
    connect(m_pleft_arrow, SIGNAL(clicked()), this, SLOT(set_effects_left()));
    connect(m_pright_arrow, SIGNAL(clicked()), this, SLOT(set_effects_right()));
}

void ToolBar::set_btn_state_no_dev()
{
    c = CAMERA_STATE::NORMAL;
    m_pphoto->setCheckable(true);
    if (m_pphoto->isCheckable() == true) {
        qDebug("m_pphoto->isCheckable() == true");
    }
    if (m_pphoto->isChecked() == false && c == CAMERA_STATE::NORMAL) {
        m_pphoto->setChecked(true);
        if (m_pphoto->isChecked() == true) {
            qDebug("m_pphoto is checked!");
        }
    }
    m_pphotos->setEnabled(false);
    m_pvideo->setEnabled(false);
    m_peffects->setEnabled(false);
    m_pleft_arrow->setEnabled(false);
    m_pright_arrow->setEnabled(false);
}

void ToolBar::set_btn_state_taked()
{
    bool clickable = !isCameraClicked;
    m_pphoto->setEnabled(clickable);
    m_pphotos->setEnabled(clickable);
    m_pvideo->setEnabled(clickable);
    m_peffects->setEnabled(clickable);
    m_pleft_arrow->setEnabled(clickable);
    m_pright_arrow->setEnabled(clickable);

    isCameraClicked = clickable;
}

void ToolBar::set_btn_state_effect()
{
    bool clickable = !isEffectClicked;
    m_pphoto->setEnabled(clickable);
    m_pphotos->setEnabled(clickable);
    m_pvideo->setEnabled(clickable);
    m_ptake_photo->setEnabled(clickable);
    isEffectClicked = clickable;
}

void ToolBar::set_btn_state_wth_dev()
{
    set_tackpic();
    m_ptake_photo->setCheckable(false);
}

void ToolBar::set_photo()
{
    c = CAMERA_STATE::PHOTO;
    emit sltPhoto();
}

void ToolBar::set_video()
{
    c = CAMERA_STATE::VIDEO;
    emit sltVideo();
}

void ToolBar::set_shoots()
{
    c = CAMERA_STATE::SHOOTS;
    emit sltThreeShot();
}

void ToolBar::set_effects()
{
    //c = CAMERA_STATE::
    set_btn_state_effect();//特效预览
    emit chooseEffect();
}

void ToolBar::set_effects_left()
{
    emit moreEffectLeft();
}

void ToolBar::set_effects_right()
{
    emit moreEffectRight();
}
void ToolBar::set_tackpic()
{
    c = CAMERA_STATE::PHOTO;
}

void ToolBar::take_photo_btn_click()
{
    if (get_curstate() == CAMERA_STATE::PHOTO) {
        if (isCameraClicked) {//连点时只是重置，不是停止
            set_btn_state_taked();
        }
        createfile1();
        emit takepic();
        //QMessageBox::information(nullptr, "Title", "CAMERA_STATE::PHOTO", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    } else if (get_curstate() == CAMERA_STATE::VIDEO) {
        set_btn_state_taked();//三拍照或视屏
        emit takeVideo();
        //QMessageBox::information(nullptr, "Title", "CAMERA_STATE::VIDEO", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    } else if (get_curstate() == CAMERA_STATE::SHOOTS) {
        set_btn_state_taked();//三拍照或视屏
        //QMessageBox::information(nullptr, "Title", "CAMERA_STATE::SHOOTS", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        createfile();
        emit threeShots();
    } else {
        //QMessageBox::information(nullptr, "Title", "CAMERA_STATE::NORMAL", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    }
}

void ToolBar::onFinishTakedCamera()
{
    set_btn_state_taked();
}
void ToolBar::onFinishEffectChoose()
{
    set_btn_state_effect();
}
void ToolBar::get_pic_frm_dev()
{
    QString m_strPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Pictures/camera");
    QStringList m_imgList;
    QDir dir(m_strPath);
    if (!dir.exists()) {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QStringList filters;
    filters << "*.png" << "*.jpg";
    dir.setNameFilters(filters);
    m_imgList = dir.entryList();
    if (m_imgList.count() <= 0) {
        return;
    }
}

int ToolBar::get_curstate()
{
    return c;
}



void ToolBar::createfile()
{
    QTimer *myTimer = new QTimer();
    //QList<QFile> *qfile = new QList<QFile>;
    QString strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QStringList a = {strPath + "/Pictures/camera/1.txt", strPath + "/Pictures/camera/2.txt", strPath + "/Pictures/camera/3.txt"};
    QStringList content = {"1", "2", "3"};
    for (int i = 0; i < a.count(); i++) {
        QFile file(a.at(i));
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.write(content.at(i).toUtf8());
        file.close();
    }
    myTimer->stop();
}

void ToolBar::createfile1()
{
    //QList<QFile> *qfile = new QList<QFile>;
    QString strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString a = {strPath + "/Pictures/camera/4.txt"};
    QString content = {"4"};
    QFile file(a);
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.write(content.toUtf8());
    file.close();
}




