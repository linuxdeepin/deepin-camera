#ifndef THUMBNAILTEST_H
#define THUMBNAILTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/src/thumbnailsbar.h"

DWIDGET_USE_NAMESPACE

class ThumbnailTest : public ::testing::Test
{
public:
    ThumbnailTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {
        delete thumbar;
    }
protected:
    ThumbnailsBar  *thumbar;
};

#endif // THUMBNAILTEST_H
