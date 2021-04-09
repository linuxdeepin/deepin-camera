/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
*
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
//#include <DApplication>
#include "capplication.h"
#include <DMainWindow>
#include <DWidgetUtil>
#include <QSharedMemory>
#include <DLog>
#include <DApplicationSettings>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "dbus_adpator.h"
extern "C"
{
#include "camview.h"
}
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

static bool CheckWayland()
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
        set_wayland_status(1);
    }

    CApplication a(argc, argv);
    //加载翻译

//    QTranslator *translator = new QTranslator;

//    bool bLoaded = translator->load("deepin-camera.qm", ":/translations");
//    if (!bLoaded) {
//        qInfo() << "load transfile error";
//    }

//    a.installTranslator(translator);

    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);

    qApp->setOrganizationName("deepin");
    qApp->setApplicationName("deepin-camera");
    qApp->loadTranslator(QList<QLocale>() << QLocale::system());

    qApp->setProductName(QObject::tr("Camera"));

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();
    qInfo() << "LogFile:" << DLogManager::getlogFilePath();

    qApp->setApplicationDisplayName(QObject::tr("Camera"));
    //static const QDate buildDate = QLocale( QLocale::English ).toDate( QString(QDate::currentDate()).replace("  ", " 0"), "MMM dd yyyy");
    QString t_date = QDate::currentDate().toString("MMdd");
    // Version Time
    qApp->setApplicationVersion(DApplication::buildVersion(t_date));
    qApp->setWindowIcon(QIcon::fromTheme("deepin-camera"));
    qApp->setProductIcon(QIcon::fromTheme("deepin-camera"));//08月21获悉已添加到系统，故更改为从系统获取
    //a.setProductIcon(QIcon(":/images/logo/deepin-camera-96px.svg")); //用于显示关于窗口的应用图标


    qApp->setApplicationDescription(QObject::tr("Camera is an image and video capture utility using your PC camera or webcam."));
    DApplicationSettings saveTheme;

    if (!runSingleInstance()) {
        qInfo() << "another deepin camera instance has started";
        QDBusInterface iface("com.deepin.camera", "/", "com.deepin.camera");
        if (iface.isValid()) {
            qWarning() << "deepin-camera raise";
            iface.asyncCall("Raise");
        }
        exit(0);
    }

    CMainWindow w;
    w.setWayland(bWayland);
    w.setMinimumSize(MinWindowWidth, MinWindowHeight);
    w.show();

    ApplicationAdaptor adaptor(&w);
    QDBusConnection::sessionBus().registerService("com.deepin.camera");
    QDBusConnection::sessionBus().registerObject("/", &w);

    Dtk::Widget::moveToCenter(&w);

    return qApp->exec();
}
