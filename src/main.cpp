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

#include "mainwindow.h"
#include "capplication.h"
#include "dbus_adpator.h"
#include "cameraconfig.h"
#include "acobjectlist.h"

extern "C"
{
#include <libimagevisualresult/visualresult.h>
}

#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>
#include <DApplicationSettings>

#include <QSharedMemory>
#include <QTime>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#ifndef DEEPIN_CAMERA_PRO
#include "config.h"
#else
#define VERSION ""
#endif

DWIDGET_USE_NAMESPACE



static bool runSingleInstance()
{
    QString userName = QDir::homePath().section("/", -1, -1);
    std::string path = ("/home/" + userName + "/.cache/deepin/deepin-camera/").toStdString();
    QDir tdir(path.c_str());
    if (!tdir.exists()) {
        tdir.mkpath(path.c_str());
    }

    path += "single";
    int fd = open(path.c_str(), O_WRONLY | O_CREAT, 0644);
    int flock = lockf(fd, F_TLOCK, 0);

    if (fd == -1) {
        qInfo() << strerror(errno);
        return false;
    }
    if (flock == -1) {
        qInfo() << strerror(errno);
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QAccessible::installFactory(accessibleFactory);

    //root login for this application
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }

    QTime time;
    time.start();
    QString lutDir = LUT_DIR;
    initFilters(lutDir.toStdString().c_str());
    qDebug() << QString("initFilters cost %1 ms").arg(time.elapsed());

    CApplication a(argc, argv);

    qApp->setObjectName("deepin-camera");
#ifndef __mips__
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
    // overwrite DApplication default value
    qApp->setAttribute(Qt::AA_ForceRasterWidgets, false);
#endif

    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
    qApp->setOrganizationName("deepin");
    //设置应用名称
    qApp->setApplicationName("deepin-camera");
    //加载翻译
    qApp->loadTranslator(QList<QLocale>() << QLocale::system());
    //设置程序名称
    qApp->setApplicationDisplayName(QObject::tr("Camera"));
    //设置产品名称
    qApp->setProductName(QObject::tr("Camera"));
    //日志
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    qInfo() << "LogFile:" << DLogManager::getlogFilePath();
    //版本
    qApp->setApplicationVersion(DApplication::buildVersion(VERSION));
    QIcon myIcon = QIcon::fromTheme("deepin-camera");
    qApp->setWindowIcon(myIcon);
    qApp->setProductIcon(myIcon);//08月21获悉已添加到系统，故更改为从系统获取

    //应用描述
    qApp->setApplicationDescription(QObject::tr("Camera is an image and video capture utility using your PC camera or webcam."));

    dc::Settings::get().init();

    DApplicationSettings saveTheme;

    if (!runSingleInstance()) {
        qDebug() << "another deepin camera instance has started";
        QDBusInterface iface("com.deepin.camera", QDir::separator(), "com.deepin.camera");
        if (iface.isValid()) {
            qDebug() << "deepin-camera raise";
            iface.asyncCall("Raise");
        }

        exit(0);
    }

    CMainWindow w;
    a.setMainWindow(&w);

    Dtk::Widget::moveToCenter(&w);
    //判断是否是平板环境
    if (CamApp->isPanelEnvironment())
        w.showMaximized();
    else
        w.setMinimumSize(CMainWindow::minWindowWidth, CMainWindow::minWindowHeight);

    w.show();
    w.loadAfterShow();

    //最小化后双击桌面恢复画面
    ApplicationAdaptor adaptor(&w);
    QDBusConnection::sessionBus().registerService("com.deepin.camera");
    QDBusConnection::sessionBus().registerObject(QDir::separator(), &w);

    return qApp->exec();
}
