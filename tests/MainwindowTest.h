#ifndef MAINWINDOWTEST_H
#define MAINWINDOWTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QtTest/QTest>
#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <QSharedMemory>
#include <DWindowOptionButton>
#include <DMenu>
#include <DLog>
#include <DApplicationSettings>
#include "../src/src/mainwindow.h"
#include "../src/src/capplication.h"
#include "../src/src/Settings.h"
#include "../src/src/videowidget.h"
#include "../src/src/imageitem.h"
#include "../src/src/Settings.h"
#include "../src/src/thumbnailsbar.h"
#include "../libcam/libcam_v4l2core/v4l2_devices.h"
#include "../src/src/LPF_V4L2.h"
#include "../src/src/majorimageprocessingthread.h"

DWIDGET_USE_NAMESPACE

class MainwindowTest : public ::testing::Test
{
public:
    MainwindowTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {

    }

protected:
    CMainWindow* mainwindow = nullptr;
    DIconButton* selectBtn = nullptr;//切换按钮
    DButtonBoxButton* iconpixBtn = nullptr;//标题栏拍照按钮
    DButtonBoxButton* iconVdBtn = nullptr;//标题栏录像按钮
    DSettingsDialog* settingdlg = nullptr;//设置界面
    DPushButton* PixVdBtn = nullptr;//缩略图按钮
    DPushButton* EndBtn = nullptr;//录制结束按钮
    ThumbnailsBar* thumbnails = nullptr;


};

#endif // MAINWINDOWTEST_H
