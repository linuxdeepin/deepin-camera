#ifndef QTLOCALPEERTEST_H
#define QTLOCALPEERTEST_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/qtsingleapplication/qtlocalpeer.h"

class QtlocalpeerTest : public ::testing::Test
{
public:
    QtlocalpeerTest();
    virtual void SetUp()
    {

    }

    virtual void TearDown()
    {
        delete qlp;
    }
protected:
    QtLocalPeer  *qlp;
};

#endif // QTLOCALPEERTEST_H
