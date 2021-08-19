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

TakePhotoSettingTest::TakePhotoSettingTest(): takePhotoSet(nullptr)
{
    mainwindow = CamApp->getMainWindow();
    if (!mainwindow->isVisible()) {
        mainwindow->show();
        mainwindow->activateWindow();
    }
    if (nullptr != mainwindow) {
        QTest::qWait(2000);
        takePhotoSet = mainwindow->findChild<takePhotoSettingAreaWidget *>(LEFT_BTNS_BOX);
    }
}

void TakePhotoSettingTest::SetUp()
{

}

void TakePhotoSettingTest::TearDown()
{

}

/**
 *  @brief 键盘交互
 */
TEST_F(TakePhotoSettingTest, KeyEvent)
{
    takePhotoSettingAreaWidget *takePhotoSettingArea = mainwindow->findChild<takePhotoSettingAreaWidget *>(LEFT_BTNS_BOX);
    circlePushButton *unfoldBtn = mainwindow->findChild<circlePushButton *>(UNFOLD_BTN);
    circlePushButton *flashlightUnfoldBtn = mainwindow->findChild<circlePushButton *>(FLASHLITE_UNFOLD_BTN);
    circlePushButton *flashlightFoldBtn = mainwindow->findChild<circlePushButton *>(FLASHLITE_FOLD_BTN);
    circlePushButton *delayUnfoldBtn = mainwindow->findChild<circlePushButton *>(DELAY_UNFOLD_BTN);
    circlePushButton *delayFoldBtn = mainwindow->findChild<circlePushButton *>(DELAY_FOLD_BTN);
    circlePushButton *foldBtn = mainwindow->findChild<circlePushButton *>(FOLD_BTN);


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
    if (nullptr != takePhotoSet)
        takePhotoSet->unfoldBtnClicked();
}
/**
 *  @brief takePhotoSettingAreaWidget展开闪光按钮点击
 */
TEST_F(TakePhotoSettingTest, flashlightUnfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->flashlightUnfoldBtnClicked();

}
/**
 *  @brief takePhotoSettingAreaWidget设置闪光开
 */
TEST_F(TakePhotoSettingTest, setFlashlightTrue)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setFlashlight(true);
}
/**
 *  @brief takePhotoSettingAreaWidget设置闪光关
 */
TEST_F(TakePhotoSettingTest, setFlashlightFalse)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setFlashlight(false);
}
/**
 *  @brief takePhotoSettingAreaWidget折叠闪光
 */
TEST_F(TakePhotoSettingTest, flashlightFoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->flashlightFoldBtnClicked();

}
/**
 *  @brief takePhotoSettingAreaWidget展开延时设置
 */
TEST_F(TakePhotoSettingTest, delayUnfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->delayUnfoldBtnClicked();
}
/**
 *  @brief takePhotoSettingAreaWidget设置延时3秒
 */
TEST_F(TakePhotoSettingTest, setDelayTime3)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setDelayTime(3);
}
/**
 *  @brief takePhotoSettingAreaWidget设置延时6秒
 */
TEST_F(TakePhotoSettingTest, setDelayTime6)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setDelayTime(6);

}
/**
 *  @brief takePhotoSettingAreaWidget设置延时0秒
 */
TEST_F(TakePhotoSettingTest, setDelayTime0)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setDelayTime(0);

}
/**
 *  @brief Settings设置延时3秒
 */
TEST_F(TakePhotoSettingTest, setDelay3)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
}
/**
 *  @brief Settings设置延时6秒
 */
TEST_F(TakePhotoSettingTest, setDelay6)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 2);
}
/**
 *  @brief Settings设置延时0秒
 */
TEST_F(TakePhotoSettingTest, setDelay0)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
}
/**
 *  @brief takePhotoSettingAreaWidget折叠延时
 */
TEST_F(TakePhotoSettingTest, delayfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->delayfoldBtnClicked();
}
/**
 *  @brief takePhotoSettingAreaWidget折叠设置
 */
TEST_F(TakePhotoSettingTest, foldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->foldBtnClicked();
}

TEST_F(TakePhotoSettingTest, CircleBtnTest)
{
    circlePushButton* btn = takePhotoSet->findChild<circlePushButton *>(UNFOLD_BTN);
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

