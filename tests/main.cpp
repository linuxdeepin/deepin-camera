#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <sanitizer/asan_interface.h>
#include <QtTest/QTest>
#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <QSharedMemory>
#include <DLog>
#include <DApplicationSettings>
#include <QCameraInfo>
#include "src/mainwindow.h"
#include "src/capplication.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{

    CApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    //加载翻译

    //设置属性
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

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

    DApplicationSettings saveTheme;

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

    CMainWindow *w = new CMainWindow;
    w->setMinimumSize(CMainWindow::minWindowWidth, CMainWindow::minWindowHeight);
    a.setMainWindow(w);

    w->show();
    w->loadAfterShow();
    //将界面移至屏幕中央
    Dtk::Widget::moveToCenter(w);

    __sanitizer_set_report_path("asan.log");

    return RUN_ALL_TESTS();
}

