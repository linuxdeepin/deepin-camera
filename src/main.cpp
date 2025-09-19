// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C" {
#include "camview.h"
}
#include "mainwindow.h"
#include "capplication.h"
#include "cameraconfig.h"
#include "acobjectlist.h"
#include "dbus_adpator.h"

#ifdef DTKCORE_CLASS_DConfigFile
#include <DConfig>
#endif

extern "C" {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <libimagevisualresult6/visualresult.h>
#else
    #include <libimagevisualresult/visualresult.h>
#endif
}

#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>
#if QT_VERSION_MAJOR <= 5
#include <DApplicationSettings>
#endif

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

//判断是否采用wayland显示服务器
static bool CheckWayland()
{
    qDebug() << "Function started: CheckWayland";
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        qDebug() << "Condition check: Wayland environment detected";
        return true;
    }
    qDebug() << "Condition check: Non-Wayland environment detected";
    return false;
}

static bool CheckFFmpegEnv()
{
    qDebug() << "Function started: CheckFFmpegEnv";
    QDir dir;
    QString path = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
    dir.setPath(path);
    QStringList list = dir.entryList(QStringList() << (QString("libavcodec") + "*"), QDir::NoDotAndDotDot | QDir::Files);
    QString libName = "libavcodec.so"; // set default name for load if not find in LibrariesPath
#if QT_VERSION_MAJOR > 5
    QRegularExpression re("libavcodec.so.*");   //Sometimes libavcodec.so may not exist, so find it through regular expression.
    for (int i = 0; i < list.count(); i++) {
        QRegularExpressionMatch match = re.match(list[i]);
        if (match.hasMatch()) {
            libName = list[i];
            qDebug() << "Found matching library:" << libName;
            break;
        }
    }
#else
    QRegExp re("libavcodec.so.*");   //Sometimes libavcodec.so may not exist, so find it through regular expression.
    for (int i = 0; i < list.count(); i++) {
        if (re.exactMatch(list[i])) {
            libName = list[i];
            qDebug() << "Found matching library:" << libName;
            break;
        }
    }
#endif

    QLibrary libavcodec;   //检查编码器是否存在
    libavcodec.setFileName(libName);
    if (!libavcodec.load()) {
        qDebug() << "Condition check: Library loading failed";
        qWarning() << QString("Not found libavcodec: %1").arg(libavcodec.errorString());
        return false;
    }
    qDebug() << "The libavcodec is loaded successfully.";
    typedef AVCodec *(*p_avcodec_find_encoder)(enum AVCodecID id);
    p_avcodec_find_encoder m_avcodec_find_encoder = nullptr;
    m_avcodec_find_encoder = reinterpret_cast<p_avcodec_find_encoder>(libavcodec.resolve("avcodec_find_encoder"));

    AVCodec *pCodec = nullptr;
    if (m_avcodec_find_encoder)
        pCodec = m_avcodec_find_encoder(AV_CODEC_ID_H264);

    if (pCodec) {
        qDebug() << "Video encoder exists. AVCodecID:" << AV_CODEC_ID_H264;
        DataManager::instance()->setEncExists(true);
    } else {
        qWarning() << "Can not find output video encoder! AVCodecID:" << AV_CODEC_ID_H264;
        DataManager::instance()->setEncExists(false);
    }

    qDebug() << "Function completed: CheckFFmpegEnv";
    return true;
}

