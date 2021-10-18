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
#include "takephotosettingareawidget.h"
#include "ac-deepin-camera-define.h"
#include "circlepushbutton.h"

#include <QColor>
#include <QPainter>
#include <QPainterPath>

#define HIDE_NIMATION_DURATION 100
#define ANIMATION_DURATION 200

takePhotoSettingAreaWidget::takePhotoSettingAreaWidget(QWidget *parent) : QWidget(parent)
    , m_btnHeightOffset(20)//暂时间隔设定为20,需确定后修改
    , m_threeBtnOffset(6)
    , m_foldBtn(nullptr)
    , m_unfoldBtn(nullptr)
    , m_flashlightFoldBtn(nullptr)
    , m_flashlightUnfoldBtn(nullptr)
    , m_flashlightOnBtn(nullptr)
    , m_flashlightOffBtn(nullptr)
    , m_delayFoldBtn(nullptr)
    , m_delayUnfoldBtn(nullptr)
    , m_noDelayBtn(nullptr)
    , m_delay3SecondBtn(nullptr)
    , m_delay6SecondBtn(nullptr)
    ,m_bPhoto(true)
{
    m_delayGroupDisplay = false;
    m_flashGroupDisplay = false;
    m_isBtnsFold = true;
    m_buttonGroupColor.setRgb(0, 0, 0, 255 * 0.4);
}

void takePhotoSettingAreaWidget::initButtons()
{
    this->setObjectName(LEFT_BTNS_BOX);
    this->setAccessibleName(LEFT_BTNS_BOX);

    m_unfoldBtn = new circlePushButton(this);
    m_unfoldBtn->setPixmap(":/images/camera/unfold.svg", ":/images/camera/unfold.svg", ":/images/camera/unfold.svg");
    m_unfoldBtn->setDisableSelect(true);
    m_unfoldBtn->setObjectName(UNFOLD_BTN);
    m_unfoldBtn->setAccessibleName(UNFOLD_BTN);

    m_flashlightFoldBtn = new circlePushButton(this);
    m_flashlightFoldBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightFoldBtn->setDisableSelect(true);
    m_flashlightFoldBtn->setObjectName(FLASHLITE_FOLD_BTN);
    m_flashlightFoldBtn->setAccessibleName(FLASHLITE_FOLD_BTN);

    m_flashlightUnfoldBtn = new circlePushButton(this);
    m_flashlightUnfoldBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");
    m_flashlightUnfoldBtn->setDisableSelect(true);
    m_flashlightUnfoldBtn->setObjectName(FLASHLITE_UNFOLD_BTN);
    m_flashlightUnfoldBtn->setAccessibleName(FLASHLITE_UNFOLD_BTN);
    m_flashlightUnfoldBtn->setToolTip(tr("Flashlight"));
    m_flashlightUnfoldBtn->setFocusPolicy(Qt::NoFocus);

    m_flashlightOnBtn = new circlePushButton(this);
    m_flashlightOnBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightOnBtn->setObjectName(FLASHLITE_ON_BTN);
    m_flashlightOnBtn->setAccessibleName(FLASHLITE_ON_BTN);
    m_flashlightOnBtn->setToolTip(tr("On"));
    m_flashlightOnBtn->setFocusPolicy(Qt::NoFocus);

    m_flashlightOffBtn = new circlePushButton(this);
    m_flashlightOffBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");
    m_flashlightOffBtn->setObjectName(FLASHLITE_OFF_BTN);
    m_flashlightOffBtn->setAccessibleName(FLASHLITE_OFF_BTN);
    m_flashlightOffBtn->setToolTip(tr("Off"));
    m_flashlightOffBtn->setFocusPolicy(Qt::NoFocus);

    m_delayFoldBtn = new circlePushButton(this);
    m_delayFoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayFoldBtn->setDisableSelect(true);
    m_delayFoldBtn->setObjectName(DELAY_FOLD_BTN);
    m_delayFoldBtn->setAccessibleName(DELAY_FOLD_BTN);

    m_delayUnfoldBtn = new circlePushButton(this);
    m_delayUnfoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayUnfoldBtn->setDisableSelect(true);
    m_delayUnfoldBtn->setObjectName(DELAY_FOLD_BTN);
    m_delayUnfoldBtn->setAccessibleName(DELAY_FOLD_BTN);
    m_delayUnfoldBtn->setToolTip(tr("Delay capture"));
    m_delayUnfoldBtn->setFocusPolicy(Qt::NoFocus);

    m_noDelayBtn = new circlePushButton(this);
    m_noDelayBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_noDelayBtn->setObjectName(DELAY_UNFOLD_BTN);
    m_noDelayBtn->setAccessibleName(DELAY_UNFOLD_BTN);
    m_noDelayBtn->setToolTip(tr("None"));
    m_noDelayBtn->setFocusPolicy(Qt::NoFocus);

    m_delay3SecondBtn = new circlePushButton(this);
    m_delay3SecondBtn->setPixmap(":/images/camera/delay3s.svg", ":/images/camera/dalay-3s-hover.svg", ":/images/camera/delay-3s-press.svg");
    m_delay3SecondBtn->setObjectName(DELAY_3S_BTN);
    m_delay3SecondBtn->setAccessibleName(DELAY_3S_BTN);
    m_delay3SecondBtn->setFocusPolicy(Qt::NoFocus);

    m_delay6SecondBtn = new circlePushButton(this);
    m_delay6SecondBtn->setPixmap(":/images/camera/delay6S.svg", ":/images/camera/delay-6S-hover.svg", ":/images/camera/delay-6S-press.svg");
    m_delay6SecondBtn->setObjectName(DELAY_6S_BTN);
    m_delay6SecondBtn->setAccessibleName(DELAY_6S_BTN);
    m_delay6SecondBtn->setFocusPolicy(Qt::NoFocus);

    m_foldBtn = new circlePushButton(this);
    m_foldBtn->setDisableSelect(true);
    m_foldBtn->setPixmap(":/images/camera/fold.svg", ":/images/camera/fold.svg", ":/images/camera/fold.svg");
    m_foldBtn->setObjectName(FOLD_BTN);
    m_foldBtn->setAccessibleName(FOLD_BTN);
    m_foldBtn->setFocusPolicy(Qt::NoFocus);

    hideAll();
}

