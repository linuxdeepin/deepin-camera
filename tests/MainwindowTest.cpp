#include <QComboBox>
#include <QAction>
#include <QProcess>
#include <DWindowMaxButton>
#include <DWindowMinButton>
#include <DWindowCloseButton>

#include "MainwindowTest.h"
#include "thumbnailTest.h"
#include "imageTest.h"
#include "DButtonBox"
#include "datamanager.h"
#include "printhelper.h"


using namespace Dtk::Core;
MainwindowTest::MainwindowTest()
{
    if(!mainwindow)
        mainwindow = dApp->getMainWindow();
    if(!mainwindow->isVisible())
    {
        mainwindow->show();
        mainwindow->activateWindow();
    }
    QTest::qWait(2000);
}

/**
 *  @brief 摄像头被占用
 */
TEST_F(MainwindowTest, CamUsed)
{
    QList<QProcess*> proclist = dApp->getprocess();

    QProcess* proc1 = proclist[0];
    QProcess* proc2 = proclist[1];
    proc1->start("ffplay -f v4l2 -video_size 1920x1080 -i /dev/video2");
    proc2->start("ffplay -f v4l2 -video_size 1920x1080 -i /dev/video0");
    DIconButton* selectBtn = mainwindow->findChild<DIconButton*>("SelectBtn");
    int n = 0;
    while(n<5){
        if(proc1->state() == QProcess::NotRunning)
        {
            proc1->start("ffplay -f v4l2 -video_size 1920x1080 -i /dev/video2");
        }
        if(proc2->state() == QProcess::NotRunning)
        {
            proc2->start("ffplay -f v4l2 -video_size 1920x1080 -i /dev/video0");
        }
        if(selectBtn->isVisible())
        {
            QTest::mouseClick(selectBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
        }
        QTest::qWait(500);
        n++;
    }
    if(proc1)
    {
        delete proc1;
        proc1 = nullptr;
    }
    if(proc2)
    {
        delete proc2;
        proc2 = nullptr;
    }
}

/**
 *  @brief 测试拍照
 */
TEST_F(MainwindowTest, TakePicture)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitlePicBtn");
    QTest::mouseMove(iconpixBtn,QPoint(0,0),1000);
    QTest::mousePress(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

    QTest::qWait(1000);

}

/**
 *  @brief 切换摄像头
 */
TEST_F(MainwindowTest, ChangeCamera)
{
    DIconButton* selectBtn = mainwindow->findChild<DIconButton*>("SelectBtn");

    if(selectBtn->isVisible())
    {
        QTest::mouseClick(selectBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
    }
    QTest::qWait(2000);
}

/**
 *  @brief 三连拍
 */
TEST_F(MainwindowTest, ThreeContinuousShooting)
{
    QString str = QDir::homePath();
    QString picture = str+QString("/Music/");

    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 1);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().setPathOption("picdatapath",QVariant("picture"));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitlePicBtn");
    QTest::mouseMove(iconpixBtn,QPoint(0,0),1000);
    QTest::mousePress(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

    QTest::qWait(3000);

}

/**
 *  @brief 删除快捷键
 */
TEST_F(MainwindowTest, DelShortCut)
{
    videowidget* prevideo = mainwindow->findChild<videowidget*>("VideoPreviewWidget");

    //点击鼠标右键
    QTest::qWait(1000);
    QTest::mouseMove(prevideo,QPoint(0,0),1000);
    for(int i = 0; i < 3; i++)
    {
        QTest::keyClick(prevideo, Qt::Key_Delete, Qt::NoModifier,0);
        QTest::qWait(500);
    }
}

/**
 *  @brief 十连拍
 */
TEST_F(MainwindowTest, TenContinuousShooting)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 2);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitlePicBtn");
    QTest::mouseMove(iconpixBtn,QPoint(0,0),1000);
    QTest::mousePress(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

    QTest::qWait(10000);

}

/**
 *  @brief 连拍取消
 */
TEST_F(MainwindowTest, ContinuousShootingCancel)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 2);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitlePicBtn");
    QTest::mouseMove(iconpixBtn,QPoint(0,0),1000);
    QTest::mousePress(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    QTest::mouseRelease(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    QTest::qWait(300);
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    QTest::qWait(10000);

}

/**
 *  @brief 延迟拍照
 */
TEST_F(MainwindowTest, TakePicDelay)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitlePicBtn");
    QTest::mouseMove(iconpixBtn,QPoint(0,0),1000);
    QTest::mousePress(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconpixBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

    QTest::qWait(10000);

}

/**
 *  @brief 最大化
 */
TEST_F(MainwindowTest, MaxMinWindow)
{
    DWindowMaxButton* WindowMaxBtnOpt = mainwindow->findChild<DWindowMaxButton*>("DTitlebarDWindowMaxButton");
    QTest::mouseClick(WindowMaxBtnOpt, Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
    QTest::qWait(1000);

    QTest::mouseClick(WindowMaxBtnOpt, Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
    QTest::qWait(1000);
}

/**
 *  @brief 右键菜单
 */
TEST_F(MainwindowTest, rightbtn)
{

//    QTest::qWait(1000);
    QMap<int, ImageItem *> it = get_imageitem();
    ImageItem* imgit;
    imgit = it.value(0);

    //点击鼠标右键
    QTest::qWait(1000);
    QTest::mouseMove(imgit,QPoint(0,0),1000);
    QMenu* rightMouseMenu = mainwindow->findChild<QMenu*>("rightbtnmenu");
    rightMouseMenu->show();
    QTest::qWait(2000);
    rightMouseMenu->hide();
}


/**
 *  @brief 缩略图多选和复制，删除
 */
TEST_F(MainwindowTest, ImageItemContinuousChoose)
{
    QTest::keyPress(mainwindow,Qt::Key_Shift,Qt::NoModifier,500);
    QMap<int, ImageItem *> ImageMap = get_imageitem();
    QList<ImageItem*> ImageList;
    ImageList.clear();
    int number = ImageMap.count();
    if(number > 0)
    {
        ImageList = ImageMap.values();
        if(ImageList.count() > 3)
        {
            for (int i = 1; i < 3; i++) {
                QTest::mouseMove(ImageList[i],QPoint(0,0),1000);
                QTest::mouseClick(ImageList[i], Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
                QTest::qWait(1000);
            }

        }
        else {
            for (int i = 1; i < ImageList.count(); i++) {
                QTest::mouseMove(ImageList[i],QPoint(0,0),1000);
                QTest::mouseClick(ImageList[i], Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
                QTest::qWait(1000);
            }
        }
        QAction* copyact = mainwindow->findChild<QAction*>("CopyAction");
        copyact->trigger();

        QAction* delact = mainwindow->findChild<QAction*>("DelAction");
        delact->trigger();
        QTest::keyRelease(mainwindow,Qt::Key_Shift,Qt::NoModifier,500);
    }
}

/**
 *  @brief 打印窗口
 */
TEST_F(MainwindowTest, printdialog)
{
    QAction* print = mainwindow->findChild<QAction*>("PrinterAction");
    print->trigger();
    QTest::qWait(1000);
}

/**
 *  @brief 录像
 */
TEST_F(MainwindowTest, TakeVideo1)
{

    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconVdBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitleVdBtn");

    QTest::mouseMove(iconVdBtn,QPoint(0,0),1000);
    QTest::mousePress(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

    QTest::qWait(2000);
    DWindowMinButton* windowMinBtn = mainwindow->findChild<DWindowMinButton*>("DTitlebarDWindowMinButton");
    QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
    QTest::qWait(3000);
    if (mainwindow->isMinimized())
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();

    QTest::qWait(4400);

    EndBtn = mainwindow->findChild<DPushButton*>("TakeVdEndBtn");

    QTest::mouseMove(EndBtn,QPoint(0,0),1000);
    QTest::mousePress(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

}


/**
 *  @brief 延迟录像
 */
TEST_F(MainwindowTest, TakeVideoDelay)
{
    QString str = QDir::homePath();
    QString video = str+QString("/Music/");
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
        dc::Settings::get().setPathOption("vddatapath",QVariant("video"));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconVdBtn =  mainwindow->findChild<DButtonBoxButton*>("pTitleVdBtn");

    QTest::mouseMove(iconVdBtn,QPoint(0,0),1000);
    QTest::mousePress(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

    PixVdBtn = mainwindow->findChild<QPushButton*>("PicVdBtn");
    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

    QTest::qWait(10000);

    EndBtn = mainwindow->findChild<DPushButton*>("TakeVdEndBtn");

    QTest::mouseMove(EndBtn,QPoint(0,0),1000);
    QTest::mousePress(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

}

/**
 *  @brief 设置页面
 */
TEST_F(MainwindowTest, SettingDialogShow)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);

    dc::Settings::get().settings()->sync();
    DSettingsDialog* dialog = mainwindow->findChild<DSettingsDialog* >("SettingDialog");
    dialog->show();
    QTest::qWait(2000);
    dialog->hide();
    mainwindow->settingDialogDel();
}



TEST_F(MainwindowTest, ImageItemDel)
{

//    QTest::qWait(1000);
    QMap<int, ImageItem *> it = get_imageitem();
    ImageItem* imgit;
    imgit = it.value(0);

    //点击鼠标右键
    QTest::qWait(1000);
    QTest::mouseMove(imgit,QPoint(0,0),1000);
    QTest::mousePress(imgit,Qt::RightButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(imgit,Qt::RightButton,Qt::NoModifier,QPoint(0,0),0);

    //点击鼠标左键
    QTest::qWait(1000);
    QTest::mouseMove(imgit,QPoint(0,0),1000);
    QTest::mousePress(imgit,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
    QTest::mouseRelease(imgit,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);



    if(!it.isEmpty()){
        imgit = it.value(0);
        QTest::keyPress(imgit, Qt::Key_C, Qt::ControlModifier,500);
        QTest::keyRelease(imgit, Qt::Key_C, Qt::ControlModifier,500);
        QTest::keyClick(imgit, Qt::Key_Delete, Qt::NoModifier,0);
    }
}

/**
 *  @brief 切换分辨率
 */
TEST_F(MainwindowTest, ChangeResolution)
{
    mainwindow->settingDialog();
    DSettingsDialog* dialog = mainwindow->findChild<DSettingsDialog* >("SettingDialog");
    dialog->show();

    QComboBox* cmbox = dialog->findChild<QComboBox*>("OptionLineEdit");
    mainwindow->update();
    cmbox->setCurrentIndex(1);
    dc::Settings::get().settings()->sync();

    mainwindow->settingDialogDel();
}
/**
 *  @brief 无摄像头
 */
TEST_F(MainwindowTest, NoCam)
{
    DIconButton* selectBtn = mainwindow->findChild<DIconButton*>("SelectBtn");

    system("echo '123'|sudo -S rm -rf /dev/video*");
    QTest::qWait(2000);
    if(selectBtn->isVisible())
    {
        QTest::mouseClick(selectBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0,0), 200);
    }
    QTest::qWait(2000);
}

/**
 *  @brief 主窗口退出(这个case一定得放在最后)
 */
TEST_F(MainwindowTest, quitmainwindow)
{
    delete mainwindow;
    mainwindow=nullptr;

}
