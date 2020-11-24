#include <QComboBox>
#include <QAction>
#include <DWindowMaxButton>
#include <DWindowMinButton>

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
    }
    QTest::qWait(2000);
}



/**
 *  @brief 设备监控线程
 */
TEST_F(MainwindowTest, devMoniThread)
{
//    v4l2_device_list_t* my_device_list = get_device_list();
//    my_device_list->num_devices = 0;
//    DataManager::instance()->setdevStatus()

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
 *  @brief 关闭主窗口
 */
TEST_F(MainwindowTest, CloseMainwindow)
{
//    mainwindow->settingDialog();
//    DSettingsDialog* dialog = mainwindow->findChild<DSettingsDialog* >("SettingDialog");
//    dialog->show();

//    QComboBox* cmbox = dialog->findChild<QComboBox*>("OptionLineEdit");
//    mainwindow->update();
//    cmbox->setCurrentIndex(1);
//    dc::Settings::get().settings()->sync();

//    mainwindow->settingDialogDel();
}


TEST_F(MainwindowTest, mainwindow)
{
//    //分辨率
//    //设置主题
//    //等待新连接相机设备,期间插入USB
//    //相机切换
//    mainwindow->settingDialog();
//    DSettingsDialog* settingdlg = mainwindow->findChild<DSettingsDialog*>("SettingDialog");
//    settingdlg->show();
//    dc::Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
//    mainwindow->settingDialogDel();


//    if(selectBtn->isVisible() == true)
//    {
//        QTest::mouseMove(selectBtn,QPoint(0,0),1000);
//        QTest::mousePress(selectBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//        QTest::mouseRelease(selectBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
//    }

////    QTest::qWait(2000);
//    //切换到拍照按钮


//    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 1);
//    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
//    dc::Settings::get().settings()->sync();



//    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
//    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);

////    QTest::qWait(1000);
//    QMap<int, ImageItem *> it = get_imageitem();
//    ImageItem* imgit;
//    imgit = it.value(0);
//    qDebug()<<"**********************************"<<it.isEmpty()<<"****************************************";
//    //键盘按下shift
////    QTest::qWait(1000);
//    QTest::keyPress(imgit, Qt::Key_Shift, Qt::NoModifier,500);
//    QTest::keyRelease(imgit, Qt::Key_Shift, Qt::NoModifier,500);
//    //点击鼠标左键
//    QTest::qWait(1000);
//    QTest::mouseMove(imgit,QPoint(0,0),1000);
//    QTest::mousePress(imgit,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(imgit,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    //点击鼠标右键
//    QTest::qWait(1000);
//    QTest::mouseMove(imgit,QPoint(0,0),1000);
//    QTest::mousePress(imgit,Qt::RightButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(imgit,Qt::RightButton,Qt::NoModifier,QPoint(0,0),0);

////    QTest::qWait(1000);
//    qDebug()<<"***************size:"<<it.size()<<"*************count:"<<it.count()<<"****************"<<__LINE__;
//    if(!it.isEmpty()){
//        imgit = it.value(0);
//        QTest::keyPress(imgit, Qt::Key_C, Qt::ControlModifier,500);
//        QTest::keyRelease(imgit, Qt::Key_C, Qt::ControlModifier,500);
//        QTest::keyClick(imgit, Qt::Key_Delete, Qt::NoModifier,0);
//    }
////    QTest::qWait(2000);

//    while(iconVdBtn->isEnabled() == false);
//    //切换到录像按钮
//    QTest::mouseMove(iconVdBtn,QPoint(0,0),1000);
//    QTest::mousePress(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(iconVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);

//    //拍照，录像，取消录像同一个按钮，此时为录像

//    QTest::mouseMove(PixVdBtn,QPoint(0,0),1000);
//    QTest::mousePress(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(PixVdBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
//    //取消录像

//    DPushButton* EndBtn = mainwindow->getEndBtn();
//    QTest::mouseMove(EndBtn,QPoint(0,0),1000);
//    QTest::mousePress(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),500);
//    QTest::mouseRelease(EndBtn,Qt::LeftButton,Qt::NoModifier,QPoint(0,0),0);
//    //关闭窗口
//    QTest::qWait(1000);
}