void takePhotoSettingAreaWidget::initLayout()
{
    QPoint pos = m_flashlightUnfoldBtn->pos();
    int btnHeight = m_foldBtn->height();

    m_delayUnfoldBtn->move(pos.x(), pos.y() + btnHeight + m_btnHeightOffset);
    m_foldBtn->move(pos.x(), pos.y() + 2 * btnHeight + 2 * m_btnHeightOffset);

    pos = m_delayFoldBtn->pos();
    m_noDelayBtn->move(pos.x(), pos.y() + btnHeight + m_btnHeightOffset);
    m_delay3SecondBtn->move(pos.x(), pos.y() + 2 * btnHeight + 2 * m_btnHeightOffset);
    m_delay6SecondBtn->move(pos.x(), pos.y() + 3 * btnHeight + 3 * m_btnHeightOffset);

    pos = m_flashlightFoldBtn->pos();
    m_flashlightOnBtn->move(pos.x(), pos.y() + btnHeight + m_btnHeightOffset);
    m_flashlightOffBtn->move(pos.x(), pos.y() + 2 * btnHeight + 2 * m_btnHeightOffset);
}

void takePhotoSettingAreaWidget::init()
{
    initButtons();
    initLayout();

    connect(m_unfoldBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::unfoldBtnClicked);
    connect(m_foldBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::foldBtnClicked);

    connect(m_flashlightUnfoldBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::flashlightUnfoldBtnClicked);
    connect(m_flashlightFoldBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::flashlightFoldBtnClicked);

    connect(m_delayUnfoldBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::delayUnfoldBtnClicked);
    connect(m_delayFoldBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::delayfoldBtnClicked);

    connect(m_noDelayBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onDelayBtnsClicked);
    connect(m_delay3SecondBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onDelayBtnsClicked);
    connect(m_delay6SecondBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onDelayBtnsClicked);

    connect(m_noDelayBtn, &circlePushButton::clicked, this, &takePhotoSettingAreaWidget::onDelayBtnsClicked);
    connect(m_delay3SecondBtn, &circlePushButton::clicked, this, &takePhotoSettingAreaWidget::onDelayBtnsClicked);
    connect(m_delay6SecondBtn, &circlePushButton::clicked, this, &takePhotoSettingAreaWidget::onDelayBtnsClicked);

    connect(m_flashlightOnBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onFlashlightBtnsClicked);
    connect(m_flashlightOffBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onFlashlightBtnsClicked);

    connect(m_flashlightOnBtn, &circlePushButton::clicked, this, &takePhotoSettingAreaWidget::onFlashlightBtnsClicked);
    connect(m_flashlightOffBtn, &circlePushButton::clicked, this, &takePhotoSettingAreaWidget::onFlashlightBtnsClicked);

//    showFold(true);
    setFixedSize(QSize(m_unfoldBtn->width(), m_unfoldBtn->height()));
    m_unfoldBtn->setVisible(true);
}

