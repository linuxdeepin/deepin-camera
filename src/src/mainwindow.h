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
const int MinWindowWidth = 800;
const int MinWindowHeight = 583;

//应用层界面通信站，与底层通信通过proxy代理类
class CMainWindow : public DMainWindow
{
    Q_OBJECT
public:
    CMainWindow(DWidget *w = nullptr);
    void newPreViewByState(PRIVIEW_STATE state);
    void newNinePreview();
    static QString lastOpenedPath();
    DSettings *getDsetMber();
    void setWayland(bool bTrue){m_bWayland = bTrue;}
    /**
    * @brief getPicBoxBtn　获取标题拍照按钮
    */
    DButtonBoxButton* getPicBoxBtn()
    {
        return m_pTitlePicBtn;
    }
    /**
    * @brief getVdBoxBtn　获取标题录像按钮
    */
    DButtonBoxButton* getVdBoxBtn()
    {
        return m_pTitleVdBtn;
    }
    /**
    * @brief getThubBtn　获取缩略图后面的拍照/录像按钮，由标题栏确定类型
    */
    DPushButton* getThubBtn()
    {
        return m_thumbnail->getPushbtn();
    }
    /**
    * @brief getChangeCam　获取相机切换按钮，当有多个相机连接时，按钮可进行相机切换；若没有相机连接或只有一个相机时，按钮隐藏
    */
    DIconButton* getChangeCam()
    {
        return pSelectBtn;
    }
    /**
    * @brief getEndBtn　获取结束录制视频按钮
    */
    DPushButton* getEndBtn()
    {
        return m_videoPre->getEndBtn();
    }
    /**
    * @brief getActionsSettings　获取设置按钮
    */
    QAction *getActionsSettings()
    {
        return m_actionSettings;
    }

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
    //void keyPressEvent(QKeyEvent *ev);
    /**
    * @brief slotPopupSettingsDialog 设置界面初始化
    */
    void slotPopupSettingsDialog();
    /**
     * @brief initBlockShutdown 阻塞关机
     */
    void initBlockShutdown();
    /**
     * @brief initBlockSleep 阻塞睡眠
     */
    void initBlockSleep();

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
    void onTakePicOnce();//多连拍的时候用
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
private:
    ThumbnailsBar              *m_thumbnail = nullptr;
    DMenu                      *m_titlemenu;
    QMenu                      *m_rightbtnmenu;
    QAction                     *actOpenfolder;
    videowidget                  *m_videoPre;
    QFileSystemWatcher           m_fileWatcher;
    DevNumMonitor              *m_devnumMonitor;
//    QVector<videowidget *>       m_VEffectPreview;
    DButtonBox                  *pDButtonBox;
    DButtonBoxButton            *m_pTitlePicBtn;
    DButtonBoxButton            *m_pTitleVdBtn;
    DIconButton                 *pSelectBtn; //切换按钮
    QString                      m_strCfgPath;
    QAction                     *m_actionSettings;
    int                          m_nActTpye;
    static QString               m_lastfilename;

    QDBusReply<QDBusUnixFileDescriptor> m_reply;
    QDBusInterface *m_pLoginManager = nullptr;
    QList<QVariant> m_arg;

    QDBusReply<QDBusUnixFileDescriptor> m_replySleep;
    QDBusInterface *m_pLoginMgrSleep = nullptr;
    QList<QVariant> m_argSleep;

    bool                        m_bWayland;

    QDBusInterface              *m_pDBus = nullptr;//接收休眠信号，仅wayland使用
};

#endif // MAINWINDOW_H
