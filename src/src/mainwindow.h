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
#include "videowidget.h"
#include "devnummonitor.h"
#include "closedialog.h"
#include "camview.h"
#include "Settings.h"
#include "titlebar.h"

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
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <QFileInfo>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class ImageItem;
class QGridLayout;
class photoRecordBtn;
class SwitchCameraBtn;
class takePhotoSettingAreaWidget;

const int TOP_TOOLBAR_HEIGHT = 50;

/**
 * @brief ActType 定义枚举类型ActType，即拍照和录像
 */
enum ActType {ActTakePic, ActTakeVideo};

//应用层界面通信站，与底层通信通过proxy代理类
class CMainWindow : public DMainWindow
{
    Q_OBJECT
public:
    explicit CMainWindow(QWidget *parent = nullptr);

    /**
    * @brief lastOpenedPath　上一次打开的文件路径,如果路径错误或者不存在，不会创建，因为有权限问题和U盘拔出问题
    */
    static QString lastOpenedPath(QStandardPaths::StandardLocation standard);

    /**
    * @brief setWayland　判断是否是wayland，并初始化对应操作
    * @param bTrue
    */
    void setWayland(bool bTrue);

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

    /**
     * @brief recoverTabWidget 拍照、录制触发完过后恢复tab框
     */
    void recoverTabWidget(uint index);

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
    * @brief initRightButtons　初始化右侧功能按钮键
    */
    void initRightButtons();

    /**
    * @brief locateRightButtons　右侧功能键位置设置
    */
    void locateRightButtons();

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
     * @brief initEventFilter 安装事件过滤器
     */
    void initEventFilter();

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

    /**
    * @brief reflushMediaFileList　刷新媒体文件列表
    */
    void reflushMediaFileList();

    /**
    * @brief getMediaFileInfoList　获得录像、图片文件列表
    */
    void getMediaFileInfoList(const QString &path, QFileInfoList& fileList);

private slots:
    /**
     * @brief setSelBtnHide 设置切换相机按钮隐藏
     */
    void setSelBtnHide();

    /**
     * @brief onLocalTimeChanged DBus更改本地时间槽函数
     */
    void onLocalTimeChanged();

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
     * @brief onSleepWhenTaking 结束(取消)连拍、录制
     * @param bTrue
     */
    void stopCancelContinuousRecording(bool);

    /**
     * @brief onDirectoryChanged 文件夹删除、重命名监控
     * @param filePath 当前文件夹路径
     */
    void onDirectoryChanged(const QString &filePath);

    /**
     * @brief onSwitchCameraSuccess 切换摄像机成功处理
     * @param cameraName 摄像机名称
     */
    void onSwitchCameraSuccess(const QString& cameraName);

    /**
     * @brief onVisible 是否是锁屏
     */
    void onTimeoutLock(const QString &, QVariantMap, QStringList);

    /**
     * @brief onToolbarShow 是否显示上层按钮
     */
    void onToolbarShow(bool bShow);

    /**
     * @brief onTrashFile 删除文件
     * @param fileName 删除文件路径
     */
    void onTrashFile(const QString &fileName);

    /**
     * @brief onSwitchPhotoBtnClked 拍照按钮点击
     */
    void onSwitchPhotoBtnClked();

    /**
     * @brief onSwitchRecordBtnClked 拍照按钮点击
     */
    void onSwitchRecordBtnClked();

    /**
     * @brief onPhotoRecordBtnClked 拍照/录像按钮点击事件
     */
    void onPhotoRecordBtnClked();

    /**
     * @brief onShowCameraNameTimer 显示摄像机名称定时器
     */
    void onShowCameraNameTimer();

    /**
     * @brief onUpdateRecordState 更新录像状态
     * @param state  录像状态
     */
    void onUpdateRecordState(int state);
protected:

    /**
     * @brief onSleepWhenTaking 键盘按下事件
     * @param e
     */
//    void keyPressEvent(QKeyEvent *e) override;

