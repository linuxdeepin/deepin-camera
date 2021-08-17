//#define private public
//#define protected public
#include "src/mainwindow.h"
//#undef private
//#undef protected
#include "src/imageitem.h"

#include <QComboBox>
#include <QAction>
#include <QProcess>
#include <QShortcut>
#include <QMap>
#include <QVariantMap>
#include <QStandardPaths>

#include <DWindowMaxButton>
#include <DWindowMinButton>
#include <DWindowCloseButton>
#include <DPrintPreviewDialog>

#include "MainwindowTest.h"
#include "DButtonBox"
#include "datamanager.h"
#include "ac-deepin-camera-define.h"
#include "stub_function.h"
#include "stub.h"
#include "addr_pri.h"

using namespace Dtk::Core;



ACCESS_PRIVATE_FUN(CMainWindow, void(const QString &serviceName, QVariantMap key2value, QStringList), onTimeoutLock);
ACCESS_PRIVATE_FUN(CMainWindow, void(), setSelBtnShow);
ACCESS_PRIVATE_FUN(CMainWindow, void(bool bTrue), stopCancelContinuousRecording);
ACCESS_PRIVATE_FUN(CMainWindow, void(const QString &), onDirectoryChanged);
ACCESS_PRIVATE_FUN(CMainWindow, void(int nType), onEnableTitleBar);

ACCESS_PRIVATE_FUN(CApplication, void(), QuitAction);



ACCESS_PRIVATE_FIELD(MajorImageProcessingThread, QAtomicInt, m_stopped);

//ACCESS_PRIVATE_FIELD(ThumbWidget, bool, m_tabFocusStatus);

ACCESS_PRIVATE_FIELD(CMainWindow, ActType, m_nActTpye);
//ACCESS_PRIVATE_FIELD(CMainWindow, DPushButton*, m_switchPhotoBtn);
//ACCESS_PRIVATE_FIELD(CMainWindow, DPushButton*, m_switchRecordBtn);

ACCESS_PRIVATE_FIELD(CMainWindow, videowidget *, m_videoPre);

ZMainwindowTest::ZMainwindowTest()
{
    if (!mainwindow)
        mainwindow = CamApp->getMainWindow();

    if (!mainwindow->isVisible()) {
        mainwindow->show();
        mainwindow->activateWindow();
    }

    QTest::qWait(2000);
}

ZMainwindowTest::~ZMainwindowTest()
{
}

/**
 *  @brief 摄像头被占用
 */
TEST_F(ZMainwindowTest, CamUsed)
{
    QList<QProcess *> proclist = CamApp->getprocess();

    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    QTest::qWait(500);
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    int n = proclist.count();

    while (n) {
        if (proclist.at(n - 1)->state() == QProcess::Running) {
            proclist.at(n - 1)->start();
            n--;
        }
    }

    for (int i = 0; i < proclist.count(); i++) {
        proclist.at(i)->close();
    }

    proclist.clear();
}




/**
 *  @brief Tab键切换
 */
TEST_F(ZMainwindowTest, TabChange)
{
    //tab键切换次数
    const int TabNumber = 10;
    //预览界面循环切换tab键
    for (int i = 0 ; i < TabNumber; i++) {
        QTest::keyPress(mainwindow, Qt::Key_Tab, Qt::NoModifier, 500);
    }
}

/**
 *  @brief 右键菜单
 */
TEST_F(ZMainwindowTest, RightMenushortCut)
{
    //点击鼠标右键
    QTest::qWait(500);
    QShortcut *shortcutCopy = mainwindow->findChild<QShortcut *>(SHORTCUT_COPY);
    QShortcut *shortcutDel = mainwindow->findChild<QShortcut *>(SHORTCUT_DELETE);
    QShortcut *shortcutMenu = mainwindow->findChild<QShortcut *>(SHORTCUT_CALLMENU);
    QShortcut *shortcutOpenFolder = mainwindow->findChild<QShortcut *>(SHORTCUT_OPENFOLDER);
    QShortcut *shortcutPrint = mainwindow->findChild<QShortcut *>(SHORTCUT_PRINT);

    emit shortcutCopy->activated();
    emit shortcutDel->activated();
    emit shortcutMenu->activated();
    emit shortcutOpenFolder->activated();
    emit shortcutPrint->activated();

}

/**
 *  @brief 快捷键
 */
