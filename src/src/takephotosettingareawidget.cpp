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

    m_foldBtn = new circlePushButton(this);
    m_foldBtn->setDisableSelect(true);
    m_foldBtn->setPixmap(":/images/camera/fold.svg", ":/images/camera/fold.svg", ":/images/camera/fold.svg");
    m_unfoldBtn = new circlePushButton(this);
    m_unfoldBtn->setPixmap(":/images/camera/unfold.svg", ":/images/camera/unfold.svg", ":/images/camera/unfold.svg");
    m_unfoldBtn->setDisableSelect(true);

    m_flashlightFoldBtn = new circlePushButton(this);
    m_flashlightFoldBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightFoldBtn->setDisableSelect(true);
    m_flashlightFoldBtn->setbackground(tmpColor);
    m_flashlightUnfoldBtn = new circlePushButton(this);
    m_flashlightUnfoldBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");
    m_flashlightUnfoldBtn->setDisableSelect(true);
    m_flashlightOnBtn = new circlePushButton(this);
    m_flashlightOnBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightOffBtn = new circlePushButton(this);
    m_flashlightOffBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");

    m_delayFoldBtn = new circlePushButton(this);
    m_delayFoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayFoldBtn->setDisableSelect(true);
    m_delayFoldBtn->setbackground(tmpColor);
    m_delayUnfoldBtn = new circlePushButton(this);
    m_delayUnfoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayUnfoldBtn->setDisableSelect(true);
    m_noDelayBtn = new circlePushButton(this);
    m_noDelayBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");

    m_delay3SecondBtn = new circlePushButton(this);
    m_delay3SecondBtn->setPixmap(":/images/camera/delay3s.svg", ":/images/camera/dalay-3s-hover.svg", ":/images/camera/delay-3s-press.svg");

    m_delay6SecondBtn = new circlePushButton(this);
    m_delay6SecondBtn->setPixmap(":/images/camera/delay6S.svg", ":/images/camera/delay-6S-hover.svg", ":/images/camera/delay-6S-press.svg");

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

    showFold(true);
}


void takePhotoSettingAreaWidget::showFold(bool bShow)
{
    QPropertyAnimation *position1 = new QPropertyAnimation(m_foldBtn, "pos", this);
    position1->setDuration(ANIMATION_DURATION);
    position1->setStartValue(QPoint(0, m_delayFoldBtn->height() * 2 + 2 * m_btnHeightOffset));
    position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

    QPropertyAnimation *position2 = new QPropertyAnimation(m_flashlightUnfoldBtn, "pos", this);
    position2->setDuration(ANIMATION_DURATION);
    position2->setStartValue(QPoint(0, 0));
    position2->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);
    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);

    pPosGroup->start();

    connect(pPosGroup, &QParallelAnimationGroup::finished, [=](){
        m_foldBtn->setVisible(false);
        m_flashlightUnfoldBtn->setVisible(false);
        m_unfoldBtn->setVisible(true);
        setFixedSize(QSize(m_unfoldBtn->width(), m_unfoldBtn->height()));
        update();
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

    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);
    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);

    m_foldBtn->setVisible(bShow);
    m_flashlightUnfoldBtn->setVisible(bShow);
    m_delayUnfoldBtn->setVisible(bShow);
    m_unfoldBtn->setVisible(false);
    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showDelayButtons(bool bShow)
{
    /*m_delayFoldBtn->setVisible(bShow);
    m_noDelayBtn->setVisible(bShow);
    m_delay3SecondBtn->setVisible(bShow);
    m_delay6SecondBtn->setVisible(bShow);

    //normal状态下设置按钮透明，只显示背景颜色
    m_noDelayBtn->setbackground(Qt::transparent);
    m_delay3SecondBtn->setbackground(Qt::transparent);
    m_delay6SecondBtn->setbackground(Qt::transparent);

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 4 + 3 * m_btnHeightOffset));
    update();*/

    QPropertyAnimation *animate1 = new QPropertyAnimation(m_noDelayBtn, "pos", this);
    animate1->setDuration(ANIMATION_DURATION);
    animate1->setStartValue(QPoint(0, 0));
    animate1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

    QPropertyAnimation *animate2 = new QPropertyAnimation(m_delay3SecondBtn, "pos", this);
    animate2->setDuration(ANIMATION_DURATION);
    animate2->setStartValue(QPoint(0, 0));
    animate2->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_btnHeightOffset * 2));

    QPropertyAnimation *animate3 = new QPropertyAnimation(m_delay6SecondBtn, "pos", this);
    animate3->setDuration(ANIMATION_DURATION);
    animate3->setStartValue(QPoint(0, 0));
    animate3->setEndValue(QPoint(0, m_delayFoldBtn->height() * 3 + m_btnHeightOffset * 3));

    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);
    pPosGroup->addAnimation(animate1);
    pPosGroup->addAnimation(animate2);
    pPosGroup->addAnimation(animate3);

    m_delayFoldBtn->setVisible(bShow);
    m_noDelayBtn->setVisible(bShow);
    m_delay3SecondBtn->setVisible(bShow);
    m_delay6SecondBtn->setVisible(bShow);
    //normal状态下设置按钮透明，只显示背景颜色
    m_noDelayBtn->setbackground(Qt::transparent);
    m_delay3SecondBtn->setbackground(Qt::transparent);
    m_delay6SecondBtn->setbackground(Qt::transparent);

    pPosGroup->start();

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 4 + 3 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showFlashlights(bool bShow)
{
    /*m_flashlightFoldBtn->setVisible(bShow);
    m_flashlightOnBtn->setVisible(bShow);
    m_flashlightOffBtn->setVisible(bShow);
    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_btnHeightOffset));
    update();*/

    QPropertyAnimation *position1 = new QPropertyAnimation(m_flashlightOnBtn, "pos", this);
    position1->setDuration(ANIMATION_DURATION);
    position1->setStartValue(QPoint(0, 0));
    position1->setEndValue(QPoint(0, m_delayFoldBtn->height() + m_btnHeightOffset));

    QPropertyAnimation *position2 = new QPropertyAnimation(m_flashlightOffBtn, "pos", this);
    position2->setDuration(ANIMATION_DURATION);
    position2->setStartValue(QPoint(0, 0));
    position2->setEndValue(QPoint(0, m_delayFoldBtn->height() * 2 + m_btnHeightOffset * 2));

    QParallelAnimationGroup *pPosGroup = new QParallelAnimationGroup(this);
    pPosGroup->addAnimation(position1);
    pPosGroup->addAnimation(position2);

    m_flashlightFoldBtn->setVisible(bShow);
    m_flashlightOnBtn->setVisible(bShow);
    m_flashlightOffBtn->setVisible(bShow);
    pPosGroup->start();
    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_btnHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::unfoldBtnClicked()
{
    hideAll();
    showUnfold(true);
}

void takePhotoSettingAreaWidget::flashlightFoldBtnClicked()
{
    showFlashlights(false);
    showUnfold(true);
    m_flashGroupDisplay = false;
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
    showUnfold(true);
    m_delayGroupDisplay = false;
}

void takePhotoSettingAreaWidget::foldBtnClicked()
{
//    hideAll();
    showFold(true);
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
        emit m_delay3SecondBtn->clicked();
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
