#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <QtTest/QTest>
#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <QSharedMemory>
#include <DLog>
#include <DApplicationSettings>
#include "../src/src/mainwindow.h"
#include "../src/src/capplication.h"

DWIDGET_USE_NAMESPACE

class QTestMain : public QObject
{
    Q_OBJECT

public:
    QTestMain();
    ~QTestMain();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testGTest();
    int runtest();
};

QTestMain::QTestMain()
{

}

QTestMain::~QTestMain()
{

}

void QTestMain::initTestCase()
{
    qDebug() << "=====start test=====";
}

void QTestMain::cleanupTestCase()
{
    qDebug() << "=====stop test=====";
}

void QTestMain::testGTest()
{
    testing::InitGoogleTest();
    runtest();
    exit(0);
}

int QTestMain::runtest()
{
    return RUN_ALL_TESTS();
}

int main(int argc, char *argv[])
{

    CApplication a(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    //加载翻译


//    QTranslator *translator = new QTranslator;

//    bool bLoaded = translator->load("deepin-camera.qm", ":/translations");
//    if (!bLoaded) {
//        qDebug() << "load transfile error";
//    }

//    a.installTranslator(translator);
    //设置属性
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);

    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-camera");
    a.setApplicationDisplayName("Camera");
    a.setProductName("Camera");
    a.loadTranslator(QList<QLocale>() << QLocale::system());
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    qDebug() << "LogFile:" << DLogManager::getlogFilePath();
    a.setApplicationVersion("1.0");
    a.setWindowIcon(QIcon(":/images/logo/deepin-camera-96px.svg"));
    //a.setProductIcon(QIcon::fromTheme("deepin-camera"));
    a.setProductIcon(QIcon(":/images/logo/deepin-camera-96px.svg")); //用于显示关于窗口的应用图标

    a.setApplicationDescription("This is camera.");

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
    QList<QProcess*> Processlist;
    Processlist.clear();
    QProcess* myProcess = new QProcess;

    QProcess* myProcess1 = new QProcess;
//    myProcess1->start("ffplay -f v4l2 -video_size 1920x1080 -i /dev/video0");

    QTest::qWait(3000);

    Processlist.append(myProcess);
    Processlist.append(myProcess1);

    CMainWindow* w = new CMainWindow;
    w->setMinimumSize(MinWindowWidth, MinWindowHeight);

    a.setMainWindow(w);
    a.setprocess(Processlist);

    w->show();
    w->loadAfterShow();
    //将界面移至屏幕中央
    Dtk::Widget::moveToCenter(w);

    QTestMain testMain;
    QTest::qExec(&testMain, argc, argv);
    return a.exec();
}

#include "main.moc"
