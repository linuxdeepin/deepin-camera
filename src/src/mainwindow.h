/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "thumbnailsbar.h"
//#include "toolbar.h"
#include "videowidget.h"
#include "devnummonitor.h"
#include "closedialog.h"
#include "camview.h"
#include "Settings.h"

#include <DMainWindow>
#include <DTitlebar>
#include <DSettingsDialog>

#include <QObject>
#include <QPushButton>
#include <QDBusReply>
#include <QDBusInterface>
#include <QDBusUnixFileDescriptor>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
class QGridLayout;
const int TOP_TOOLBAR_HEIGHT = 50;
//const int TOOLBAR_MINIMUN_WIDTH = 630 - 20 + 10 + 2;


//应用层界面通信站，与底层通信通过proxy代理类
class CMainWindow : public DMainWindow
{
    Q_OBJECT
public:
    CMainWindow(DWidget *w = nullptr);

    /**
    * @brief lastOpenedPath　上一次打开的图片文件路径
    */
    static QString lastOpenedPicPath();

    /**
    * @brief lastOpenedPath　上一次打开的视频文件路径
    */
    static QString lastOpenedVideoPath();

    /**
    * @brief setWayland　判断是否是wayland，并初始化对应操作
    * @param bTrue
    */
    void setWayland(bool bTrue) {m_bWayland = bTrue;}

    /**
    * @brief noSettingPathsave　判断设置路径是否存在，当设置路径不存在时，图片默认存在～/Pictures/相机下，视频默认存在～/Videos/相机下
    */
    void SettingPathsave();

    /**
    * @brief settingDialog　加载设置对话框
    */
    void settingDialog();

    /**
    * @brief settingDialogDel　删除设置对话框
    */
    void settingDialogDel();

    /**
    * @brief loadAfterShow　延后加载
    */
    void loadAfterShow();

    ~CMainWindow() override;
private:
    /**
    * @brief initUI　初始化界面
    */
    void initUI();

    /**
    * @brief initTitleBar　初始化标题栏
    */
    void initTitleBar();

    /**
    * @brief initConnection　初始化连接状态
    */
    void initConnection();

    /**
    * @brief initThumbnails　初始化缩略图
    */
    void initThumbnails();

    /**
    * @brief initThumbnailsConn　初始化缩略图信号槽
    */
    void initThumbnailsConn();

    /**
     * @brief initBlockShutdown 阻塞关机
     */
    void initBlockShutdown();

    /**
     * @brief initBlockSleep 阻塞睡眠
     */
    void initBlockSleep();

    /**
     * @brief initTabOrder 设置住窗口tab顺序
     */
    void initTabOrder();

    /**
     * @brief initShortcut 设置快捷键
     */
    void initShortcut();

    /**
    * @brief setupTitlebar　装载标题栏
    */
    void setupTitlebar();

    /**
    * @brief resizeEvent　窗口大小改变时重新调整窗口
    */
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

    /**
    * @brief closeEvent　录制视频时关闭窗口事件
    */
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    /**
    * @brief changeEvent　录制视频时改变窗口事件
    */
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;


    /**
    * @brief menuItemInvoked 调用菜单选项
    * @param action  动作
    */
    void menuItemInvoked(QAction *action);

    /**
    * @brief settingsTriggered 触发设置
    * @param bTrue
    */
    void settingsTriggered(bool bTrue);

    /**
    * @brief slotPopupSettingsDialog 设置界面初始化
    */
    void slotPopupSettingsDialog();

    /**
    * @brief initDynamicLibPath 初始化动态库的路径
    */
    void initDynamicLibPath();

    /**
    * @brief libPath 动态库路径
    * @param strlib 路径的字符串
    */
    QString libPath(const QString &strlib);
private slots:
    /**
     * @brief setSelBtnHide 设置切换相机按钮隐藏
     */
    void setSelBtnHide();

    /**
     * @brief setSelBtnShow 设置切换相机按钮显示
     */
    void setSelBtnShow();

    /**
     * @brief onFitToolBar 自适应工具栏
     */
    void onFitToolBar();

