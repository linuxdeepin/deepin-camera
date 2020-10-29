#ifndef CLOSEDIALOGTEST_H
#define CLOSEDIALOGTEST_H
#include "../src/src/closedialog.h"
#include <gtest/gtest.h>
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

class CloseDialogTest: public ::testing::Test
{
public:
    CloseDialogTest();
protected:
    CloseDialog  *closedialog;
};

#endif // CLOSEDIALOGTEST_H
