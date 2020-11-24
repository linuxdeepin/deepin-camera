#ifndef VIDEOWIDGETTEST_H
#define VIDEOWIDGETTEST_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../basepub/printhelper.h"

class PrintHelperTest : public ::testing::Test
{
public:
    PrintHelperTest();
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
protected:
    PrintHelper  *printhelper;
};

#endif // VIDEOWIDGETTEST_H
