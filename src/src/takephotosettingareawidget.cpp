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

#define HIDE_NIMATION_DURATION 100
#define ANIMATION_DURATION 200

takePhotoSettingAreaWidget::takePhotoSettingAreaWidget(QWidget *parent) : QWidget(parent)
    , m_btnHeightOffset(20)//暂时间隔设定为20,需确定后修改
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
{
    m_delayGroupDisplay = false;
    m_flashGroupDisplay = false;
    m_buttonGroupColor.setRgb(0, 0, 0, 255 * 0.4);
}

void takePhotoSettingAreaWidget::initButtons()
{
    QColor tmpColor;
    tmpColor.setRgb(1, 1, 1);

    this->setObjectName(RIGHT_BTNS_BOX);
    this->setAccessibleName(RIGHT_BTNS_BOX);

    m_foldBtn = new circlePushButton(this);
    m_foldBtn->setDisableSelect(true);
    m_foldBtn->setPixmap(":/images/camera/fold.svg", ":/images/camera/fold.svg", ":/images/camera/fold.svg");
    m_foldBtn->setObjectName(UNFOLD_BTN);
    m_foldBtn->setAccessibleName(UNFOLD_BTN);

    m_unfoldBtn = new circlePushButton(this);
    m_unfoldBtn->setPixmap(":/images/camera/unfold.svg", ":/images/camera/unfold.svg", ":/images/camera/unfold.svg");
    m_unfoldBtn->setDisableSelect(true);
    m_unfoldBtn->setObjectName(FOLD_BTN);
    m_unfoldBtn->setAccessibleName(FOLD_BTN);

    m_flashlightFoldBtn = new circlePushButton(this);
    m_flashlightFoldBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightFoldBtn->setDisableSelect(true);
    m_flashlightFoldBtn->setbackground(tmpColor);
    m_flashlightFoldBtn->setObjectName(FLASHLITE_FOLD_BTN);
    m_flashlightFoldBtn->setAccessibleName(FLASHLITE_FOLD_BTN);

    m_flashlightUnfoldBtn = new circlePushButton(this);
    m_flashlightUnfoldBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");
    m_flashlightUnfoldBtn->setDisableSelect(true);
    m_flashlightUnfoldBtn->setObjectName(FLASHLITE_UNFOLD_BTN);
    m_flashlightUnfoldBtn->setAccessibleName(FLASHLITE_UNFOLD_BTN);
    m_flashlightUnfoldBtn->setToolTip(tr("Flashlight"));

    m_flashlightOnBtn = new circlePushButton(this);
    m_flashlightOnBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightOnBtn->setObjectName(FLASHLITE_ON_BTN);
    m_flashlightOnBtn->setAccessibleName(FLASHLITE_ON_BTN);
    m_flashlightOnBtn->setToolTip(tr("On"));

    m_flashlightOffBtn = new circlePushButton(this);
    m_flashlightOffBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");
    m_flashlightOffBtn->setObjectName(FLASHLITE_OFF_BTN);
    m_flashlightOffBtn->setAccessibleName(FLASHLITE_OFF_BTN);
    m_flashlightOffBtn->setToolTip(tr("Off"));

    m_delayFoldBtn = new circlePushButton(this);
    m_delayFoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayFoldBtn->setDisableSelect(true);
    m_delayFoldBtn->setbackground(tmpColor);
    m_delayFoldBtn->setObjectName(DELAY_FOLD_BTN);
    m_delayFoldBtn->setAccessibleName(DELAY_FOLD_BTN);

    m_delayUnfoldBtn = new circlePushButton(this);
    m_delayUnfoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayUnfoldBtn->setDisableSelect(true);
    m_delayUnfoldBtn->setObjectName(DELAY_FOLD_BTN);
    m_delayUnfoldBtn->setAccessibleName(DELAY_FOLD_BTN);
    m_delayUnfoldBtn->setToolTip(tr("Delay capture"));

    m_noDelayBtn = new circlePushButton(this);
    m_noDelayBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_noDelayBtn->setObjectName(DELAY_UNFOLD_BTN);
    m_noDelayBtn->setAccessibleName(DELAY_UNFOLD_BTN);

    m_delay3SecondBtn = new circlePushButton(this);
    m_delay3SecondBtn->setPixmap(":/images/camera/delay3s.svg", ":/images/camera/dalay-3s-hover.svg", ":/images/camera/delay-3s-press.svg");
    m_delay3SecondBtn->setObjectName(DELAY_3S_BTN);
    m_delay3SecondBtn->setAccessibleName(DELAY_3S_BTN);

    m_delay6SecondBtn = new circlePushButton(this);
    m_delay6SecondBtn->setPixmap(":/images/camera/delay6S.svg", ":/images/camera/delay-6S-hover.svg", ":/images/camera/delay-6S-press.svg");
    m_delay6SecondBtn->setObjectName(DELAY_6S_BTN);
    m_delay6SecondBtn->setAccessibleName(DELAY_6S_BTN);

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

    connect(m_flashlightOnBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onFlashlightBtnsClicked);
    connect(m_flashlightOffBtn, &QPushButton::clicked, this, &takePhotoSettingAreaWidget::onFlashlightBtnsClicked);

//    showFold(true);
    setFixedSize(QSize(m_unfoldBtn->width(), m_unfoldBtn->height()));
    m_unfoldBtn->setVisible(true);
}

