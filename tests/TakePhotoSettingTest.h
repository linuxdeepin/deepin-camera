#ifndef _TAKE_PHOTO_SETTING_TEST_H
#define _TAKE_PHOTO_SETTING_TEST_H

#include <gtest/gtest.h>


class CMainWindow;
class takePhotoSettingAreaWidget;

class TakePhotoSettingTest: public ::testing::Test
{
public:
    TakePhotoSettingTest();
    virtual void SetUp() override;

    virtual void TearDown() override;

protected:
    CMainWindow *mainwindow;
    takePhotoSettingAreaWidget *takePhotoSet;
};
#endif
