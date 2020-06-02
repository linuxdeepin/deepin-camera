/*
 * (c) 2020, Uniontech Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "actiontoken.h"
#include "avcodec.h"
#include "cameradetect.h"
#include "effectproxy.h"
#include "thumbnailsbar.h"
#include "toolbar.h"
#include "videoeffect.h"
#include "videowidget.h"
#include "widgetproxy.h"
#include "set_widget.h"

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
DWIDGET_USE_NAMESPACE
class QGridLayout;


//应用层界面通信站，与底层通信通过proxy代理类
class CMainWindow : public DMainWindow
{
    Q_OBJECT
public:
    CMainWindow(DWidget *w = nullptr);
    ~CMainWindow();

    void newPreViewByState(PRIVIEW_STATE state);
    void newNinePreview();
    void showPreviewByState(PRIVIEW_STATE state);
protected:
    void keyPressEvent(QKeyEvent *ev) override;

private:
    ThumbnailsBar   m_thumbnail;
    actionToken     m_actToken;
    avCodec         m_avCodec;
    CameraDetect    m_camDetect;
    effectproxy     m_effProxy;
    ToolBar         m_toolBar;
    videoEffect     m_videoEffect;
    videowidget     m_videoPre;
    widgetProxy     m_wgtProxy;

    QFileSystemWatcher m_fileWatcher;

    QVector<videowidget *> m_VEffectPreview;

    DTitlebar *dtitlebar;
    Set_Widget *m_setwidget;
 private:
    void initUI();
    void initConnection();
    void setupTitlebar();
    void menuItemInvoked(QAction *action);
    void settingsTriggered(bool bTrue);
    void resizeEvent(QResizeEvent *event);
private:

};

#endif // MAINWINDOW_H
