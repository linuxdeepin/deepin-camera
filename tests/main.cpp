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
//    testing::GTEST_FLAG(output) = "xml:./report/report_deepin-camera.xml";
    testing::InitGoogleTest();
    runtest();
    __sanitizer_set_report_path("asan.log");
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

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    QStringList cameralist;
    QString command("ffplay -f v4l2 -video_size 1920x1080 -i ");
    for (QList<QCameraInfo>::Iterator it = cameras.begin(); it != cameras.end(); ++it) {
        QString tmp = command + it->deviceName();
        cameralist.append(tmp);
    }


    QList<QProcess *> Processlist;
    Processlist.clear();
    int n = 0;
    for (QStringList::Iterator str = cameralist.begin(); str != cameralist.end(); ++str) {
        Processlist.append(new QProcess);
        Processlist.at(n)->start(*str);
        n++;
    }

    while (n > 0) {
        if (Processlist.at(n - 1)->state() == QProcess::Starting) {
            n--;
        }
    }

    CMainWindow *w = new CMainWindow;
    w->setMinimumSize(CMainWindow::minWindowWidth, CMainWindow::minWindowHeight);
    a.setMainWindow(w);
    a.setprocess(Processlist);

    w->show();
    w->loadAfterShow();
    //将界面移至屏幕中央
    Dtk::Widget::moveToCenter(w);

    QTestMain testMain;
    QTest::qExec(&testMain, argc, argv);
    return qApp->exec();
}

#include "main.moc"
