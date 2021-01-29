#define private public
#include "src/mainwindow.h"
#undef private
#include "src/imageitem.h"

#include <QComboBox>
#include <QAction>
#include <QProcess>
#include <QShortcut>
#include <QMap>

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

using namespace Dtk::Core;

extern QMap<int, ImageItem *> g_indexImage;

MainwindowTest::MainwindowTest()
{
    if (!mainwindow)
        mainwindow = CamApp->getMainWindow();

    if (!mainwindow->isVisible()) {
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
    QList<QProcess *> proclist = CamApp->getprocess();

    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);
    QTest::qWait(1000);
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
TEST_F(MainwindowTest, TabChange)
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
TEST_F(MainwindowTest, RightMenushortCut)
{
    //点击鼠标右键
    QTest::qWait(1000);
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
TEST_F(MainwindowTest, shortcut)
{
    QShortcut *ShortcutView =  mainwindow->findChild<QShortcut *>(SHORTCUT_VIEW);
    QShortcut *ShortcutSpace = mainwindow->findChild<QShortcut *>(SHORTCUT_SPACE);
    emit ShortcutView->activated();
    emit ShortcutSpace->activated();
}

/**
 *  @brief 主题切换
 */
TEST_F(MainwindowTest, Themechange)
{
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    QTest::qWait(1000);
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::DarkType);

    iconVdBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);

    QTest::mouseMove(iconVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::qWait(1000);

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(1000);
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    QTest::qWait(1000);
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

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(1000);

}

/**
 *  @brief 切换摄像头
 */
TEST_F(MainwindowTest, ChangeCamera)
{
    DIconButton *selectBtn = mainwindow->findChild<DIconButton *>(BUTTOM_TITLE_SELECT);

    if (selectBtn->isVisible()) {
        QTest::mouseClick(selectBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    }
    QTest::qWait(2000);
}

/**
 *  @brief 三连拍
 */
TEST_F(MainwindowTest, ThreeContinuousShooting)
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
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(3000);

}

/**
 *  @brief 删除快捷键
 */
TEST_F(MainwindowTest, DelShortCut)
{
    videowidget *prevideo = mainwindow->findChild<videowidget *>(VIDEO_PREVIEW_WIDGET);

    //点击鼠标右键
    QTest::qWait(1000);
    QTest::mouseMove(prevideo, QPoint(0, 0), 1000);
    for (int i = 0; i < 3; i++) {
        QTest::keyClick(prevideo, Qt::Key_Delete, Qt::NoModifier, 0);
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
    dc::Settings::get().setPathOption("picdatapath", QVariant(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(10000);

}

/**
 *  @brief 连拍取消
 */
TEST_F(MainwindowTest, ContinuousShootingCancel)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 2);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(2000);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(500);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    QTest::qWait(4500);

    while (1) {
        if (iconpixBtn->isEnabled()) {
            QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
            QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
            QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
            break;
        } else {
            continue;
        }
    }
    QTest::qWait(500);

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

    iconpixBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_PICTURE);
    QTest::mouseMove(iconpixBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconpixBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(10000);

}

/**
 *  @brief 最大化
 */
TEST_F(MainwindowTest, MaxMinWindow)
{
    DWindowMaxButton *WindowMaxBtnOpt = mainwindow->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
    QTest::mouseClick(WindowMaxBtnOpt, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(1000);

    QTest::mouseClick(WindowMaxBtnOpt, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(1000);
}

/**
 *  @brief 右键菜单
 */
TEST_F(MainwindowTest, rightbtn)
{
    //点击鼠标右键
    QTest::qWait(1000);
    QMenu *rightMouseMenu = mainwindow->findChild<QMenu *>(BUTTON_RIGHT_MENU);
    if (rightMouseMenu->isVisible())
        rightMouseMenu->hide();
}

/**
 *  @brief 缩略图多选和复制，删除
 */
TEST_F(MainwindowTest, ImageItemContinuousChoose)
{
    //Ctrl多选
    QTest::keyPress(mainwindow, Qt::Key_Control, Qt::NoModifier, 500);
    QMap<int, ImageItem *> ImageMap = get_imageitem();
    QList<ImageItem *> ImageList;
    ImageList.clear();
    int number = ImageMap.count();
    if (number > 0) {
        ImageList = ImageMap.values();
        if (ImageList.count() > 3) {
            for (int i = 1; i < 3; i++) {
                QTest::mouseMove(ImageList[i], QPoint(0, 0), 1000);
                QTest::mouseClick(ImageList[i], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
                QTest::qWait(1000);
            }

        } else {
            for (int i = 1; i < ImageList.count(); i++) {
                QTest::mouseMove(ImageList[i], QPoint(0, 0), 1000);
                QTest::mouseClick(ImageList[i], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
                QTest::qWait(1000);
            }
        }

        QTest::keyRelease(mainwindow, Qt::Key_Control, Qt::NoModifier, 500);
        //Shift多选
        if (get_imageitem().count() > 2) {
            QTest::mouseMove(ImageMap[1], QPoint(0, 0), 500);
            QTest::mousePress(ImageMap[1], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
            QTest::mouseRelease(ImageMap[1], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);

            QTest::keyPress(mainwindow, Qt::Key_Shift, Qt::ShiftModifier, 500);
            QTest::mousePress(ImageMap[2], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);
            QTest::mouseRelease(ImageMap[2], Qt::LeftButton, Qt::ShiftModifier, QPoint(0, 0), 200);

            QTest::mouseMove(ImageMap[0], QPoint(0, 0), 500);
            QTest::mousePress(ImageMap[0], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
            QTest::mouseRelease(ImageMap[0], Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
        }

        QAction *copyact = mainwindow->findChild<QAction *>("CopyAction");
        copyact->trigger();

        QAction *delact = mainwindow->findChild<QAction *>("DelAction");
        delact->trigger();


    }



}

/**
 *  @brief 打印窗口
 */
TEST_F(MainwindowTest, printdialog)
{
    QMap<int, ImageItem *> it = get_imageitem();

    if (it.count() > 0) {
        QAction *print = mainwindow->findChild<QAction *>("PrinterAction");
        print->trigger();
        QTest::qWait(1000);
    }
}

/**
 *  @brief 录像
 */
TEST_F(MainwindowTest, TakeVideo1)
{
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().setPathOption("vddatapath", QVariant(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconVdBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);

    QTest::mouseMove(iconVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    QTest::qWait(2000);
    DWindowMinButton *windowMinBtn = mainwindow->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
    QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(3000);
    if (mainwindow->isMinimized()) {
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();
    }

    QTest::qWait(2000);
    QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(1500);
    if (mainwindow->isMinimized()) {
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();
    }

    QTest::qWait(2000);
    QTest::mouseClick(windowMinBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 200);
    QTest::qWait(1500);
    if (mainwindow->isMinimized()) {
        mainwindow->showNormal();
        mainwindow->show();
        mainwindow->activateWindow();
    }

    EndBtn = mainwindow->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
    if (EndBtn->isVisible()) {
        QTest::mouseMove(EndBtn, QPoint(0, 0), 1000);
        QTest::mousePress(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
}

/**
 *  @brief 延迟录像
 */
TEST_F(MainwindowTest, TakeVideoDelay)
{
    QString str = QDir::homePath();
    QString video = str + QString("/Music/");
    mainwindow->settingDialog();
    dc::Settings::get().settings()->setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().settings()->setOption(QString("photosetting.photosdelay.photodelays"), 1);
    dc::Settings::get().setPathOption("vddatapath", QVariant("video"));
    dc::Settings::get().settings()->sync();
    mainwindow->settingDialogDel();

    iconVdBtn =  mainwindow->findChild<DButtonBoxButton *>(BUTTOM_TITLE_VEDIO);

    QTest::mouseMove(iconVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(iconVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);

    PixVdBtn = mainwindow->findChild<QPushButton *>(BUTTON_PICTURE_VIDEO);
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(4500);

    EndBtn = mainwindow->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
    if (EndBtn->isVisible()) {
        //窗口按钮退出
        DWindowCloseButton *windowcloseBtn = mainwindow->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
        QTest::mouseMove(windowcloseBtn, QPoint(0, 0), 1000);
        QTest::mousePress(windowcloseBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(windowcloseBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }
    QTest::qWait(1000);
    if (EndBtn->isVisible()) {
        //菜单键退出
        QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
        QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::qWait(6000);
        CamApp->popupConfirmDialog();
    }
    QTest::qWait(1000);

    //结束录制
    QTest::mouseMove(PixVdBtn, QPoint(0, 0), 1000);
    QTest::mousePress(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(PixVdBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(6000);

    if (EndBtn->isVisible()) {
        QTest::mouseMove(EndBtn, QPoint(0, 0), 1000);
        QTest::mousePress(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(EndBtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }

}

/**
 *  @brief 重置设置页面
 */
TEST_F(MainwindowTest, SettingDialogReset)
{
    mainwindow->settingDialog();
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    dialog->show();

    auto reset = dialog->findChild<QPushButton *>("SettingsContentReset");

    QTest::mouseMove(reset, QPoint(0, 0), 1000);
    QTest::mousePress(reset, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(reset, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::qWait(2000);
    dialog->hide();
    mainwindow->settingDialogDel();
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
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    dialog->show();

    //拍照路径
    DPushButton *openpicfilebtn = dialog->findChild<DPushButton *>(BUTTON_OPTION_PIC_LINE_EDIT);
    DDialog *invaliddlg = dialog->findChild<DDialog *>(OPTION_INVALID_DIALOG);
    DLineEdit *piclineedit = dialog->findChild<DLineEdit *>(OPTION_PIC_SELECTABLE_LINE_EDIT);

    piclineedit->clear();
    piclineedit->setText(QDir::homePath() + "/Musics/");
    emit piclineedit->editingFinished();
    emit piclineedit->focusChanged(true);

    emit openpicfilebtn->clicked();
    QTest::mouseMove(openpicfilebtn, QPoint(0, 0), 1000);
    QTest::mousePress(openpicfilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(openpicfilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    emit invaliddlg->close();
    QTest::qWait(500);

    //视频路径
    DPushButton *openvideofilebtn = dialog->findChild<DPushButton *>(BUTTON_OPTION_VIDEO_LINE_EDIT);
    DLineEdit *videolineedit = dialog->findChild<DLineEdit *>(OPTION_VIDEO_SELECTABLE_LINE_EDIT);

    videolineedit->clear();
    videolineedit->setText(QDir::homePath() + "/Musics/");
    emit videolineedit->editingFinished();
    emit videolineedit->focusChanged(true);

    emit openvideofilebtn->clicked();
    QTest::mouseMove(openvideofilebtn, QPoint(0, 0), 1000);
    QTest::mousePress(openvideofilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(openvideofilebtn, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);

    emit invaliddlg->close();
    QTest::qWait(500);

    dialog->hide();
    mainwindow->settingDialogDel();
}

/**
 *  @brief 删除单张缩略图
 */
TEST_F(MainwindowTest, ImageItemDel)
{

//    QTest::qWait(1000);
    QMap<int, ImageItem *> it = get_imageitem();
    ImageItem *imgit = nullptr;

    if (it.count() > 0) {
        imgit = it.value(0);
        if (!imgit)
            return;

        //点击鼠标右键
        QTest::qWait(1000);
        QTest::mouseMove(imgit, QPoint(0, 0), 1000);
        QTest::mousePress(imgit, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(imgit, Qt::RightButton, Qt::NoModifier, QPoint(0, 0), 0);

        //点击鼠标左键
        QTest::qWait(1000);
        QTest::mouseMove(imgit, QPoint(0, 0), 1000);
        QTest::mousePress(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
        QTest::mouseRelease(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    }

    if (!it.isEmpty()) {
        imgit = it.value(0);
        QTest::keyPress(imgit, Qt::Key_C, Qt::ControlModifier, 500);
        QTest::keyRelease(imgit, Qt::Key_C, Qt::ControlModifier, 500);
        QTest::keyClick(imgit, Qt::Key_Delete, Qt::NoModifier, 0);
    }
}

/**
 *  @brief 切换分辨率
 */
TEST_F(MainwindowTest, ChangeResolution)
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
TEST_F(MainwindowTest, SettingDialogOpen)
{
    mainwindow->settingDialog();
    DSettingsDialog *dialog = mainwindow->findChild<DSettingsDialog * >(SETTING_DIALOG);
    dialog->show();

    QTest::qWait(3000);

    dialog->hide();
    mainwindow->settingDialogDel();
}

/**
 *  @brief 最大延迟帧
 */
TEST_F(MainwindowTest, reachMaxDelayedFrames)
{
    MajorImageProcessingThread *processThread = mainwindow->findChild<MajorImageProcessingThread *>("MajorThread");
    emit processThread->reachMaxDelayedFrames();
}

/**
 *  @brief Setting类打桩
 */
TEST_F(MainwindowTest, Setting)
{
    //进入setNewResolutionList设备句柄不为空分支
    Stub stub;
    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    dc::Settings::get().setNewResolutionList();

    //进入setNewResolutionList有效分辨率分支
    stub.set(v4l2core_get_format_resolution_index, ADDR(Stub_Function, v4l2core_get_format_resolution_index));
    stub.set(v4l2core_get_frame_format_index, ADDR(Stub_Function, v4l2core_get_frame_format_index));
    stub.set(v4l2core_get_formats_list, ADDR(Stub_Function, v4l2core_get_formats_list));
    stub.set(ADDR(QString, toInt), ADDR(Stub_Function, toInt));
    dc::Settings::get().setNewResolutionList();

    //进入setNewResolutionList无效分辨率分支
    stub.reset(ADDR(QString, toInt));
    dc::Settings::get().setNewResolutionList();

    //打桩函数全部还原
    stub.reset(get_v4l2_device_handler);
    stub.reset(v4l2core_get_format_resolution_index);
    stub.reset(v4l2core_get_frame_format_index);
    stub.reset(v4l2core_get_formats_list);
}

/**
 *  @brief thumbnailsbar类打桩
 */
TEST_F(MainwindowTest, thumbarnail)
{
    Stub stub;
    ThumbnailsBar *thumbnailsBarNew = new ThumbnailsBar();
    //调用setBtntooltip()
    thumbnailsBarNew->setBtntooltip();
    //调用设备不可用状态分支
    thumbnailsBarNew->onBtnClick();
    //调用onBtnClick（）设备可用状态分支
    stub.set(ADDR(DataManager, getdevStatus), ADDR(Stub_Function, getdevStatus));
    //调用拍照分支的非正在拍照状态
    thumbnailsBarNew->onBtnClick();
    //调用拍照分支的正在拍照状态
    CMainWindow *mw = new CMainWindow();
    thumbnailsBarNew->onBtnClick();
    //调用录像分支但不处于正在录像分支
    mw->m_nActTpye = ActTakeVideo;
    thumbnailsBarNew->ChangeActType(mw->m_nActTpye);
    thumbnailsBarNew->onBtnClick();
    //调用录像分支但处于正在录像分支
    thumbnailsBarNew->onBtnClick();
    //还原
    stub.reset(ADDR(DataManager, getdevStatus));
    //调用onShortcutDel删除太快分支
    stub.set(ADDR(QDateTime, msecsTo), ADDR(Stub_Function, msecsTo));
    thumbnailsBarNew->onShortcutDel();
    delete thumbnailsBarNew;
    //还原
    stub.reset(ADDR(QDateTime, msecsTo));

    //调用addFile()
    QString str = QString(QDir::homePath() + "/Pictures/" + QObject::tr("Camera"));
    qWarning() << QDir(str).exists();
    ThumbnailsBar *thumbnailsBar = mainwindow->findChild<ThumbnailsBar *>(THUMBNAIL);
    thumbnailsBar->addFile(str);
    //调用onTrashFile()
    QMap<int, ImageItem *> it = get_imageitem();
    ImageItem *imgit = nullptr;

    if (it.count() > 0) {
        imgit = it.value(0);
        if (!imgit)
            return;
    }
    QTest::mouseMove(imgit, QPoint(0, 0), 1000);
    QTest::mousePress(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 500);
    QTest::mouseRelease(imgit, Qt::LeftButton, Qt::NoModifier, QPoint(0, 0), 0);
    thumbnailsBar->onTrashFile();
    //调用delFile
    thumbnailsBar->delFile(str);
    //调用widthChanged
    thumbnailsBar->widthChanged();

}

TEST_F(MainwindowTest, videowidget)
{
    videowidget *videowidgt = mainwindow->findChild<videowidget *>(VIDEO_PREVIEW_WIDGET);

    //调用videowidget构造函数中的wayland平台设置背景色为黑色分支
    Stub stub;
    stub.set(get_wayland_status, ADDR(Stub_Function, get_wayland_status));
    DGuiApplicationHelper::instance()->setThemeType(DGuiApplicationHelper::DarkType);
    videowidget *videowidgetTest = new videowidget();
    stub.reset(get_wayland_status);

    //调用delayInit函数
    QTest::qWait(500);
    //调用无设备状态分支
    DataManager::instance()->setdevStatus(CAM_CANNOT_USE);
    videowidgetTest->delayInit();
    //启动失败分支
    stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));
    videowidgetTest->delayInit();
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    videowidgetTest->delayInit();
    stub.reset(camInit);

    //调用showNocam函数
    QTest::qWait(500);
    videowidgt->showNocam();
    DGuiApplicationHelper::instance()->setThemeType(DGuiApplicationHelper::LightType);//切换主题
    videowidgt->showNocam();

    //调用showCamUsed函数
    QTest::qWait(500);
    videowidgt->showCamUsed();
    DGuiApplicationHelper::instance()->setThemeType(DGuiApplicationHelper::DarkType);//切换主题
    videowidgt->showCamUsed();

    //调用onReachMaxDelayedFrames函数
    QTest::qWait(500);
    videowidgt->onReachMaxDelayedFrames();

    //调用showCountDownLabel函数
    videowidgt->showCountDownLabel(PICTRUE);//调用PICTRUE分支
    videowidgt->showCountDownLabel(VIDEO);//调用VIDEO分支
    videowidgt->showCountDownLabel(NODEVICE);//调用NODEVICE分支

    //调用showCountdown函数
    //无相机停止定时器
    stub.set(ADDR(QTimer, isActive), ADDR(Stub_Function, isActive));
    //倒计时显示隐藏打桩
    stub.set(ADDR(DFloatingWidget, isVisible), ADDR(Stub_Function, isVisible_Countdown));
    //倒计时laber显示隐藏打桩
    stub.set(ADDR(DLabel, isVisible), ADDR(Stub_Function, isVisible_Label));
    //调用无相机分支
    videowidgt->showCountdown();
    //调用相机可用分支下的拍照连拍次数为0分支
    DataManager::instance()->setdevStatus(CAM_CANUSE);
    videowidgt->m_curTakePicTime = 0;
    videowidgt->m_nInterval = 0;
    videowidgt->showCountdown();
    //调用相机可用分支下的拍照连拍次数不为0分支
    videowidgt->m_curTakePicTime = 1;
    videowidgt->showCountdown();
    //桩函数还原
    stub.reset(ADDR(DFloatingWidget, isVisible));
    stub.reset(ADDR(DLabel, isVisible));
    stub.reset(ADDR(QTimer, isActive));
    DataManager::instance()->setdevStatus(NOCAM);

    //调用showCountdown函数
    //调用时长不正常分支
    videowidgt->showRecTime();
    //由于时长m_nCount小于等于3视为为不正常时长，需要调用两次调用才能进入0时0分0秒状态
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_5));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_60));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);
    //调用6时6分6秒
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_21966));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);
    //调用11时11分11秒
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_40271));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);

    //调用flash函数
    videowidgt->flash();

    //调用slotresolutionchanged函数
    //调用相机不可用，则直接返回
    videowidgt->slotresolutionchanged("1184x656");
    //调用相机可用
    DataManager::instance()->setdevStatus(CAM_CANUSE);
    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    videowidgt->slotresolutionchanged("1184x656");
    stub.reset(get_v4l2_device_handler);
    DataManager::instance()->setdevStatus(NOCAM);
    //调用onEndBtnClicked函数
    videowidgt->setCapStatus(true);
    //录像标志函数打桩
    stub.set(video_capture_get_save_video, ADDR(Stub_Function, video_capture_get_save_video));
    videowidgt->onEndBtnClicked();
    stub.reset(video_capture_get_save_video);

    //调用onRestartDevices函数
    videowidgt->onRestartDevices();

    //调用onChangeDev函数
    //进入设备数为2的无设备分支
    stub.set(get_device_list, ADDR(Stub_Function, get_device_list_2));
    videowidgt->onChangeDev();
    //进入设备数为2启动失败分支
    stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));
    videowidgt->onChangeDev();
    stub.reset(camInit);
    //进入设备数为2启动成功分支
    stub.set(camInit, ADDR(Stub_Function, camInit_OK));
    stub.set(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
    videowidgt->onChangeDev();
    //桩函数还原
    stub.reset(camInit);
    stub.reset(get_device_list);
    stub.reset(ADDR(MajorImageProcessingThread, start));
    stub.reset(get_device_list);
    //进入设备数为1的无设备分支
    stub.set(get_device_list, ADDR(Stub_Function, get_device_list_1));
    videowidgt->onChangeDev();
    //进入设备数为1启动失败分支
    stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));
    videowidgt->onChangeDev();
    stub.reset(camInit);
    //还原桩函数
    stub.reset(camInit);
    stub.reset(ADDR(MajorImageProcessingThread, start));
    stub.reset(get_device_list);

    //调用onTakePic函数
    videowidgt->onTakePic(true);
    videowidgt->onTakePic(false);

    //调用onTakeVideo函数
    //进入倒计时期间处理
    videowidgt->m_nInterval = 1;
    videowidgt->onTakeVideo();
    //录制完成处理
    videowidgt->setCapStatus(true);
    videowidgt->onTakeVideo();
    //进入最大间隔为0直接录制分支
    videowidgt->m_nInterval = 0;
    videowidgt->setCapStatus(false);
    videowidgt->m_Maxinterval = 0;
    videowidgt->onTakeVideo();
    //进入最大间隔不为0分支
    videowidgt->m_Maxinterval = 3;
    videowidgt->onTakeVideo();

    //调用forbidScrollBar函数
    videowidgt->forbidScrollBar(videowidgt->m_pNormalView);

    //调用startTakeVideo函数
    //录制完成处理
    videowidgt->setCapStatus(true);
    videowidgt->startTakeVideo();
    videowidgt->setCapStatus(false);
    //调用相机不可用分支
    videowidgt->startTakeVideo();
    //调用相机可用分支
    DataManager::instance()->setdevStatus(CAM_CANUSE);
    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    videowidgt->startTakeVideo();
    stub.reset(start_encoder_thread);

    //调用itemPosChange函数
    videowidgt->itemPosChange();

    //调用stopEverything函数
    videowidgt->stopEverything();

}

/**
 *  @brief 主窗口退出(这个case一定得放在最后)
 */
TEST_F(MainwindowTest, quitmainwindow)
{
    delete mainwindow;
    mainwindow = nullptr;
}