    /**
     * @brief onSleepWhenTaking 键盘弹起事件
     * @param e
     */
//    void keyReleaseEvent(QKeyEvent *e) override;

    bool eventFilter(QObject *obj, QEvent *e)override;

private:
    /**
     * @brief showRightButtons 显示右侧控件
     */
    void showRightButtons();

    /**
     * @brief showWidget 显示/隐藏控件
     * @param widget 需要操作控件
     * @param bShow 是否显示
     */
    void showWidget(DWidget* widget, bool bShow);

    /**
     * @brief reflushSnapshotLabel 刷新缩略图控件的显示
     */
    void reflushSnapshotLabel();

public:
    static const int                minWindowWidth;//最小窗口宽度
    static const int                minWindowHeight;//最小窗口高度
//    static QString                  lastVdFileName;//上次打开的视频文件
//    static QString                  lastPicFileName;//上次打开的照片文件
private:
    enum ActType                    m_nActTpye;
    bool                            m_bWayland;
//    ThumbnailsBar                   *m_thumbnail;//缩略图
    videowidget                     *m_videoPre;//相机预览类
    DSettingsDialog                 *m_SetDialog;//设置页面
    DevNumMonitor                   *m_devnumMonitor;//设备数量监控
//    DButtonBox                      *pDButtonBox;//按钮盒
//    DButtonBoxButton                *m_pTitlePicBtn;//标题栏拍照按钮
//    DButtonBoxButton                *m_pTitleVdBtn;//标题栏视频按钮
//    DIconButton                     *m_pSelectBtn; //切换按钮
    DMenu                           *m_titlemenu;//标题栏菜单
//    QMenu                           *m_rightbtnmenu;//右键菜单
//    QAction                         *m_actOpenfolder;//打开文件
    QFileSystemWatcher              m_fileWatcher;//文件监控
    QFileSystemWatcher              m_fileWatcherUp;//文件夹监控，当前视频、照片文件夹的上级路径监控
    QString                         m_strCfgPath;//配置文件路径
    QAction                         *m_actionSettings;//打开设置页面
    QDBusReply<QDBusUnixFileDescriptor> m_reply;
    QDBusInterface                  *m_pLoginManager;
    QList<QVariant>                 m_arg;
    QDBusReply<QDBusUnixFileDescriptor> m_replySleep;
    QDBusInterface                  *m_pLoginMgrSleep;
    QList<QVariant>                 m_argSleep;
    QDBusInterface                  *m_pDBus;//接收休眠信号，仅wayland使用
    qint64                          m_SpaceKeyInterval;//空格按键时间间隔

    Titlebar                        *m_pTitlebar;           //标题栏

    //右侧按钮
    SwitchCameraBtn                 *m_cameraSwitchBtn;     //摄像头切换按钮
    photoRecordBtn                  *m_photoRecordBtn;      //拍照，录像按钮
    DPushButton                     *m_switchPhotoBtn;      //活动按钮
    DPushButton                     *m_switchRecordBtn;     //活动按钮
    ImageItem                       *m_snapshotLabel;       //缩略图
    DLabel                          *m_labelCameraName;     //摄像机名称提示label
    QTimer                          *m_showCameraNameTimer; //显示摄像机名称Timer

    bool                            m_bPhotoing;            //是否正在拍照
    bool                            m_bRecording;           //是否正在录像
    bool                            m_bSwitchCameraShowEnable; //是否可显示摄像头切换
    QString                         m_videoPath;            //配置的视频路径
    QString                         m_picPath;              //配置图片路径
    QMultiMap<QDateTime, QString>   m_mapFile;              //缩略图图像 时间排序
    takePhotoSettingAreaWidget      *m_takePhotoSettingArea;//拍照设置界面
    bool                            m_bUIinit;              //UI初始化是否完成
};

#endif // MAINWINDOW_H