int main(int argc, char *argv[])
{
    qInfo() << "Starting deepin-camera application...";
    
    // Task 326583 不参与合成器崩溃重连
    unsetenv("QT_WAYLAND_RECONNECT");

    QAccessible::installFactory(accessibleFactory);
    bool bWayland = CheckWayland();
    qInfo() << "Wayland status:" << (bWayland ? "enabled" : "disabled");
    
    bool bFFmpegEnv = CheckFFmpegEnv();
    if (argc > 1) {
        qDebug() << "Condition check: Arguments found";
        if (QString(argv[1]) == "-g") {
            bFFmpegEnv = false;
            qDebug() << "Running in gstreamer test environment";
        }
    }
    DataManager::instance()->setEncodeEnv(bFFmpegEnv ? FFmpeg_Env : GStreamer_Env);
    qInfo() << "Encoding environment:" << (bFFmpegEnv ? "FFmpeg" : "GStreamer");

    //root login for this application
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
        qDebug() << "Set XDG_CURRENT_DESKTOP to Deepin";
    }

    if (bWayland) {
        qDebug() << "Condition check: Wayland configuration required";
        QSurfaceFormat format;
        format.setRenderableType(QSurfaceFormat::OpenGLES);
        format.setDefaultFormat(format);
        set_wayland_status(1);
        qDebug() << "Configured OpenGLES for Wayland";

        int mp4Encode = -1;
#ifdef DTKCORE_CLASS_DConfigFile
        //需要查询是否对不同机型设置MP4编码缓存特殊处理
        DConfig *dconfig = DConfig::create("org.deepin.camera", "org.deepin.camera.encode");
        if (dconfig && dconfig->isValid() && dconfig->keyList().contains("mp4EncodeMode")) {
            mp4Encode = dconfig->value("mp4EncodeMode").toInt();
            set_pugx_status(mp4Encode);
            qDebug() << "Loaded MP4 encode mode from DConfig:" << mp4Encode;
        }
#endif
        qInfo() << "mp4EncodeMode value is:" << get_pugx_status();
        if (mp4Encode == -1) {
            qDebug() << "Condition check: MP4 encode mode detection required";
            QStringList options;
            options << QString(QStringLiteral("-c"));
            options << QString(QStringLiteral("dmidecode -t 11 | grep 'String 4' | awk '{print $NF}' && "
                                              "dmidecode -s system-product-name|awk '{print $NF}'"));
            QProcess process;
            process.start(QString(QStringLiteral("bash")), options);
            process.waitForFinished();
            process.waitForReadyRead();
            QByteArray tempArray = process.readAllStandardOutput();
            char *charTemp = tempArray.data();
            QString str_output = QString(QLatin1String(charTemp));
            process.close();

            if (str_output.contains("PGUX", Qt::CaseInsensitive)) {
                qDebug() << "Condition check: PGUX system detected";
                mp4Encode = 1;
                qDebug() << "Detected PGUX system";
            } else {
                mp4Encode = 0;
            }
            qInfo() << "Process find mp4EncodeMode value is:" << get_pugx_status();
        }

        set_pugx_status(mp4Encode);
        qInfo() << "Final mp4EncodeMode value is:" << get_pugx_status();
    }

#if QT_VERSION_MAJOR > 5
    QElapsedTimer time;
#else
    QTime time;
#endif
    time.start();
    QString lutDir = LUT_DIR;
    initFilters(lutDir.toStdString().c_str());
    qDebug() << QString("Filter initialization completed in %1 ms").arg(time.elapsed());

    CApplication a(argc, argv);

    qApp->setObjectName("deepin-camera");
#ifndef __mips__
    qApp->setAttribute(Qt::AA_UseHighDpiPixmaps);
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
    qInfo() << "Log system initialized. Log file:" << DLogManager::getlogFilePath();
    //版本
    qApp->setApplicationVersion(DApplication::buildVersion(VERSION));
    QIcon myIcon = QIcon::fromTheme("deepin-camera");
    qApp->setWindowIcon(myIcon);
    qApp->setProductIcon(myIcon);//08月21获悉已添加到系统，故更改为从系统获取
    qDebug() << "Application version:" << DApplication::buildVersion(VERSION);

    //应用描述
    qApp->setApplicationDescription(QObject::tr("Camera is an image and video capture utility using your PC camera or webcam."));

    dc::Settings::get().init();
    qDebug() << "Application settings initialized";

#if QT_VERSION_MAJOR <= 5
    DApplicationSettings saveTheme;
#endif

    if (!qApp->setSingleInstance("deepin-camera")) {
        qWarning() << "Another deepin camera instance is already running";
        QDBusInterface iface("com.deepin.camera", QDir::separator(), "com.deepin.camera");
        if (iface.isValid()) {
            qDebug() << "Attempting to raise existing instance";
            iface.asyncCall("Raise");
        }

        qDebug() << "Function completed: main (exit due to existing instance)";
        exit(0);
    }

    CMainWindow w;
    a.setMainWindow(&w);
    qDebug() << "Main window created and set";

    Dtk::Widget::moveToCenter(&w);
    w.setWayland(bWayland);
    //判断是否是平板环境
    if (CamApp->isPanelEnvironment()) {
        w.showMaximized();
        qDebug() << "Running in panel environment, showing maximized window";
    } else {
        w.setMinimumSize(CMainWindow::minWindowWidth, CMainWindow::minWindowHeight);
        qDebug() << "Running in desktop environment, showing normal window";
    }

    w.show();
    w.loadAfterShow();
    qInfo() << "Main window displayed and initialized";

    ApplicationAdaptor adaptor(&w);
    QDBusConnection::sessionBus().registerService("com.deepin.camera");
    QDBusConnection::sessionBus().registerObject(QDir::separator(), &w);
    qDebug() << "DBus service and object registered";

    qInfo() << "Application startup completed successfully";
    return qApp->exec();
}