/*TEST_F(ZMainwindowTest, shortcut)
{
    QShortcut *ShortcutView =  mainwindow->findChild<QShortcut *>(SHORTCUT_VIEW);
    QShortcut *ShortcutSpace = mainwindow->findChild<QShortcut *>(SHORTCUT_SPACE);
    emit ShortcutView->activated();
    emit ShortcutSpace->activated();
}*/

/**
 *  @brief 主题切换
 */
TEST_F(ZMainwindowTest, Themechange)
{
//    Stub stub;
//    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    QTest::qWait(500);
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);

//    iconVdBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);

//    QTest::mouseMove(iconVdBtn, QPoint(0, 0), 500);
//    QTest::mousePress(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    QTest::mouseRelease(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
//    QTest::qWait(500);

//    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
//    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 500);
//    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(500);
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    QTest::qWait(500);
}

/**
 *  @brief 测试拍照
 */
TEST_F(ZMainwindowTest, TakePicture)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

//    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
//    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 500);
//    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

//    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
//    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
//    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(500);

}

/**
 *  @brief 切换摄像头
 */
/*TEST_F(ZMainwindowTest, ChangeCamera)
{
    DIconButton *selectBtn = mainwindow->findChild<DIconButton *>(BUTTOM_TITLE_SELECT);

    if (selectBtn->isVisible()) {
        QTest::mouseClick(selectBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    }
    QTest::qWait(1000);
}*/

/**
 *  @brief 三连拍
 */
/*TEST_F(ZMainwindowTest, ThreeContinuousShooting)
{
    QString str = QDir::homePath();
    QString picture = str + QString("/Music/");

    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 1);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().setPathOption("picdatapath", QVariant(picture));
    dc::Settings::get().setPathOption("vddatapath", QVariant(picture));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 500);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(3000);

}*/

/**
 *  @brief 十连拍
 */
/*TEST_F(ZMainwindowTest, TenContinuousShooting)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 2);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().setPathOption("picdatapath", QVariant(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 500);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(10000);

}*/

/**
 *  @brief 连拍取消
 */
/*TEST_F(ZMainwindowTest, ContinuousShootingCancel)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 2);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 500);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(2000);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(500);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(4500);

    while (1) {
        if (iconpixBtn->isEnabled()) {
            QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
            QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
            QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
            break;
        } else {
            continue;
        }
    }
    QTest::qWait(500);

}*/

/**
 *  @brief 延迟拍照
 */
/*TEST_F(ZMainwindowTest, TakePicDelay)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 500);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(10000);

}*/

/**
 *  @brief 最大化
 */
