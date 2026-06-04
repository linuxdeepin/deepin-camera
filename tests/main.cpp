#include "src/mainwindow.h"
#include "src/capplication.h"
#include "stub_function.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <sanitizer/asan_interface.h>
#include <QTest>
#include <dtk6/DWidget/DApplication>
#include <dtk6/DWidget/DMainWindow>
#include <dtk6/DWidget/DWidgetUtil>
#include <QSharedMemory>
#include <dtk6/DCore/DLog>
#if DTK_VERSION < DTK_VERSION_CHECK(6, 0, 0, 0)
#include <dtk6/DWidget/dapplicationsettings>
#endif
#include <QCameraDevice>
#include "SettingTest.h"
#include "src/accessibility/ac-deepin-camera-define.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    Stub_Function::init();
    Stub_Function::initSub();
    CApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    //加载翻译

    //设置属性 (Qt6中AA_UseHighDpiPixmaps已默认启用，无需手动设置)
#if QT_VERSION_MAJOR <= 5
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    qApp->setOrganizationName("deepin");
    qApp->setApplicationName("deepin-camera");
    qApp->setApplicationDisplayName("Camera");
    qApp->setProductName("Camera");
    qApp->loadTranslator(QList<QLocale>() << QLocale::system());
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    qDebug() << "LogFile:" << DLogManager::getlogFilePath();
    qApp->setApplicationVersion("1.0");
    qApp->setWindowIcon(QIcon(":/images/logo/deepin-camera-96px.svg"));
    //a.setProductIcon(QIcon::fromTheme("deepin-camera"));
    qApp->setProductIcon(QIcon(":/images/logo/deepin-camera-96px.svg")); //用于显示关于窗口的应用图标

    qApp->setApplicationDescription("This is camera.");

#if DTK_VERSION < DTK_VERSION_CHECK(6, 0, 0, 0)
    DApplicationSettings saveTheme;
#endif

    //仅允许打开一个相机，设置共享内存段
    QSharedMemory shared_memory("deepincamera");

    if (shared_memory.attach()) {
        shared_memory.detach();
    }

    if (!shared_memory.create(1)) {
        qDebug() << "another deepin camera instance has started";
        exit(0);
    }

    dc::Settings::get().init();
    InitSetting();
    CMainWindow *mainWnd = new CMainWindow();
    mainWnd->setMinimumSize(CMainWindow::minWindowWidth, CMainWindow::minWindowHeight);
    a.setMainWindow(mainWnd);
    mainWnd->setWayland(false);

    mainWnd->show();
    mainWnd->loadAfterShow();
    //将界面移至屏幕中央
    Dtk::Widget::moveToCenter(mainWnd);

    __sanitizer_set_report_path("asan.log");

    int ret = RUN_ALL_TESTS();
    mainWnd->close();
    delete mainWnd;
    Stub_Function::release();
    printf("end\n");
    return  ret;
}

