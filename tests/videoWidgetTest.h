#ifndef VIDEOWIDGETTEST_H
#define VIDEOWIDGETTEST_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/src/videowidget.h"

class VideowidgetTest : public ::testing::Test
{
public:
    VideowidgetTest();
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
protected:
    videowidget  *videowgt;
};

#endif // VIDEOWIDGETTEST_H
