#ifndef IMAGETEST_H
#define IMAGETEST_H

#include "../src/src/imageitem.h"
#include <gtest/gtest.h>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

DWIDGET_USE_NAMESPACE

class ImageTest : public ::testing::Test
{
public:
    ImageTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {
        delete ImageItemtest;
    }
protected:
    ImageItem  *ImageItemtest;
};
#endif // IMAGETEST_H
