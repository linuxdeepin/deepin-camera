// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QTimer>
#include <QtTest/QTest>

#include "ClosedialogTest.h"

CloseDialogTest::CloseDialogTest()
{


}

/**
 *  @brief 打开关闭页面
 */
TEST_F(CloseDialogTest, dialogshow)
{
    CloseDialog *closedialog = new CloseDialog;
    closedialog->show();
    QTest::qWait(1000);
    delete closedialog;
    closedialog = nullptr;
}
