// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

extern "C" {
#include "v4l2_formats.h"
}
#include "Settings.h"
#include "camview.h"
#include "gviewv4l2core.h"
#include "capplication.h"
#include "camera.h"
#include "globalutils.h"

#include <QtCore>
#include <QtGui>
#include <QComboBox>

namespace dc {
using namespace Dtk::Core;

Settings Settings::m_instance;
Settings &Settings::get()
{
    return m_instance;
}

Settings::Settings()
    : QObject(0),
      m_backend(nullptr),
      m_settings(nullptr)
{
    qDebug() << "Initializing Settings instance";
}

void Settings::init()
{
    qDebug() << "Initializing Settings";
    m_configPath.clear();
    m_configPath.append(QString("%1/%2/%3/config.conf")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                        .arg(qApp->organizationName())
                        .arg(qApp->applicationName()));
    qInfo() << "Loading config from path:" << m_configPath;
    m_backend = new QSettingBackend(m_configPath);

    if (CamApp->isPanelEnvironment()) {
        qDebug() << "Loading panel environment settings";
        m_settings = DSettings::fromJsonFile(":/resource/panel_settings.json");
    } else {
        qDebug() << "Loading normal environment settings";
        m_settings = DSettings::fromJsonFile(":/resource/settings.json");
    }

    QStringList videoFormatList;
    if (DataManager::instance()->encodeEnv() == FFmpeg_Env) {
        qDebug() << "Using FFmpeg encoding environment";
        if (DataManager::instance()->encExists()) {
            qDebug() << "FFmpeg encoder exists, loading camera configuration";
            GlobalUtils::loadCameraConf();
            if (!GlobalUtils::isLowPerformanceBoard()) {
                qDebug() << "High performance board detected, using mp4 and webm formats";
                videoFormatList << tr("mp4") << tr("webm");
            } else {
                qDebug() << "Low performance board detected, using webm and mp4 formats";
                videoFormatList << tr("webm") << tr("mp4");
            }
        } else {
            qDebug() << "FFmpeg encoder not found, using webm format only";
            videoFormatList << tr("webm");
            m_settings->setOption("outsetting.outformat.vidformat", 0);
        }
    } else {
        qDebug() << "Using non-FFmpeg encoding environment, using webm format only";
        videoFormatList << "webm";
        m_settings->setOption("outsetting.outformat.vidformat", 0);
    }
    m_settings->option("outsetting.outformat.vidformat")->setData("items", videoFormatList);

    m_settings->setBackend(m_backend);

    connect(m_settings, &DSettings::valueChanged, this, &Settings::onValueChanged);

    qInfo() << "Available settings keys:" << m_settings->keys();
    setNewResolutionList();
}

QVariant Settings::getOption(const QString &opt)
{
    qDebug() << "Getting option:" << opt;
    return m_settings->getOption(opt);
}

void Settings::setOption(const QString &opt, const QVariant &v)
{
    qDebug() << "Setting option:" << opt << "to value:" << v;
    m_settings->setOption(opt, v);
}

void Settings::setNewResolutionList()
{
    qDebug() << "Setting up new resolution list";
    auto resolutionmodeFamily = m_settings->option("outsetting.resolutionsetting.resolution");

    if (get_v4l2_device_handler() != nullptr) {
        qDebug() << "V4L2 device handler found, getting resolution list";
        //格式索引
        int format_index = v4l2core_get_frame_format_index(
                               get_v4l2_device_handler(),
                               v4l2core_get_requested_frame_format(get_v4l2_device_handler()));

        //分辨率索引
        int resolu_index = v4l2core_get_format_resolution_index(
                               get_v4l2_device_handler(),
                               format_index,
                               v4l2core_get_frame_width(get_v4l2_device_handler()),
                               v4l2core_get_frame_height(get_v4l2_device_handler()));

        qDebug() << "Format index:" << format_index << "Resolution index:" << resolu_index;

        //获取当前摄像头的格式表
        v4l2_stream_formats_t *list_stream_formats = v4l2core_get_formats_list(get_v4l2_device_handler());

        //初始化分辨率字符串表
        QStringList resolutionDatabase;
        resolutionDatabase.clear();

        //当前分辨率下标

        if (format_index >= 0 && resolu_index >= 0) {
            qDebug() << "Processing available resolutions";
            for (int i = 0 ; i < list_stream_formats[format_index].numb_res; i++) {
                int w = list_stream_formats[format_index].list_stream_cap[i].width;
                int h = list_stream_formats[format_index].list_stream_cap[i].height;
                if (is_valid_resolution(w, h)) {
                    //加入分辨率的字符串
                    QString res_str = QString("%1x%2").arg(w).arg(h);
                    //去重
                    if (resolutionDatabase.contains(res_str) == false) {
                        resolutionDatabase.append(res_str);
                        qDebug() << "Added resolution:" << res_str;
                    }
                }
            }

            // Sort resolutions
            qDebug() << "Sorting resolutions";
            int defres = 0;
            int tempostion = 0;
            int len = resolutionDatabase.size() - 1;
            for (int i = 0; i < resolutionDatabase.size() - 1; i++) {
                int flag = 1;
                for (int j = 0 ; j < len; j++) {
                    QStringList resolutiontemp1 = resolutionDatabase[j].split("x");
                    QStringList resolutiontemp2 = resolutionDatabase[j + 1].split("x");

                    if ((resolutiontemp1[0].toInt() <= resolutiontemp2[0].toInt())
                            || (resolutiontemp1[1].toInt() < resolutiontemp2[1].toInt())) {
                        QString resolutionstr = resolutionDatabase[j + 1];
                        resolutionDatabase[j + 1] = resolutionDatabase[j];
                        resolutionDatabase[j] = resolutionstr;
                        flag = 0;
                        tempostion = j;
                    }
                }
                len = tempostion;
                if (flag == 1)
                    continue;
            }

            // Find default resolution
            for (int i = 0; i < resolutionDatabase.size(); i++) {
                QStringList resolutiontemp = resolutionDatabase[i].split("x");
                if ((v4l2core_get_frame_width(get_v4l2_device_handler()) == resolutiontemp[0].toInt()) &&
                        (v4l2core_get_frame_height(get_v4l2_device_handler()) == resolutiontemp[1].toInt())) {
                    defres = i; //set selected resolution index
                    qDebug() << "Found default resolution at index:" << defres;
                    break;
                }
            }

            resolutionmodeFamily->setData("items", resolutionDatabase);

            //设置当前分辨率的索引
            m_settings->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        } else {
            qWarning() << "Invalid format or resolution index, setting default resolution";
            resolutionDatabase.clear();
            resolutionDatabase.append(QString(tr("None")));
            m_settings->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
            resolutionmodeFamily->setData("items", resolutionDatabase);
        }
    } else {
        //初始化分辨率字符串表
        qDebug() << "No V4L2 device handler, using QCamera resolutions";
        int defres = 0;
        QStringList resolutionDatabase = resolutionmodeFamily->data("items").toStringList();
        if (DataManager::instance()->encodeEnv() == QCamera_Env && !Camera::instance()->getSupportResolutions().isEmpty()) {
            qDebug() << "Using QCamera supported resolutions";
            resolutionDatabase = Camera::instance()->getSupportResolutions();

            for (int i = 0; i < resolutionDatabase.size(); i++) {
                QStringList resolutiontemp = resolutionDatabase[i].split("x");
                if ((Camera::instance()->getCameraResolution().width() == resolutiontemp[0].toInt()) &&
                        Camera::instance()->getCameraResolution().height() == resolutiontemp[1].toInt()) {
                    defres = i; //设置分辨率下拉菜单当前索引
                    qDebug() << "Found default QCamera resolution at index:" << defres;
                    break;
                }
            }
        } else {
            qDebug() << "No supported resolutions found, using default";
            if (resolutionDatabase.size() > 0)
                resolutionmodeFamily->data("items").clear();

            resolutionDatabase.clear();
            resolutionDatabase.append(QString(tr("None")));
        }
        m_settings->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        resolutionmodeFamily->setData("items", resolutionDatabase);
    }

    m_settings->sync();
    qDebug() << "Resolution list setup complete";
}

void Settings::setPathOption(const QString &opt, const QVariant &v)
{
    qDebug() << "Setting path option:" << opt << "to:" << v;
    m_settings->setOption(QString("base.save.%1").arg(opt), v);
    m_settings->sync();
}

void Settings::setBackOption(const QString &opt, const QVariant &v)
{
    qDebug() << "Setting backend option:" << opt << "to:" << v;
    m_backend->setOption(opt,v);
    m_backend->sync();
}

QVariant Settings::getBackOption(const QString &opt)
{
    qDebug() << "Getting backend option:" << opt;
    return m_backend->getOption(opt);
}

void Settings::onValueChanged(const QString & key, const QVariant & value)
{
    qDebug() << "Settings value changed:" << key << "to:" << value;
    if (key.startsWith("outsetting.resolutionsetting.resolution")) {
        auto mode_opt = m_settings->option("outsetting.resolutionsetting.resolution");
        if (value.toInt() >= 0 && mode_opt->data("items").toStringList().size() > value.toInt()) {
            QString mode = mode_opt->data("items").toStringList()[value.toInt()];
            qInfo() << "Resolution changed to:" << mode;
            emit resolutionchanged(mode);
        }
    }

    if (key.startsWith("base.photogrid.photogrids")) {
        auto grid_opt1 = m_settings->option("base.photogrid.photogrids");
        if (value.toInt() >= 0 && grid_opt1->data("items").toStringList().size() > value.toInt()) {
            qInfo() << "Grid type changed to:" << value.toInt();
            emit gridTypeChanged(value.toInt());
        }
    }

    if (key.startsWith("photosetting.photosdelay.photodelays")) {
        auto mode_opt1 = m_settings->option("photosetting.photosdelay.photodelays");
        if (value.toInt() >= 0 && mode_opt1->data("items").toStringList().size() > value.toInt()) {
            QString mode1 = mode_opt1->data("items").toStringList()[value.toInt()];
            qInfo() << "Delay time changed to:" << mode1;
            emit delayTimeChanged(mode1);
        }
    }

    if (key.startsWith("photosetting.mirrorMode.mirrorMode")){
        bool bMirror = m_settings->getOption("photosetting.mirrorMode.mirrorMode").toBool();
        qInfo() << "Mirror mode changed to:" << bMirror;
        emit mirrorModeChanged(bMirror);
    }

    if (key.startsWith("photosetting.Flashlight.Flashlight")){
        bool bLight = m_settings->getOption("photosetting.Flashlight.Flashlight").toBool();
        qInfo() << "Flashlight changed to:" << bLight;
        emit flashLightChanged(bLight);
    }

    if (key.startsWith("outsetting.outformat.vidformat")) {
        QPointer<DSettingsOption> formatOpt = m_settings->option("outsetting.outformat.vidformat");
        if (value.toInt() >= 0 && formatOpt->data("items").toStringList().size() > value.toInt()) {
            QString videoFormat = formatOpt->data("items").toStringList()[value.toInt()];
            qInfo() << "Video format changed to:" << videoFormat;
            emit videoFormatChanged(videoFormat);
        }
    }
}

Settings::~Settings()
{
    qDebug() << "Cleaning up Settings instance";
    if (m_backend) {
        delete m_backend;
        m_backend = nullptr;
    }
    if (m_settings) {
        delete m_settings;
        m_settings = nullptr;
    }
}
}