void takePhotoSettingAreaWidget::showFold(bool bShow)
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

    connect(pPosGroup, &QParallelAnimationGroup::finished, [=](){
        m_foldBtn->setVisible(false);
        m_flashlightUnfoldBtn->setVisible(false);
        m_unfoldBtn->setVisible(true);
        setFixedSize(QSize(m_unfoldBtn->width(), m_unfoldBtn->height()));

        group->start();
    });
}

void takePhotoSettingAreaWidget::showUnfold(bool bShow)
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

    m_foldBtn->setVisible(bShow);
    m_flashlightUnfoldBtn->setVisible(bShow);
    m_delayUnfoldBtn->setVisible(bShow);
    m_unfoldBtn->setVisible(!bShow);
    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showDelayButtons(bool bShow)
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
        position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, 0));
        position2->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_btnHeightOffset * 2));

        position3->setDuration(ANIMATION_DURATION);
        position3->setStartValue(QPoint(0, 0));
        position3->setEndValue(QPoint(0, m_delayFoldBtn->height() * 3 + m_btnHeightOffset * 3));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(0);
        opacity->setEndValue(102);

        m_delayFoldBtn->setVisible(bShow);
        m_noDelayBtn->setVisible(bShow);
        m_delay3SecondBtn->setVisible(bShow);
        m_delay6SecondBtn->setVisible(bShow);
    } else {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));
        position1->setEndValue(QPoint(0, 0));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_btnHeightOffset * 2));
        position2->setEndValue(QPoint(0, 0));

        position3->setDuration(ANIMATION_DURATION);
        position3->setStartValue(QPoint(0, m_delayFoldBtn->height() * 3 + m_btnHeightOffset * 3));
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
            showUnfold(true);
        });
    }

    //normal状态下设置按钮透明，只显示背景颜色
    m_noDelayBtn->setbackground(Qt::transparent);
    m_delay3SecondBtn->setbackground(Qt::transparent);
    m_delay6SecondBtn->setbackground(Qt::transparent);

    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);
    pPosGroup->addAnimation(position3);
    pPosGroup->addAnimation(opacity);

    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 4 + 3 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showFlashlights(bool bShow)
{
    QPropertyAnimation *position1 = new QPropertyAnimation(m_flashlightOnBtn, "pos", this);
    QPropertyAnimation *position2 = new QPropertyAnimation(m_flashlightOffBtn, "pos", this);
    QPropertyAnimation *opacity = new QPropertyAnimation(this, "opacity", this);
    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);

    if (bShow) {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, 0));
        position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, 0));
        position2->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_btnHeightOffset * 2));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(0);
        opacity->setEndValue(102);

        m_flashlightFoldBtn->setVisible(bShow);
        m_flashlightOnBtn->setVisible(bShow);
        m_flashlightOffBtn->setVisible(bShow);
    } else {
        position1->setDuration(ANIMATION_DURATION);
        position1->setStartValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));
        position1->setEndValue(QPoint(0, 0));

        position2->setDuration(ANIMATION_DURATION);
        position2->setStartValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_btnHeightOffset * 2));
        position2->setEndValue(QPoint(0, 0));

        opacity->setDuration(ANIMATION_DURATION);
        opacity->setStartValue(102);
        opacity->setEndValue(0);

        connect(pPosGroup, &QPropertyAnimation::finished, this, [=]{
            showUnfold(true);
            m_flashlightFoldBtn->setVisible(bShow);
            m_flashlightOnBtn->setVisible(bShow);
            m_flashlightOffBtn->setVisible(bShow);
            m_flashGroupDisplay = false;
        });
    }

    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);
    pPosGroup->addAnimation(opacity);

    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::foldBtnClicked()
{
//    hideAll();
    showFold(true);
}

void takePhotoSettingAreaWidget::unfoldBtnClicked()
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
        showUnfold(true);
        group->deleteLater();
    });
}

void takePhotoSettingAreaWidget::flashlightFoldBtnClicked()
{
    showFlashlights(false);
//    showUnfold(true);
//    m_flashGroupDisplay = false;
}