void takePhotoSettingAreaWidget::showFold(bool bShow, bool isShortcut)
{
    //位移动画
    QPropertyAnimation *position1 = new QPropertyAnimation(m_foldBtn, "pos", this);
    position1->setDuration(ANIMATION_DURATION);
    position1->setStartValue(QPoint(0, m_delayFoldBtn->height() * 2 + 2 * m_btnHeightOffset));
    position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

    QPropertyAnimation *position2 = new QPropertyAnimation(m_flashlightUnfoldBtn, "pos", this);
    position2->setDuration(ANIMATION_DURATION);
    position2->setStartValue(QPoint(0, 0));
    position2->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

    //透明度动画
    QPropertyAnimation *opacity1 = new QPropertyAnimation(m_foldBtn, "opacity", this);
    opacity1->setDuration(ANIMATION_DURATION);
    opacity1->setStartValue(102);
    opacity1->setEndValue(0);

    QPropertyAnimation *opacity2 = new QPropertyAnimation(m_delayUnfoldBtn, "opacity", this);
    opacity2->setDuration(ANIMATION_DURATION);
    opacity2->setStartValue(102);
    opacity2->setEndValue(0);

    QPropertyAnimation *opacity3 = new QPropertyAnimation(m_flashlightUnfoldBtn, "opacity", this);
    opacity3->setDuration(ANIMATION_DURATION);
    opacity3->setStartValue(102);
    opacity3->setEndValue(0);

    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);
    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);
    pPosGroup->addAnimation(opacity1);
    pPosGroup->addAnimation(opacity2);
    pPosGroup->addAnimation(opacity3);

    pPosGroup->start();

    //展开按钮旋转动画
    QPropertyAnimation * opacity = new QPropertyAnimation(m_unfoldBtn, "opacity", this);
    opacity->setStartValue(0);
    opacity->setEndValue(102);
    opacity->setDuration(ANIMATION_DURATION);

    QPropertyAnimation *rotate = new QPropertyAnimation(m_unfoldBtn, "rotate", this);
    rotate->setStartValue(90);
    rotate->setEndValue(0);
    rotate->setDuration(ANIMATION_DURATION);

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(opacity);
    group->addAnimation(rotate);

    connect(group, &QParallelAnimationGroup::finished, this, [=](){
        m_isBtnsFold = true;
        if (isShortcut)
            m_unfoldBtn->setFocus();
    });

    connect(pPosGroup, &QParallelAnimationGroup::finished, [=](){
        m_foldBtn->setVisible(false);
        m_flashlightUnfoldBtn->setVisible(false);
        m_unfoldBtn->setVisible(true);
        setFixedSize(QSize(m_unfoldBtn->width(), m_unfoldBtn->height()));

        group->start();
    });
}

