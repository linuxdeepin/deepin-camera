#ifndef SETTINGTEST_H
#define SETTINGTEST_H


#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/src/Settings.h"

using namespace dc;

class SettingTest : public ::testing::Test
{
public:
    SettingTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {
    }
protected:

};

#endif // SETTINGTEST_H
