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
        takePhotoSet = mainwindow->findChild<takePhotoSettingAreaWidget *>(RIGHT_BTNS_BOX);
    }
}

void TakePhotoSettingTest::SetUp()
{

}

void TakePhotoSettingTest::TearDown()
{

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