void takePhotoSettingAreaWidget::showUnfold(bool bShow, circlePushButton *btn, bool isShortcut)
{
    //位移动画
    QPropertyAnimation *position1 = new QPropertyAnimation(m_foldBtn, "pos", this);
    position1->setDuration(ANIMATION_DURATION);
    position1->setStartValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));
    position1->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + 2 * m_btnHeightOffset));

    QPropertyAnimation *position2 = new QPropertyAnimation(m_flashlightUnfoldBtn, "pos", this);
    position2->setDuration(ANIMATION_DURATION);
    position2->setStartValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));
    position2->setEndValue(QPoint(0, 0));

    //透明度动画
    QPropertyAnimation *opacity1 = new QPropertyAnimation(m_foldBtn, "opacity", this);
    opacity1->setDuration(ANIMATION_DURATION);
    opacity1->setStartValue(0);
    opacity1->setEndValue(102);

    QPropertyAnimation *opacity2 = new QPropertyAnimation(m_delayUnfoldBtn, "opacity", this);
    opacity2->setDuration(ANIMATION_DURATION);
    opacity2->setStartValue(0);
    opacity2->setEndValue(102);

    QPropertyAnimation *opacity3 = new QPropertyAnimation(m_flashlightUnfoldBtn, "opacity", this);
    opacity3->setDuration(ANIMATION_DURATION);
    opacity3->setStartValue(0);
    opacity3->setEndValue(102);

    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);
    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);
    pPosGroup->addAnimation(opacity1);
    pPosGroup->addAnimation(opacity2);
    pPosGroup->addAnimation(opacity3);

    connect(pPosGroup, &QParallelAnimationGroup::finished, this, [=](){
        m_isBtnsFold = false;
        if (isShortcut) {
            btn->setFocus();
        }
    });

    m_foldBtn->setVisible(bShow);
    if(m_bPhoto) {
        m_flashlightUnfoldBtn->setVisible(bShow);
    } else {
        m_flashlightUnfoldBtn->setVisible(!bShow);
    }
    m_delayUnfoldBtn->setVisible(bShow);
    m_unfoldBtn->setVisible(!bShow);
    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showDelayButtons(bool bShow, bool isShortcut)
{
    //位移动画
    QPropertyAnimation *position1 = new QPropertyAnimation(m_noDelayBtn, "pos", this);
    QPropertyAnimation *position2 = new QPropertyAnimation(m_delay3SecondBtn, "pos", this);
    QPropertyAnimation *position3 = new QPropertyAnimation(m_delay6SecondBtn, "pos", this);
    //透明度动画
    QPropertyAnimation *opacity = new QPropertyAnimation(this, "opacity", this);
    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);

    if (bShow) {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, 0));
        position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_threeBtnOffset));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, 0));
        position2->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_threeBtnOffset * 2));

        position3->setDuration(ANIMATION_DURATION);
        position3->setStartValue(QPoint(0, 0));
        position3->setEndValue(QPoint(0, m_delayFoldBtn->height() * 3 + m_threeBtnOffset * 3));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(0);
        opacity->setEndValue(102);

        m_delayFoldBtn->setVisible(bShow);
        m_noDelayBtn->setVisible(bShow);
        m_delay3SecondBtn->setVisible(bShow);
        m_delay6SecondBtn->setVisible(bShow);
        connect(pPosGroup, &QPropertyAnimation::finished, this, [=]{
            if (isShortcut)
                m_delayFoldBtn->setFocus();
        });
    } else {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, m_delayFoldBtn->height() + m_threeBtnOffset));
        position1->setEndValue(QPoint(0, 0));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_threeBtnOffset * 2));
        position2->setEndValue(QPoint(0, 0));

        position3->setDuration(ANIMATION_DURATION);
        position3->setStartValue(QPoint(0, m_delayFoldBtn->height() * 3 + m_threeBtnOffset * 3));
        position3->setEndValue(QPoint(0, 0));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(102);
        opacity->setEndValue(0);

        connect(pPosGroup, &QPropertyAnimation::finished, this, [=](){
            m_delayFoldBtn->setVisible(bShow);
            m_noDelayBtn->setVisible(bShow);
            m_delay3SecondBtn->setVisible(bShow);
            m_delay6SecondBtn->setVisible(bShow);
            m_delayGroupDisplay = false;
            showUnfold(true, m_delayUnfoldBtn, isShortcut);
        });
    }

    //normal状态下设置按钮透明，只显示背景颜色
    m_delayFoldBtn->setOpacity(255 * 0.4);
    m_noDelayBtn->setbackground(Qt::transparent);
    m_delay3SecondBtn->setbackground(Qt::transparent);
    m_delay6SecondBtn->setbackground(Qt::transparent);

    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);
    pPosGroup->addAnimation(position3);
    pPosGroup->addAnimation(opacity);

    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 4 + m_threeBtnOffset * 3 + 2));
    update();
}