    /**
     * @brief onEnableTitleBar 标题栏的禁用与恢复
     * @param nType 1、禁用标题栏视频；2、禁用标题栏拍照；3、恢复标题栏视频；4、恢复标题栏拍照
     */
    void onEnableTitleBar(int nType);

    /**
     * @brief onTitlePicBtn 设置标题栏拍照时按钮颜色
     */
    void onTitlePicBtn();

    /**
     * @brief onTitleVdBtn 设置标题栏录像时按钮颜色
     */
    void onTitleVdBtn();

    /**
     * @brief onSettingsDlgClose 连拍、延时、存储位置设置
     */
    void onSettingsDlgClose();

    /**
     * @brief onEnableSettings 设置可用调节
     * @param bTrue
     */
    void onEnableSettings(bool bTrue);

    /**
     * @brief onTakePicDone 拍照完成后设置属性
     */
    void onTakePicDone();

    /**
     * @brief onTakePicOnce 设置多连拍属性
     */
    void onTakePicOnce();

    /**
     * @brief onTakePicCancel 取消拍照
     */
    void onTakePicCancel();

    /**
     * @brief onTakeVdDone 录像完成后设置属性
     */
    void onTakeVdDone();

    /**
     * @brief onTakeVdCancel 取消录像
     */
    void onTakeVdCancel();

    /**
     * @brief onThemeChange 更换主题
     * @param type 三种：light、dark、跟随系统
     */
    void onThemeChange(DGuiApplicationHelper::ColorType type);

    /**
     * @brief updateBlockSystem 更新阻塞关机
     * @param bTrue
     */
    void updateBlockSystem(bool bTrue);

    /**
     * @brief onNoCam 找不到设备恢复按钮和状态
     */
    void onNoCam();

    /**
     * @brief onSleepWhenTaking 录像时如果处于休眠状态则关闭录像
     * @param bTrue
     */
    void onSleepWhenTaking(bool);

protected:

    /**
     * @brief onSleepWhenTaking 键盘按下事件
     * @param e
     */
    void keyPressEvent(QKeyEvent *e) override;

    /**
     * @brief onSleepWhenTaking 键盘弹起事件
     * @param e
     */
    void keyReleaseEvent(QKeyEvent *e) override;

public:
    static const int                minWindowWidth;//最小窗口宽度
    static const int                minWindowHeight;//最小窗口高度
private:
    static QString                  lastVdFileName;//上次打开的视频文件
    static QString                  lastPicFileName;//上次打开的照片文件
    DSettingsDialog                 *m_SetDialog = nullptr;//设置页面
    ThumbnailsBar                   *m_thumbnail = nullptr;//缩略图
    DMenu                           *m_titlemenu;//标题栏菜单
    QMenu                           *m_rightbtnmenu;//右键菜单
    QAction                         *m_actOpenfolder;//打开文件
    videowidget                     *m_videoPre = nullptr;//相机预览类
    QFileSystemWatcher              m_fileWatcher;//文件监控
    DevNumMonitor                   *m_devnumMonitor;//设备数量监控
    DButtonBox                      *pDButtonBox;//按钮盒
    DButtonBoxButton                *m_pTitlePicBtn;//标题栏拍照按钮
    DButtonBoxButton                *m_pTitleVdBtn;//标题栏视频按钮
    DIconButton                     *pSelectBtn; //切换按钮
    QString                         m_strCfgPath;//配置文件路径
    QAction                         *m_actionSettings;//打开设置页面
    enum ActType                    m_nActTpye;
    QDBusReply<QDBusUnixFileDescriptor> m_reply;
    QDBusInterface                 *m_pLoginManager = nullptr;
    QList<QVariant>                 m_arg;
    QDBusReply<QDBusUnixFileDescriptor> m_replySleep;
    QDBusInterface                 *m_pLoginMgrSleep = nullptr;
    QList<QVariant>                 m_argSleep;
    bool                            m_bWayland;
    QDBusInterface                  *m_pDBus = nullptr;//接收休眠信号，仅wayland使用
};

#endif // MAINWINDOW_H
