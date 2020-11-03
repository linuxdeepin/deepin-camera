#ifndef MAINWINDOWTEST_H
#define MAINWINDOWTEST_H

//#include <QTest>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QtTest/QTest>
#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <QSharedMemory>
#include <DLog>
#include <DApplicationSettings>
#include "../src/src/mainwindow.h"
#include "../src/src/capplication.h"
#include "../src/src/Settings.h"
#include "../src/src/videowidget.h"
#include "../src/src/imageitem.h"
#include "../src/src/Settings.h"
#include "../libcam/libcam_v4l2core/v4l2_devices.h"

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

};

#endif // MAINWINDOWTEST_H