void takePhotoSettingAreaWidget::showFlashlights(bool bShow, bool isShortcut)
{
    QPropertyAnimation *position1 = new QPropertyAnimation(m_flashlightOnBtn, "pos", this);
    QPropertyAnimation *position2 = new QPropertyAnimation(m_flashlightOffBtn, "pos", this);
    QPropertyAnimation *opacity = new QPropertyAnimation(this, "opacity", this);
    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);

    if (bShow) {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, 0));
        position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_threeBtnOffset));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, 0));
        position2->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_threeBtnOffset * 2));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(0);
        opacity->setEndValue(102);

        m_flashlightFoldBtn->setVisible(bShow);
        m_flashlightOnBtn->setVisible(bShow);
        m_flashlightOffBtn->setVisible(bShow);

        connect(pPosGroup, &QPropertyAnimation::finished, this, [=]{
            if (isShortcut)
                m_flashlightFoldBtn->setFocus();
        });
    } else {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, m_delayFoldBtn->height() + m_threeBtnOffset));
        position1->setEndValue(QPoint(0, 0));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_threeBtnOffset * 2));
        position2->setEndValue(QPoint(0, 0));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(102);
        opacity->setEndValue(0);

        connect(pPosGroup, &QPropertyAnimation::finished, this, [=]{
            m_flashlightFoldBtn->setVisible(bShow);
            m_flashlightOnBtn->setVisible(bShow);
            m_flashlightOffBtn->setVisible(bShow);
            m_flashGroupDisplay = false;
            showUnfold(true, m_flashlightUnfoldBtn, isShortcut);
        });
    }

    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);
    pPosGroup->addAnimation(opacity);

    pPosGroup->start();

    setFixedSize(QSize(m_flashlightFoldBtn->width(), m_flashlightFoldBtn->height() * 3 + m_threeBtnOffset * 2 + 2));
    update();
}

void takePhotoSettingAreaWidget::foldBtnClicked(bool isShortcut)
{
//    hideAll();
    showFold(true, isShortcut);
}

void takePhotoSettingAreaWidget::unfoldBtnClicked(bool isShortcut)
{
    QPropertyAnimation * opacity = new QPropertyAnimation(m_unfoldBtn, "opacity", this);
    opacity->setStartValue(102);
    opacity->setEndValue(0);
    opacity->setDuration(ANIMATION_DURATION);

    QPropertyAnimation *rotate = new QPropertyAnimation(m_unfoldBtn, "rotate", this);
    rotate->setStartValue(0);
    rotate->setEndValue(90);
    rotate->setDuration(ANIMATION_DURATION);

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(opacity);
    group->addAnimation(rotate);

    group->start();

    connect(group, &QParallelAnimationGroup::finished, this, [=](){
        if (m_bPhoto) {
            showUnfold(true, m_flashlightUnfoldBtn, isShortcut);
        } else {
            showUnfold(true, m_delayUnfoldBtn, isShortcut);
        }
        group->deleteLater();

    });
}

void takePhotoSettingAreaWidget::flashlightFoldBtnClicked(bool isShortcut)
{
    showFlashlights(false, isShortcut);
//    showUnfold(true);
//    m_flashGroupDisplay = false;
}

void takePhotoSettingAreaWidget::flashlightUnfoldBtnClicked(bool isShortcut)
{
    hideAll();
    showFlashlights(true, isShortcut);
    m_flashGroupDisplay = true;

    //normal状态下设置按钮透明，只显示背景颜色
    m_flashlightFoldBtn->setOpacity(255 * 0.4);
    m_flashlightOnBtn->setbackground(Qt::transparent);
    m_flashlightOffBtn->setbackground(Qt::transparent);
}

void takePhotoSettingAreaWidget::delayUnfoldBtnClicked(bool isShortcut)
{
    hideAll();
    showDelayButtons(true, isShortcut);
    m_delayGroupDisplay = true;
}

void takePhotoSettingAreaWidget::delayfoldBtnClicked(bool isShortcut)
{
    showDelayButtons(false, isShortcut);
//    showUnfold(true);
//    m_delayGroupDisplay = false;
}

