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

extern "C"
{
#include "camview.h"
}
#include "mainwindow.h"
#include "capplication.h"
#include "dbus_adpator.h"
#include <stdio.h>

#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>
#include <DApplicationSettings>

#include <QSharedMemory>

DWIDGET_USE_NAMESPACE
//判断是否采用wayland显示服务器
bool CheckWayland()
{
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        return true;
    } else {
        return false;
    }
}
int main(int argc, char *argv[])
{

    bool bWayland = CheckWayland();
    if (bWayland) {
        //默认走xdgv6,该库没有维护了，因此需要添加该代码
        qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGLES);
        format.setDefaultFormat(format);
        set_wayland_status(1);
    }
//    MyObject obj(argc,argv);

//    DApplication *app = MyObject::getDtkInstance();
//    app->installEventFilter(&obj);

    CApplication a(argc, argv);

    a.setObjectName("deepin-camera");
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    // overwrite DApplication default value
    a.setAttribute(Qt::AA_ForceRasterWidgets, false);


    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setOrganizationName("deepin");
    a.setApplicationName("deepin-camera");
    //加载翻译
    a.loadTranslator(QList<QLocale>() << QLocale::system());
    a.setApplicationDisplayName(QObject::tr("Camera"));
    a.setProductName(QObject::tr("Camera"));
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    qDebug() << "LogFile:" << DLogManager::getlogFilePath();

    QString t_date = QDate::currentDate().toString("MMdd");
    // Version Time
    a.setApplicationVersion(DApplication::buildVersion(t_date));
    a.setWindowIcon(QIcon::fromTheme("deepin-camera"));
    a.setProductIcon(QIcon::fromTheme("deepin-camera"));//08月21获悉已添加到系统，故更改为从系统获取
    //a.setProductIcon(QIcon(":/images/logo/deepin-camera-96px.svg")); //用于显示关于窗口的应用图标

    a.setApplicationDescription(QObject::tr("Camera is an image and video capture utility using your PC camera or webcam."));

    DApplicationSettings saveTheme;

    //一个用户仅允许打开一个相机
    QString userpath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QSharedMemory shared_memory(userpath + "deepincamera");

    if (shared_memory.attach()) {
        shared_memory.detach();
    }

    if (!shared_memory.create(1)) {
        qDebug() << "another deepin camera instance has started";
        QDBusInterface iface("com.deepin.camera", "/", "com.deepin.camera");
                if (iface.isValid()) {
                     qWarning() << "deepin-camera raise";
                    iface.asyncCall("Raise");
                }
        exit(0);
    }

    CMainWindow w;

//    DVtableHook::overrideVfptrFun(app, &DApplication::handleQuitAction, w, &CMainWindow::handleQuitAction);
//    bool is = DVtableHook::getDestructFunIndex(&app,&CMainWindow::handleQuitAction);
//    if(is){
//        quintptr *obj = DVtableHook::getVtableOfObject(app);
//    }
    w.setWayland(bWayland);
    w.setMinimumSize(MinWindowWidth, MinWindowHeight);
    w.show();
    w.loadAfterShow();

    //最小化后双击桌面恢复画面
    ApplicationAdaptor adaptor(&w);
    QDBusConnection::sessionBus().registerService("com.deepin.camera");
    QDBusConnection::sessionBus().registerObject("/", &w);

    Dtk::Widget::moveToCenter(&w);

    return a.exec();
}
