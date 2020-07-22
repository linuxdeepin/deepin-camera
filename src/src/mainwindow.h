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

#include <QObject>
#include <DMainWindow>
#include <QPushButton>
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
#include <DTitlebar>
#include <DSettingsDialog>

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

    ~CMainWindow() override;
private:
    void initUI();
    void initTitleBar();
    void initConnection();
    void setupTitlebar();
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
    void menuItemInvoked(QAction *action);
    void settingsTriggered(bool bTrue);
    //void keyPressEvent(QKeyEvent *ev);
    void slotPopupSettingsDialog();
private slots:
    void setSelBtnHide();
    void setSelBtnShow();
    void onFitToolBar();
    void onEnableTitleBar(int nType);
    void onTitlePicBtn();
    void onTitleVdBtn();
    void onSettingsDlgClose();
    void onEnableSettings(bool bTrue);
    void onTakePicDone();
    void onTakeVdCancel();
    void onThemeChange(DGuiApplicationHelper::ColorType type);
protected:
    void keyPressEvent(QKeyEvent *e) override;
    void keyReleaseEvent(QKeyEvent *e) override;
private:
    ThumbnailsBar              *m_thumbnail;
    DMenu                     *menu;
    videowidget                  m_videoPre;
    CloseDialog                  *m_closeDlg;
    QFileSystemWatcher           m_fileWatcher;
    DevNumMonitor              *m_devnumMonitor;
    QVector<videowidget *>       m_VEffectPreview;
    DButtonBox                  *pDButtonBox;
    DButtonBoxButton            *m_pTitlePicBtn;
    DButtonBoxButton            *m_pTitleVdBtn;
    DIconButton                 *pSelectBtn; //切换按钮
    QString                      m_strCfgPath;
    QAction                     *m_actionSettings;
    int                          m_nActTpye;
    static QString                m_lastfilename;
};

#endif // MAINWINDOW_H
