/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     tanlang <tanlang@uniontech.com>
* Maintainer: tanlang <tanlang@uniontech.com>
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

#include "TakePhotoSettingTest.h"
#include "src/capplication.h"
#include "src/takephotosettingareawidget.h"
#include "src/circlepushbutton.h"
#include "src/accessibility/ac-deepin-camera-define.h"

#include <QtTest/QTest>

using namespace Dtk::Core;

TakePhotoSettingTest::TakePhotoSettingTest(): m_takePhotoSet(nullptr)
{

}

void TakePhotoSettingTest::SetUp()
{
    m_mainwindow = CamApp->getMainWindow();
    if (!m_mainwindow->isVisible()) {
        m_mainwindow->show();
        m_mainwindow->activateWindow();
    }
    if (nullptr != m_mainwindow) {
        QTest::qWait(2000);
        m_takePhotoSet = m_mainwindow->findChild<takePhotoSettingAreaWidget *>(LEFT_BTNS_BOX);
    }
}

void TakePhotoSettingTest::TearDown()
{
     m_mainwindow = nullptr;
     m_takePhotoSet = nullptr;
}

/**
 *  @brief 键盘交互
 */
TEST_F(TakePhotoSettingTest, KeyEvent)
{
    takePhotoSettingAreaWidget *takePhotoSettingArea = m_mainwindow->findChild<takePhotoSettingAreaWidget *>(LEFT_BTNS_BOX);
    circlePushButton *unfoldBtn = m_mainwindow->findChild<circlePushButton *>(UNFOLD_BTN);
    circlePushButton *flashlightUnfoldBtn = m_mainwindow->findChild<circlePushButton *>(FLASHLITE_UNFOLD_BTN);
    circlePushButton *flashlightFoldBtn = m_mainwindow->findChild<circlePushButton *>(FLASHLITE_FOLD_BTN);
    circlePushButton *delayUnfoldBtn = m_mainwindow->findChild<circlePushButton *>(DELAY_UNFOLD_BTN);
    circlePushButton *delayFoldBtn = m_mainwindow->findChild<circlePushButton *>(DELAY_FOLD_BTN);
    circlePushButton *foldBtn = m_mainwindow->findChild<circlePushButton *>(FOLD_BTN);


    if (unfoldBtn != nullptr) {
        unfoldBtn->setFocus();
        QTest::keyClick(unfoldBtn, Qt::Key_Return, Qt::NoModifier, 200);

        for (int i = 0; i < 3; i++) {
            QTest::keyClick(takePhotoSettingArea, Qt::Key_Up, Qt::NoModifier, 200);
        }

        for (int i = 0; i < 3; i++) {
            QTest::keyClick(takePhotoSettingArea, Qt::Key_Down, Qt::NoModifier, 200);
        }
    }

    QTest::qWait(800);
    if (flashlightUnfoldBtn != nullptr) {
        flashlightUnfoldBtn->setFocus();
        QTest::keyClick(flashlightUnfoldBtn, Qt::Key_Return, Qt::NoModifier, 200);
        if (flashlightFoldBtn != nullptr) {
            QTest::keyClick(flashlightFoldBtn, Qt::Key_Return, Qt::NoModifier, 800);
        } else {
            QTest::qWait(800);
            takePhotoSettingArea->unfoldBtnClicked();
        }
    }

    QTest::qWait(800);
    if (delayUnfoldBtn != nullptr) {
        delayUnfoldBtn->setFocus();
        QTest::keyClick(delayUnfoldBtn, Qt::Key_Return, Qt::NoModifier, 200);
        if (delayFoldBtn != nullptr) {
            QTest::keyClick(delayFoldBtn, Qt::Key_Return, Qt::NoModifier, 800);
        } else {
            QTest::qWait(800);
            takePhotoSettingArea->unfoldBtnClicked();
        }
    }

    QTest::qWait(800);
    takePhotoSettingArea->foldBtnClicked();
}

/**
 *  @brief takePhotoSettingAreaWidget展开按钮点击
 */
TEST_F(TakePhotoSettingTest, unfoldBtnClicked)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->unfoldBtnClicked();
}
/**
 *  @brief takePhotoSettingAreaWidget展开闪光按钮点击
 */
TEST_F(TakePhotoSettingTest, flashlightUnfoldBtnClicked)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->flashlightUnfoldBtnClicked();

}
/**
 *  @brief takePhotoSettingAreaWidget设置闪光开
 */
TEST_F(TakePhotoSettingTest, setFlashlightTrue)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->setFlashlight(true);
}
/**
 *  @brief takePhotoSettingAreaWidget设置闪光关
 */
TEST_F(TakePhotoSettingTest, setFlashlightFalse)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->setFlashlight(false);
}
/**
 *  @brief takePhotoSettingAreaWidget折叠闪光
 */
TEST_F(TakePhotoSettingTest, flashlightFoldBtnClicked)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->flashlightFoldBtnClicked();

}
/**
 *  @brief takePhotoSettingAreaWidget展开延时设置
 */
TEST_F(TakePhotoSettingTest, delayUnfoldBtnClicked)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->delayUnfoldBtnClicked();
}
/**
 *  @brief takePhotoSettingAreaWidget设置延时3秒
 */
TEST_F(TakePhotoSettingTest, setDelayTime3)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->setDelayTime(3);
}
/**
 *  @brief takePhotoSettingAreaWidget设置延时6秒
 */
TEST_F(TakePhotoSettingTest, setDelayTime6)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->setDelayTime(6);

}
/**
 *  @brief takePhotoSettingAreaWidget设置延时0秒
 */
TEST_F(TakePhotoSettingTest, setDelayTime0)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->setDelayTime(0);

}
/**
 *  @brief Settings设置延时
 */
TEST_F(TakePhotoSettingTest, setDelay3)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    QTest::qWait(500);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 2);
    QTest::qWait(500);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    QTest::qWait(500);
}

/**
 *  @brief takePhotoSettingAreaWidget折叠延时
 */
TEST_F(TakePhotoSettingTest, delayfoldBtnClicked)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->delayfoldBtnClicked();
}
/**
 *  @brief takePhotoSettingAreaWidget折叠设置
 */
TEST_F(TakePhotoSettingTest, foldBtnClicked)
{
    if (nullptr != m_takePhotoSet)
        m_takePhotoSet->foldBtnClicked();
}

TEST_F(TakePhotoSettingTest, CircleBtnTest)
{
    circlePushButton* btn = m_takePhotoSet->findChild<circlePushButton *>(UNFOLD_BTN);
    btn->setDisableSelect(true);
    btn->setSelected(true);
    btn->setSelected(false);
    QTest::mouseMove(btn, QPoint(5, 5), 500);
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::mousePress(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(500);
    btn->setDisableSelect(false);
    btn->setSelected(true);
    btn->setSelected(false);
    QTest::mouseMove(btn, QPoint(5, 5), 500);
    QTest::mouseClick(btn, Qt::LeftButton);
    QTest::mousePress(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(500);
}

