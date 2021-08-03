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


TEST_F(TakePhotoSettingTest, unfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->unfoldBtnClicked();
}

TEST_F(TakePhotoSettingTest, flashlightUnfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->flashlightUnfoldBtnClicked();

}

TEST_F(TakePhotoSettingTest, setFlashlightTrue)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setFlashlight(true);
}

TEST_F(TakePhotoSettingTest, setFlashlightFalse)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setFlashlight(false);
}

TEST_F(TakePhotoSettingTest, flashlightFoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->flashlightFoldBtnClicked();

}

TEST_F(TakePhotoSettingTest, delayUnfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->delayUnfoldBtnClicked();
}

TEST_F(TakePhotoSettingTest, setDelayTime3)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setDelayTime(3);
}

TEST_F(TakePhotoSettingTest, setDelayTime6)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setDelayTime(6);

}

TEST_F(TakePhotoSettingTest, setDelayTime0)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->setDelayTime(0);

}

TEST_F(TakePhotoSettingTest, setDelay3)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
}

TEST_F(TakePhotoSettingTest, setDelay6)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 2);
}

TEST_F(TakePhotoSettingTest, setDelay0)
{
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
}

TEST_F(TakePhotoSettingTest, delayfoldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->delayfoldBtnClicked();
}

TEST_F(TakePhotoSettingTest, foldBtnClicked)
{
    if (nullptr != takePhotoSet)
        takePhotoSet->foldBtnClicked();
}