void takePhotoSettingAreaWidget::hideAll()
{
    m_foldBtn->setVisible(false);
    m_unfoldBtn->setVisible(false);
    m_flashlightFoldBtn->setVisible(false);
    m_flashlightUnfoldBtn->setVisible(false);
    m_flashlightOnBtn->setVisible(false);
    m_flashlightOffBtn->setVisible(false);
    m_delayFoldBtn->setVisible(false);
    m_delayUnfoldBtn->setVisible(false);
    m_noDelayBtn->setVisible(false);
    m_delay3SecondBtn->setVisible(false);
    m_delay6SecondBtn->setVisible(false);
}

void takePhotoSettingAreaWidget::keyDownClick()
{
    if (!m_isBtnsFold) {
        if (m_flashlightUnfoldBtn->hasFocus()) {
            m_delayUnfoldBtn->setFocus();
        } else if(m_delayUnfoldBtn->hasFocus()){
            m_foldBtn->setFocus();
        } else if(m_foldBtn->hasFocus()){
            m_flashlightUnfoldBtn->setFocus();
        }
    }

    if (m_flashGroupDisplay) {
        if (m_flashlightFoldBtn->hasFocus()) {
            m_flashlightOnBtn->setFocus();
        } else if(m_flashlightOnBtn->hasFocus()){
            m_flashlightOffBtn->setFocus();
        } else if(m_flashlightOffBtn->hasFocus()){
            m_flashlightFoldBtn->setFocus();
        }
    }

    if (m_delayGroupDisplay) {
        if (m_delayFoldBtn->hasFocus()) {
            m_noDelayBtn->setFocus();
        } else if(m_noDelayBtn->hasFocus()) {
            m_delay3SecondBtn->setFocus();
        } else if(m_delay3SecondBtn->hasFocus()) {
            m_delay6SecondBtn->setFocus();
        } else if(m_delay6SecondBtn->hasFocus()) {
            m_delayFoldBtn->setFocus();
        }
    }
}

void takePhotoSettingAreaWidget::keyUpClick()
{
    if (!m_isBtnsFold) {
        if (m_flashlightUnfoldBtn->hasFocus()) {
            m_foldBtn->setFocus();
        } else if(m_delayUnfoldBtn->hasFocus()){
            m_flashlightUnfoldBtn->setFocus();
        } else if(m_foldBtn->hasFocus()){
            m_delayUnfoldBtn->setFocus();
        }
    }

    if (m_flashGroupDisplay) {
        if (m_flashlightFoldBtn->hasFocus()) {
            m_flashlightOffBtn->setFocus();
        } else if(m_flashlightOnBtn->hasFocus()){
            m_flashlightFoldBtn->setFocus();
        } else if(m_flashlightOffBtn->hasFocus()){
            m_flashlightOnBtn->setFocus();
        }
    }

    if (m_delayGroupDisplay) {
        if (m_delayFoldBtn->hasFocus()) {
            m_delay6SecondBtn->setFocus();
        } else if(m_noDelayBtn->hasFocus()) {
            m_delayFoldBtn->setFocus();
        } else if(m_delay3SecondBtn->hasFocus()) {
            m_noDelayBtn->setFocus();
        } else if(m_delay6SecondBtn->hasFocus()) {
            m_delay3SecondBtn->setFocus();
        }
    }
}

void takePhotoSettingAreaWidget::keyEnterClick()
{
    if (m_unfoldBtn->hasFocus()) {
        unfoldBtnClicked(true);
    } else if (m_foldBtn->hasFocus()) {
        foldBtnClicked(true);
    } else if (m_flashlightUnfoldBtn->hasFocus()) {
        flashlightUnfoldBtnClicked(true);
    } else if (m_flashlightFoldBtn->hasFocus()) {
        flashlightFoldBtnClicked(true);
    } else if (m_flashlightOnBtn->hasFocus()) {
        emit m_flashlightOnBtn->clicked(true);
    } else if (m_flashlightOffBtn->hasFocus()) {
        emit m_flashlightOffBtn->clicked(true);
    } else if (m_delayFoldBtn->hasFocus()) {
        delayfoldBtnClicked(true);
    } else if (m_delayUnfoldBtn->hasFocus()) {
        delayUnfoldBtnClicked(true);
    } else if (m_noDelayBtn->hasFocus()) {
        emit m_noDelayBtn->clicked(true);
    } else if (m_delay3SecondBtn->hasFocus()) {
        emit m_delay3SecondBtn->clicked(true);
    } else if (m_delay6SecondBtn->hasFocus()) {
        emit m_delay6SecondBtn->clicked(true);
    }
}

void takePhotoSettingAreaWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    int width, height;

    if (m_delayGroupDisplay) {
        QPainterPath path;  //上半圆+矩形
        QPainterPath arc1;  //下半圆

        width = m_delayFoldBtn->width();
        height = m_delayFoldBtn->height();

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(m_buttonGroupColor));

        path.moveTo(m_delayFoldBtn->rect().center());
        path.arcTo(m_delayFoldBtn->rect(), 0, 180);
        path.lineTo(m_delay6SecondBtn->x(), m_delay6SecondBtn->y() + height / 2);
        path.lineTo(m_delay6SecondBtn->x() + width, m_delay6SecondBtn->y() + height / 2);
        path.lineTo(m_delayFoldBtn->x() + width, m_delayFoldBtn->y() + height / 2);

        arc1.moveTo(m_delay6SecondBtn->x() + width / 2, m_delay6SecondBtn->y() + height / 2);
        arc1.arcTo(QRect(m_delay6SecondBtn->x(), m_delay6SecondBtn->y(), width, height), 180, 180);

        path.addPath(arc1);
        painter.fillPath(path, QBrush(m_buttonGroupColor));

    } else if (m_flashGroupDisplay) {
        QPainterPath path;
        QPainterPath arc1;

        width = m_flashlightFoldBtn->width();
        height = m_flashlightFoldBtn->height();

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(m_buttonGroupColor));

        path.moveTo(m_flashlightFoldBtn->rect().center());
        path.arcTo(m_flashlightFoldBtn->rect(), 0, 180);
        path.lineTo(m_flashlightOffBtn->x(), m_flashlightOffBtn->y() + height / 2);
        path.lineTo(m_flashlightOffBtn->x() + width, m_flashlightOffBtn->y() + height / 2);
        path.lineTo(m_flashlightFoldBtn->x() + width, m_flashlightFoldBtn->y() + height / 2);

        arc1.moveTo(m_flashlightOffBtn->x() + width / 2, m_flashlightOffBtn->y() + height / 2);
        arc1.arcTo(QRect(m_flashlightOffBtn->x(), m_flashlightOffBtn->y(), width, height), 180, 180);

        path.addPath(arc1);
        painter.fillPath(path, QBrush(m_buttonGroupColor));
    }
}

void takePhotoSettingAreaWidget::onDelayBtnsClicked(bool isShortcut)
{
    circlePushButton *pBtn = static_cast<circlePushButton *>(sender());
    if (nullptr == pBtn)
        return;
    pBtn == m_noDelayBtn ? m_noDelayBtn->setSelected(true) : m_noDelayBtn->setSelected(false);
    pBtn == m_delay3SecondBtn ? m_delay3SecondBtn->setSelected(true) : m_delay3SecondBtn->setSelected(false);
    pBtn == m_delay6SecondBtn ? m_delay6SecondBtn->setSelected(true) : m_delay6SecondBtn->setSelected(false);

    int delayTime = 0;
    circlePushButton *bCopyBtn = m_noDelayBtn;

    if (m_delay3SecondBtn->getButtonState()) {
        delayTime = 3;
        bCopyBtn = m_delay3SecondBtn;
    }

    if (m_delay6SecondBtn->getButtonState()) {
        delayTime = 6;
        bCopyBtn = m_delay6SecondBtn;
    }

    m_delayUnfoldBtn->copyPixmap(*bCopyBtn);
    m_delayFoldBtn->copyPixmap(*bCopyBtn);
    emit sngSetDelayTakePhoto(delayTime);
    update();

    showDelayButtons(false, isShortcut); //三级菜单选中后跳转到上一级菜单
}

void takePhotoSettingAreaWidget::onFlashlightBtnsClicked(bool isShortcut)
{
    auto pBtn = static_cast<circlePushButton *>(sender());
    if (nullptr == pBtn)
        return;

    pBtn == m_flashlightOnBtn ? m_flashlightOnBtn->setSelected(true) : m_flashlightOnBtn->setSelected(false);
    pBtn == m_flashlightOffBtn ? m_flashlightOffBtn->setSelected(true) : m_flashlightOffBtn->setSelected(false);

    setFlashlight(m_flashlightOnBtn->getButtonState());
    emit sngSetFlashlight(m_flashlightOnBtn->getButtonState());

    showFlashlights(false, isShortcut);
}

