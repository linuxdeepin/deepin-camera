#include <QComboBox>
#include <QAction>
#include <QProcess>
#include <QShortcut>

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
    QTest::keyPress(mainwindow, Qt::Key_Shift, Qt::NoModifier, 500);
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
        QAction *copyact = mainwindow->findChild<QAction *>("CopyAction");
        copyact->trigger();

        QAction *delact = mainwindow->findChild<QAction *>("DelAction");
        delact->trigger();
        QTest::keyRelease(mainwindow, Qt::Key_Shift, Qt::NoModifier, 500);
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

    Stub stub;
    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    dc::Settings::get().setNewResolutionList();

    stub.set(v4l2core_get_format_resolution_index, ADDR(Stub_Function, v4l2core_get_format_resolution_index));
    stub.set(v4l2core_get_frame_format_index, ADDR(Stub_Function, v4l2core_get_frame_format_index));
    stub.set(v4l2core_get_formats_list, ADDR(Stub_Function, v4l2core_get_formats_list));
    stub.set(ADDR(QString, toInt), ADDR(Stub_Function, toInt));
    dc::Settings::get().setNewResolutionList();

    stub.reset(ADDR(QString, toInt));
    dc::Settings::get().setNewResolutionList();

    //打桩函数全部还原
    stub.reset(get_v4l2_device_handler);
    stub.reset(v4l2core_get_format_resolution_index);
    stub.reset(v4l2core_get_frame_format_index);
    stub.reset(v4l2core_get_formats_list);
}

/**
 *  @brief 主窗口退出(这个case一定得放在最后)
 */
TEST_F(MainwindowTest, quitmainwindow)
{
    delete mainwindow;
    mainwindow = nullptr;
}
