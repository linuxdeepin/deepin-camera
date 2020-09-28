#ifndef MAJORTHREADTEST_H
#define MAJORTHREADTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/src/majorimageprocessingthread.h"
#include "../src/src/LPF_V4L2.h"

class MajorthreadTest : public ::testing::Test
{
public:
    MajorthreadTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {
        delete mThread;
    }
protected:
    MajorImageProcessingThread  *mThread;
};

#endif // MAJORTHREADTEST_H