void takePhotoSettingAreaWidget::setDelayTime(int delayTime)
{
    m_noDelayBtn->setSelected(false);
    m_delay3SecondBtn->setSelected(false);
    m_delay6SecondBtn->setSelected(false);
    switch (delayTime) {
    case 0:
//        emit m_noDelayBtn->clicked(); //不使用触发按钮点击方式实现，防止影响动画效果
        m_noDelayBtn->setSelected(true);
        m_delayUnfoldBtn->copyPixmap(*m_noDelayBtn);
        m_delayFoldBtn->copyPixmap(*m_noDelayBtn);
        update();
        break;
    case 3:
//        emit m_delay3SecondBtn->clicked();
        m_delay3SecondBtn->setSelected(true);
        m_delayUnfoldBtn->copyPixmap(*m_delay3SecondBtn);
        m_delayFoldBtn->copyPixmap(*m_delay3SecondBtn);
        update();
        break;
    case 6:
//        emit m_delay6SecondBtn->clicked();
        m_delay6SecondBtn->setSelected(true);
        m_delayUnfoldBtn->copyPixmap(*m_delay6SecondBtn);
        m_delayFoldBtn->copyPixmap(*m_delay6SecondBtn);
        update();
        break;
    default:
        ;
    }
}

void takePhotoSettingAreaWidget::setFlashlight(bool bFlashOn)
{
    m_flashlightOnBtn->setSelected(bFlashOn);
    m_flashlightOffBtn->setSelected(!bFlashOn);
    auto p = bFlashOn ? m_flashlightOnBtn : m_flashlightOffBtn;
    m_flashlightUnfoldBtn->copyPixmap(*p);
    m_flashlightFoldBtn->copyPixmap(*p);
    update();
}

bool takePhotoSettingAreaWidget::flashLight()
{
    return m_flashlightOnBtn->getButtonState();
}

void takePhotoSettingAreaWidget::resizeEvent(QResizeEvent *event)
{
    moveToParentLeft();
    return QWidget::resizeEvent(event);
}

void takePhotoSettingAreaWidget::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Down:
        keyDownClick();
        break;
    case Qt::Key_Up:
        keyUpClick();
        break;
    case Qt::Key_Return:
        keyEnterClick();
        break;
    }
    QWidget::keyReleaseEvent(event);
}

void takePhotoSettingAreaWidget::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    if (m_isBtnsFold) {
        m_unfoldBtn->setFocus();
    } else if (!m_bPhoto) {
        if (m_delayGroupDisplay) {
            m_delayFoldBtn->setFocus();
        } else {
            m_delayUnfoldBtn->setFocus();
        }
    } else {
        if (m_flashGroupDisplay) {
            m_flashlightFoldBtn->setFocus();
        } else if (m_delayGroupDisplay) {
            m_delayFoldBtn->setFocus();
        } else {
            m_flashlightUnfoldBtn->setFocus();
        }
    }
    update();
}

void takePhotoSettingAreaWidget::moveToParentLeft()
{
    if (nullptr == parent())
        return;

    auto pParentWidget = static_cast<QWidget *>(parent());

    if (nullptr == pParentWidget)
        return;

    move(20, pParentWidget->height() / 2 - height() / 2);
}

void takePhotoSettingAreaWidget::closeAllGroup()
{
    //判断延迟拍照组是否显示
    if (m_delayGroupDisplay)
        delayfoldBtnClicked();

    //判断闪光灯拍照组是否显示
    if (m_flashGroupDisplay)
        flashlightFoldBtnClicked();
}

void takePhotoSettingAreaWidget::setOpacity(int opacity)
{
    m_opacity = opacity;
    m_buttonGroupColor.setAlpha(opacity);
    update();
};

void takePhotoSettingAreaWidget::setState(bool bPhoto)
{
    m_bPhoto = bPhoto;
    if (bPhoto && !m_isBtnsFold) {
        m_flashlightUnfoldBtn->setVisible(true);
    } else {
        m_flashlightUnfoldBtn->setVisible(false);
    }
    update();
}