TEST_F(ZMainwindowTest, MaxMinWindow)
{
    DWindowMaxButton *WindowMaxBtnOpt = mainwindow->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
    QTest::mouseClick(WindowMaxBtnOpt, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(500);

    QTest::mouseClick(WindowMaxBtnOpt, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(500);
}

/**
 *  @brief 右键菜单
 */
/*TEST_F(ZMainwindowTest, rightbtn)
{
    //点击鼠标右键
    QTest::qWait(500);
    QMenu *rightMouseMenu = mainwindow->findChild<QMenu *>(BUTTON_RIGHT_MENU);
    if (rightMouseMenu->isVisible())
        rightMouseMenu->hide();
}*/

/**
 *  @brief 录像
 */
TEST_F(ZMainwindowTest, TakeVideo1)
{
//    Stub stub;
//    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().setPathOption("vddatapath", QVariant(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

//    iconVdBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);
//    if (iconVdBtn) {
//        QTest::mouseMove(iconVdBtn, QPoint(0, 0), 500);
//        QTest::mousePress(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//        QTest::mouseRelease(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
//    }

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    if (PixVdBtn) {
        QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
        QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    QTest::qWait(1500);
    DWindowMinButton *windowMinBtn = mainwindow->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
    if (windowMinBtn)
        QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(1500);
    if (mainwindow->isMinimized()) {
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();
    }

    QTest::qWait(1000);
    QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(500);
    if (mainwindow->isMinimized()) {
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();
    }

    QTest::qWait(1000);
    QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(1000);
    if (mainwindow->isMinimized()) {
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();
    }

    EndBtn = mainwindow->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
    if (EndBtn && EndBtn->isVisible()) {
        QTest::mouseMove(EndBtn, QPoint(0, 0), 500);
        QTest::mousePress(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
}

/**
 *  @brief 延迟录像
 */
TEST_F(ZMainwindowTest, TakeVideoDelay)
{
    QString str = QDir::homePath();
    QString video = str + QString("/Music/");
//    Stub stub;
//    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    dc::Settings::get().setPathOption("vddatapath", QVariant("video"));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

//    iconVdBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);

//    if (iconVdBtn) {
//        QTest::mouseMove(iconVdBtn, QPoint(0, 0), 500);
//        QTest::mousePress(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//        QTest::mouseRelease(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
//    }
    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);

    if (PixVdBtn) {
        QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
        QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::qWait(4500);
    }
    EndBtn = mainwindow->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
    if (EndBtn && EndBtn->isVisible()) {
        //窗口按钮退出
        DWindowCloseButton *windowcloseBtn = mainwindow->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
        QTest::mouseMove(windowcloseBtn, QPoint(0, 0), 500);
        QTest::mousePress(windowcloseBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(windowcloseBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    QTest::qWait(500);
    if (EndBtn && EndBtn->isVisible()) {
        //菜单键退出
        QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
        QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::qWait(3000);
        CamApp->popupConfirmDialog();
    }
    QTest::qWait(1000);

    //结束录制
    if (PixVdBtn) {
        QTest::mouseMove(PixVdBtn, QPoint(0, 0), 500);
        QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::qWait(6000);
    }
//    if (EndBtn->isVisible()) {
//        QTest::mouseMove(EndBtn, QPoint(0, 0), 500);
//        QTest::mousePress(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//        QTest::mouseRelease(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
//    }

}

/**
 *  @brief 重置设置页面
 */
TEST_F(ZMainwindowTest, SettingDialogReset)
{
    mainwindow->settingDialog();
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    if (dialog == nullptr)
        return ;

    dialog->show();

    auto reset = dialog->findChild<QPushButton *>("SettingsContentReset");
    if (reset) {
        QTest::mouseMove(reset, QPoint(0, 0), 500);
        QTest::mousePress(reset, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(reset, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    QTest::qWait(1000);
    dialog->hide();
    mainwindow->settingDialogDel();
}

/**
 *  @brief 设置页面
 */
TEST_F(ZMainwindowTest, SettingDialogShow)
{
    dc::Settings::get().settings()->setOption("outsetting.resolutionsetting.resolution","1920x1080");
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);

    dc::Settings::get().settings()->sync();
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    if (dialog == nullptr)
        return ;

    dialog->show();

    //拍照路径
    DPushButton *openpicfilebtn = dialog->findChild<DPushButton *>(BUTTON_OPTION_PIC_LINE_EDIT);
    DDialog *invaliddlg = dialog->findChild<DDialog *>(OPTION_INVALID_DIALOG);
    DLineEdit *piclineedit = dialog->findChild<DLineEdit *>(OPTION_PIC_SELECTABLE_LINE_EDIT);

    if (piclineedit) {
        piclineedit->clear();
        piclineedit->setText(QDir::homePath() + "/Musics/");
        emit piclineedit->editingFinished();
        emit piclineedit->focusChanged(true);
    }
    if (openpicfilebtn) {
        emit openpicfilebtn->clicked();
        QTest::mouseMove(openpicfilebtn, QPoint(0, 0), 500);
        QTest::mousePress(openpicfilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(openpicfilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    if (invaliddlg)
        emit invaliddlg->close();
    QTest::qWait(500);

    //视频路径
    DPushButton *openvideofilebtn = dialog->findChild<DPushButton *>(BUTTON_OPTION_VIDEO_LINE_EDIT);
    DLineEdit *videolineedit = dialog->findChild<DLineEdit *>(OPTION_VIDEO_SELECTABLE_LINE_EDIT);

    if (videolineedit) {
        videolineedit->clear();
        videolineedit->setText(QDir::homePath() + "/Musics/");
        emit videolineedit->editingFinished();
        emit videolineedit->focusChanged(true);
    }
    if (openvideofilebtn) {
        emit openvideofilebtn->clicked();
        QTest::mouseMove(openvideofilebtn, QPoint(0, 0), 500);
        QTest::mousePress(openvideofilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(openvideofilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    if (invaliddlg)
        emit invaliddlg->close();
    QTest::qWait(500);

    dialog->hide();
    mainwindow->settingDialogDel();
}

/**
 *  @brief 切换分辨率
 */
TEST_F(ZMainwindowTest, ChangeResolution)
{
    mainwindow->settingDialog();
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    dialog->show();

    QComboBox *cmbox = dialog->findChild<QComboBox *>("OptionLineEdit");
    mainwindow->update();
    if (cmbox->count() > 2) {
        cmbox->setCurrentIndex(1);
        dc::Settings::get().settings()->sync();
    }
    mainwindow->settingDialogDel();
}

/**
 *  @brief 打开摄像头设置页面
 */
TEST_F(ZMainwindowTest, SettingDialogOpen)
{
    mainwindow->settingDialog();
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    dialog->show();

    QTest::qWait(1000);

    dialog->hide();
    mainwindow->settingDialogDel();
}


/**
 *  @brief Setting类打桩
 */
TEST_F(ZMainwindowTest, Setting)
{
    //进入setNewResolutionList设备句柄不为空分支
//    Stub stub;
//    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    dc::Settings::get().setNewResolutionList();

    //进入setNewResolutionList有效分辨率分支
//    stub.set(v4l2core_get_format_resolution_index, ADDR(Stub_Function, v4l2core_get_format_resolution_index));
//    stub.set(v4l2core_get_frame_format_index, ADDR(Stub_Function, v4l2core_get_frame_format_index));
//    stub.set(v4l2core_get_formats_list, ADDR(Stub_Function, v4l2core_get_formats_list));
//    stub.set(ADDR(QString, toInt), ADDR(Stub_Function, toInt));
    dc::Settings::get().setNewResolutionList();

    //进入setNewResolutionList无效分辨率分支
    //stub.reset(ADDR(QString, toInt));
    dc::Settings::get().setNewResolutionList();

    //调用generalOption
    dc::Settings::get().generalOption("");

    //打桩函数全部还原
//    stub.reset(get_v4l2_device_handler);
//    stub.reset(v4l2core_get_format_resolution_index);
//    stub.reset(v4l2core_get_frame_format_index);
//    stub.reset(v4l2core_get_formats_list);
}




/**
 *  @brief ThumbWidget类
 */
/*TEST_F(ZMainwindowTest, ThumbWidget)
{
    ThumbnailsBar *thumbnailBar = mainwindow->findChild<ThumbnailsBar *>(THUMBNAIL);

    QFocusEvent focusOut(QEvent::FocusOut);
    QFocusEvent focusIn(QEvent::FocusIn);
    QPaintEvent paintevent(QRect(0, 0, 0, 0));

    qApp->sendEvent(thumbnailBar->m_thumbLeftWidget, &focusOut);
    qApp->sendEvent(thumbnailBar->m_thumbLeftWidget, &focusIn);
    qApp->sendEvent(thumbnailBar->m_thumbLeftWidget, &paintevent);
}*/

/**
 *  @brief CMainWindow类打桩
 */
TEST_F(ZMainwindowTest, CMainWindow)
{
    //Stub stub;
    //调用非wayland锁屏定时器槽函数
    QVariantMap key2value1;
    QStringList strList;
    call_private_fun::CMainWindowonTimeoutLock(*mainwindow, "", key2value1, strList);
    //调用wayland锁屏定时器槽函数
    QVariantMap key2value2;
    key2value2.insert("Locked", true);
//    access_private_field::CMainWindowm_thumbnail(*mainwindow)->m_nStatus = STATPicIng;
    mainwindow->setWayland(true);
    call_private_fun::CMainWindowonTimeoutLock(*mainwindow, "", key2value2, strList);
    //调用wayland锁屏定时器槽函数
    QVariantMap key2value3;
    key2value3.insert("Locked", false);
    call_private_fun::CMainWindowonTimeoutLock(*mainwindow, "", key2value3, strList);
    //还原wayland状态
    mainwindow->setWayland(false);

    //调用settingDialog函数
    mainwindow->settingDialog();
    //进入settingDialog设备句柄不为空分支
    //stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    mainwindow->settingDialog();
    //进入settingDialog有效分辨率分支
//    stub.set(v4l2core_get_format_resolution_index, ADDR(Stub_Function, v4l2core_get_format_resolution_index));
//    stub.set(v4l2core_get_frame_format_index, ADDR(Stub_Function, v4l2core_get_frame_format_index));
//    stub.set(v4l2core_get_formats_list, ADDR(Stub_Function, v4l2core_get_formats_list));
//    stub.set(ADDR(QString, toInt), ADDR(Stub_Function, toInt));
//    mainwindow->settingDialog();

    //进入settingDialog无效分辨率分支
    //stub.reset(ADDR(QString, toInt));
    mainwindow->settingDialog();
    //stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    mainwindow->settingDialog();
    //打桩函数全部还原
//    stub.reset(get_v4l2_device_handler);
//    stub.reset(v4l2core_get_format_resolution_index);
//    stub.reset(v4l2core_get_frame_format_index);
//    stub.reset(v4l2core_get_formats_list);

    //调用录制状态关闭窗口函数
    access_private_field::CMainWindowm_videoPre(*mainwindow)->setCapStatus(true);

    //调用显示相机可选按钮窗口
    call_private_fun::CMainWindowsetSelBtnShow(*mainwindow);

    //调用拍照屏幕休眠函数
    mainwindow->setWayland(true);
    call_private_fun::CMainWindowsetSelBtnShow(*mainwindow);
    call_private_fun::CMainWindowstopCancelContinuousRecording(*mainwindow, true);
    mainwindow->setWayland(false);

//    //调用路径文件改变槽函数
//    //路径文件夹无改变分支
//    call_private_fun::CMainWindowonDirectoryChanged(*mainwindow, "");
//    //调用图片视频存储路径不存在分支
//    mainwindow->lastPicFileName = QString("/a");
//    mainwindow->lastVdFileName = QString("/a");
    //access_private_static_field::CMainWindow::CMainWindowlastPicFileName() = QString("/a");
    //access_private_static_field::CMainWindow::CMainWindowlastVdFileName() = QString("/a");
    call_private_fun::CMainWindowonDirectoryChanged(*mainwindow, "");

    //调用onEnableTitleBar的case
    call_private_fun::CMainWindowonEnableTitleBar(*mainwindow, 1);
    call_private_fun::CMainWindowonEnableTitleBar(*mainwindow, 2);
    call_private_fun::CMainWindowonEnableTitleBar(*mainwindow, 5);

    //调用lastOpenedPath最后打开路径不存在分支
//    stub.set(ADDR(QVariant, toString), ADDR(Stub_Function, toString));
//    stub.set(ADDR(QDir, mkdir), ADDR(Stub_Function, mkdir));
//    stub.set(ADDR(QDir, currentPath), ADDR(Stub_Function, currentPath));
    //删除视频文件夹的相机文件夹
    QDir dir;
    QString del_file = QString(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                               + QDir::separator() + QObject::tr("Camera"));
    dir.setPath(del_file);
    dir.removeRecursively();
    mainwindow->lastOpenedPath(QStandardPaths::MoviesLocation);
    mainwindow->lastOpenedPath(QStandardPaths::PicturesLocation);
    //桩函数还原
//    stub.reset(ADDR(QVariant, toString));
//    stub.reset(ADDR(QDir, mkdir));
//    stub.reset(ADDR(QDir, currentPath));
}

TEST_F(ZMainwindowTest, PhotoRecordBtnEventTest)
{
   QWidget* btn = mainwindow->findChild<QWidget *>(BUTTON_PICTURE_VIDEO);
   QTest::mouseMove(btn, QPoint(5, 5), 500);
   QTest::mouseClick(btn, Qt::LeftButton);
   QTest::mousePress(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
   QTest::mouseRelease(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
   QTest::qWait(500);
}

TEST_F(ZMainwindowTest, SwitchCameraBtnEventTest)
{
   QWidget* btn = mainwindow->findChild<QWidget *>(CAMERA_SWITCH_BTN);
   btn->show();
   QTest::mouseMove(btn, QPoint(5, 5), 500);
   QTest::mouseClick(btn, Qt::LeftButton);
   QTest::mousePress(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
   QTest::mouseRelease(btn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
   QTest::qWait(500);
}

//先删除该用例，会导致挂起
//TEST_F(ZMainwindowTest, ShortCurtTest)
//{
//    QTest::keyClick(mainwindow, Qt::Key_Return);
//    QTest::keySequence(mainwindow, QKeySequence("Ctrl+Shift+/"));
//    QTest::keyClick(mainwindow, Qt::Key_Space);
//}

//TEST_F(ZMainwindowTest, SwitchToRecord)
//{
//    emit access_private_field::CMainWindowm_switchPhotoBtn(*mainwindow)->clicked();
//}

//TEST_F(ZMainwindowTest, SwitchToPhoto)
//{
//    emit access_private_field::CMainWindowm_switchRecordBtn(*mainwindow)->clicked();
//}

/**
 *  @brief 应用退出
 */
TEST_F(ZMainwindowTest, QuitAction)
{
    call_private_fun::CApplicationQuitAction(*CamApp);
    QTest::qWait(1000);
}

/**
 *  @brief 主窗口退出(这个case一定得放在最后)
 */
TEST_F(ZMainwindowTest, quitmainwindow)
{
    delete mainwindow;
    mainwindow = nullptr;
}
