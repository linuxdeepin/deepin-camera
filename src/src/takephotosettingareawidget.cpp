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

takePhotoSettingAreaWidget::takePhotoSettingAreaWidget(QWidget *parent) : QWidget(parent)
    , m_butHeightOffset(20)//暂时间隔设定为20,需确定后修改
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
    m_buttonGroupColor.setRgb(0, 0, 0, 255 * 0.4);
}

void takePhotoSettingAreaWidget::initButtons()
{
    m_foldBtn = new circlePushButton(this);
    m_foldBtn->setPixmap(":/images/camera/fold.svg", ":/images/camera/fold.svg", ":/images/camera/fold-press.svg");
    m_unfoldBtn = new circlePushButton(this);
    m_unfoldBtn->setPixmap(":/images/camera/fold.svg", ":/images/camera/fold.svg", ":/images/camera/fold-press.svg");

    m_flashlightFoldBtn = new circlePushButton(this);
    m_flashlightFoldBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightUnfoldBtn = new circlePushButton(this);
    m_flashlightUnfoldBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");
    m_flashlightOnBtn = new circlePushButton(this);
    m_flashlightOnBtn->setPixmap(":/images/camera/flashlight.svg", ":/images/camera/flashlight-hover.svg", ":/images/camera/flashlight-press.svg");
    m_flashlightOffBtn = new circlePushButton(this);
    m_flashlightOffBtn->setPixmap(":/images/camera/close-flashlight.svg", ":/images/camera/close-flashlight-hover.svg", ":/images/camera/close-flashlight-press.svg");

    m_delayFoldBtn = new circlePushButton(this);
    m_delayFoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");
    m_delayUnfoldBtn = new circlePushButton(this);
    m_delayUnfoldBtn->setPixmap(":/images/camera/delay.svg", ":/images/camera/delay-hover.svg", ":/images/camera/delay-press.svg");

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

    m_delayUnfoldBtn->move(pos.x(), pos.y() + btnHeight + m_butHeightOffset);
    m_foldBtn->move(pos.x(), pos.y() + 2 * btnHeight + 2 * m_butHeightOffset);

    pos = m_delayFoldBtn->pos();
    m_noDelayBtn->move(pos.x(), pos.y() + btnHeight + m_butHeightOffset);
    m_delay3SecondBtn->move(pos.x(), pos.y() + 2 * btnHeight + 2 * m_butHeightOffset);
    m_delay6SecondBtn->move(pos.x(), pos.y() + 3 * btnHeight + 3 * m_butHeightOffset);

    pos = m_flashlightFoldBtn->pos();
    m_flashlightOnBtn->move(pos.x(), pos.y() + btnHeight + m_butHeightOffset);
    m_flashlightOffBtn->move(pos.x(), pos.y() + 2 * btnHeight + 2 * m_butHeightOffset);
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

    showFold(true);
}


void takePhotoSettingAreaWidget::showFold(bool bShow)
{
    m_unfoldBtn->setVisible(bShow);
    setFixedSize(QSize(m_unfoldBtn->width(), m_unfoldBtn->height()));
    update();
}

void takePhotoSettingAreaWidget::showUnfold(bool bShow)
{
    m_foldBtn->setVisible(bShow);
    m_flashlightUnfoldBtn->setVisible(bShow);
    m_delayUnfoldBtn->setVisible(bShow);
    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_butHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showDelayButtons(bool bShow)
{
    m_delayFoldBtn->setVisible(bShow);
    m_noDelayBtn->setVisible(bShow);
    m_delay3SecondBtn->setVisible(bShow);
    m_delay6SecondBtn->setVisible(bShow);

    //normal状态下设置按钮透明，只显示背景颜色
    m_noDelayBtn->setbackground(Qt::transparent);
    m_delay3SecondBtn->setbackground(Qt::transparent);
    m_delay6SecondBtn->setbackground(Qt::transparent);

    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 4 + 3 * m_butHeightOffset));
    update();
}

void takePhotoSettingAreaWidget::showFlashlights(bool bShow)
{
    m_flashlightFoldBtn->setVisible(bShow);
    m_flashlightOnBtn->setVisible(bShow);
    m_flashlightOffBtn->setVisible(bShow);
    setFixedSize(QSize(m_delayFoldBtn->width(), m_delayFoldBtn->height() * 3 + 2 * m_butHeightOffset));
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
}

void takePhotoSettingAreaWidget::flashlightUnfoldBtnClicked()
{
    hideAll();
    showFlashlights(true);
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
    hideAll();
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
        QPoint bottomRight = m_delay6SecondBtn->pos() + QPoint(m_delayFoldBtn->width(), m_delay6SecondBtn->height() * 0.5 + 1);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(m_buttonGroupColor));
        painter.drawRect(QRect(topLeft, bottomRight));

        //再画两个半圆
        painter.drawPie(QRect(m_delayFoldBtn->pos(), m_delayFoldBtn->pos() + QPoint(m_delayFoldBtn->width(), m_delayFoldBtn->height())), 0 * 16, 180 * 16);
        painter.drawPie(QRect(m_delay6SecondBtn->pos(), m_delay6SecondBtn->pos() + QPoint(m_delay6SecondBtn->width(), m_delay6SecondBtn->height())), 180 * 16, 360 * 16);
    }
}

