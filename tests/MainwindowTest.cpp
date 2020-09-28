#include "MainwindowTest.h"

MainwindowTest::MainwindowTest()
{
        mw = new CMainWindow();
}

TEST_F(MainwindowTest, mwshow)
{
    mw->show();
    mw->hide();
}
TEST_F(MainwindowTest, lastopenpath)
{
    ASSERT_TRUE(mw->lastOpenedPath() != "");
}
TEST_F(MainwindowTest, setWindowState1)
{
    mw->setWindowState(Qt::WindowMinimized);
    mw->showMinimized();
}

TEST_F(MainwindowTest, setWindowState2)
{
    mw->setWindowState(Qt::WindowMaximized);
    mw->setWindowState(Qt::WindowMinimized);
    mw->showMaximized();
}