void takePhotoSettingAreaWidget::flashlightUnfoldBtnClicked()
{
    hideAll();
    showFlashlights(true);
    m_flashGroupDisplay = true;

    //normal状态下设置按钮透明，只显示背景颜色
    m_flashlightOnBtn->setbackground(Qt::transparent);
    m_flashlightOffBtn->setbackground(Qt::transparent);
}

void takePhotoSettingAreaWidget::delayUnfoldBtnClicked()
{
    hideAll();
    showDelayButtons(true);
    m_delayGroupDisplay = true;
}

void takePhotoSettingAreaWidget::delayfoldBtnClicked()
{
    showDelayButtons(false);
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

void takePhotoSettingAreaWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    if (m_delayGroupDisplay) {
        //先画一个矩形
        QPoint topLeft = m_delayFoldBtn->pos() + QPoint(-1, m_delayFoldBtn->height() * 0.5);
        QPoint bottomRight = m_delay6SecondBtn->pos() + QPoint(m_delayFoldBtn->width(), m_delay6SecondBtn->height() * 0.5);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(m_buttonGroupColor));
        painter.drawRect(QRect(topLeft, bottomRight));

        //再画两个半圆
        painter.drawPie(QRect(m_delayFoldBtn->pos(), m_delayFoldBtn->pos() + QPoint(m_delayFoldBtn->width(), m_delayFoldBtn->height())), 0 * 16, 180 * 16);
        painter.drawPie(QRect(m_delay6SecondBtn->pos(), m_delay6SecondBtn->pos() + QPoint(m_delay6SecondBtn->width(), m_delay6SecondBtn->height())), 180 * 16, 180 * 16);
    } else if (m_flashGroupDisplay) {
        //先画一个矩形
        QPoint topLeft = m_flashlightFoldBtn->pos() + QPoint(-1, m_flashlightFoldBtn->height() * 0.5);
        QPoint bottomRight = m_flashlightOffBtn->pos() + QPoint(m_flashlightFoldBtn->width(), m_flashlightOffBtn->height() * 0.5);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(m_buttonGroupColor));
        painter.drawRect(QRect(topLeft, bottomRight));

        //再画两个半圆
        painter.drawPie(QRect(m_flashlightFoldBtn->pos(), m_flashlightFoldBtn->pos() + QPoint(m_flashlightFoldBtn->width(), m_flashlightFoldBtn->height())), 0 * 16, 180 * 16);
        painter.drawPie(QRect(m_flashlightOffBtn->pos(), m_flashlightOffBtn->pos() + QPoint(m_flashlightOffBtn->width(), m_flashlightOffBtn->height())), 180 * 16, 180 * 16);
    }
}

void takePhotoSettingAreaWidget::onDelayBtnsClicked()
{
    auto pBtn = static_cast<circlePushButton *>(sender());
    if (nullptr == pBtn)
        return;
    pBtn == m_noDelayBtn ? m_noDelayBtn->setSelected(true) : m_noDelayBtn->setSelected(false);
    pBtn == m_delay3SecondBtn ? m_delay3SecondBtn->setSelected(true) : m_delay3SecondBtn->setSelected(false);
    pBtn == m_delay6SecondBtn ? m_delay6SecondBtn->setSelected(true) : m_delay6SecondBtn->setSelected(false);

    int delayTime = 0;
    auto bCopyBtn = m_noDelayBtn;

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

}

/**
* @brief onDelayBtnsClicked 闪光开关按钮点击槽函数
*/
void takePhotoSettingAreaWidget::onFlashlightBtnsClicked()
{
    auto pBtn = static_cast<circlePushButton *>(sender());
    if (nullptr == pBtn)
        return;

    pBtn == m_flashlightOnBtn ? m_flashlightOnBtn->setSelected(true) : m_flashlightOnBtn->setSelected(false);
    pBtn == m_flashlightOffBtn ? m_flashlightOffBtn->setSelected(true) : m_flashlightOffBtn->setSelected(false);

    setFlashlight(m_flashlightOnBtn->getButtonState());
    emit sngSetFlashlight(m_flashlightOnBtn->getButtonState());
}

void takePhotoSettingAreaWidget::setDelayTime(int delayTime)
{
    switch (delayTime) {
    case 0:
        emit m_noDelayBtn->clicked();
        break;
    case 3:
        emit m_delay3SecondBtn->clicked();
        break;
    case 6:
        emit m_delay6SecondBtn->clicked();
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

void takePhotoSettingAreaWidget::resizeEvent(QResizeEvent *event)
{
    moveToParentLeft();
    return QWidget::resizeEvent(event);
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

/**
* @brief closeAllGroup 关闭所有的按钮弹出组
*/
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
