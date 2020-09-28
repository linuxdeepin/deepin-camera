#ifndef DEVMONTEST_H
#define DEVMONTEST_H

#include <gtest/gtest.h>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/src/devnummonitor.h"

class DevmonitorTest : public ::testing::Test
{
public:
    DevmonitorTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {
    }
protected:
    DevNumMonitor  *devmon;
};

#endif // DEVMONTEST_H
