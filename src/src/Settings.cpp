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

#include "Settings.h"
#include "camview.h"
#include "gviewv4l2core.h"
#include "capplication.h"

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

}

void Settings::init()
{
    m_configPath.clear();
    m_configPath.append(QString("%1/%2/%3/config.conf")
                        .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                        .arg(qApp->organizationName())
                        .arg(qApp->applicationName()));
    qInfo() << "configPath" << m_configPath;
    m_backend = new QSettingBackend(m_configPath);

    if (CamApp->isPanelEnvironment())
        m_settings = DSettings::fromJsonFile(":/resource/panel_settings.json");
    else
        m_settings = DSettings::fromJsonFile(":/resource/settings.json");

    m_settings->setBackend(m_backend);

    connect(m_settings, &DSettings::valueChanged, this, &Settings::onValueChanged);

    qInfo() << "keys" << m_settings->keys();
    setNewResolutionList();
}

QVariant Settings::getOption(const QString &opt)
{
    return m_settings->getOption(opt);
}

void Settings::setOption(const QString &opt, const QVariant &v)
{
    m_settings->setOption(opt, v);
}

void Settings::setNewResolutionList()
{
    auto resolutionmodeFamily = m_settings->option("outsetting.resolutionsetting.resolution");

    if (get_v4l2_device_handler() != nullptr) {
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

        //获取当前摄像头的格式表
        v4l2_stream_formats_t *list_stream_formats = v4l2core_get_formats_list(get_v4l2_device_handler());

        //初始化分辨率字符串表
        QStringList resolutionDatabase;
        resolutionDatabase.clear();

        //当前分辨率下标

        if (format_index >= 0 && resolu_index >= 0) {
            for (int i = 0 ; i < list_stream_formats[format_index].numb_res; i++) {
                if ((list_stream_formats[format_index].list_stream_cap[i].width > 0
                        && list_stream_formats[format_index].list_stream_cap[i].height > 0) &&
                        (list_stream_formats[format_index].list_stream_cap[i].width < 7680
                         && list_stream_formats[format_index].list_stream_cap[i].height < 4320) &&
                        ((list_stream_formats[format_index].list_stream_cap[i].width % 8) == 0
                         && (list_stream_formats[format_index].list_stream_cap[i].width % 16) == 0
                         && (list_stream_formats[format_index].list_stream_cap[i].height % 8) ==  0)) {
                    //加入分辨率的字符串
                    QString res_str = QString("%1x%2").arg(list_stream_formats[format_index].list_stream_cap[i].width).arg(list_stream_formats[format_index].list_stream_cap[i].height);
                    //去重
                    if (resolutionDatabase.contains(res_str) == false)
                        resolutionDatabase.append(res_str);

                }
            }
            int defres = 0;
            int tempostion = 0;
            int len = resolutionDatabase.size() - 1;
            for (int i = 0; i < resolutionDatabase.size() - 1; i++) {
                int flag = 1;
                for (int j = 0 ; j < len; j++) {
                    QStringList resolutiontemp1 = resolutionDatabase[j].split("x");
                    QStringList resolutiontemp2 = resolutionDatabase[j + 1].split("x");

                    if ((resolutiontemp1[0].toInt() <= resolutiontemp2[0].toInt())
                            && (resolutiontemp1[1].toInt() < resolutiontemp2[1].toInt())) {
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

            for (int i = 0; i < resolutionDatabase.size(); i++) {
                QStringList resolutiontemp = resolutionDatabase[i].split("x");
                if ((v4l2core_get_frame_width(get_v4l2_device_handler()) == resolutiontemp[0].toInt()) &&
                        (v4l2core_get_frame_height(get_v4l2_device_handler()) == resolutiontemp[1].toInt())) {
                    defres = i; //set selected resolution index
                    break;
                }
            }

            resolutionmodeFamily->setData("items", resolutionDatabase);

            //设置当前分辨率的索引
            m_settings->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        } else {
            resolutionDatabase.clear();
            resolutionDatabase.append(QString(tr("None")));
            m_settings->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
            resolutionmodeFamily->setData("items", resolutionDatabase);
        }
    } else {
        //初始化分辨率字符串表
        QStringList resolutionDatabase = resolutionmodeFamily->data("items").toStringList();

        if (resolutionDatabase.size() > 0)
            resolutionmodeFamily->data("items").clear();

        resolutionDatabase.clear();
        resolutionDatabase.append(QString(tr("None")));
        m_settings->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
        resolutionmodeFamily->setData("items", resolutionDatabase);
    }

    m_settings->sync();
}

void Settings::setPathOption(const QString &opt, const QVariant &v)
{
    m_settings->setOption(QString("base.save.%1").arg(opt), v);
    m_settings->sync();
}

void Settings::setBackOption(const QString &opt, const QVariant &v)
{
    m_backend->setOption(opt,v);
    m_backend->sync();
}

QVariant Settings::getBackOption(const QString &opt)
{
    return m_backend->getOption(opt);
}

void Settings::onValueChanged(const QString & key, const QVariant & value)
{
    if (key.startsWith("outsetting.resolutionsetting.resolution")) {
        auto mode_opt = m_settings->option("outsetting.resolutionsetting.resolution");
        if (value >= 0 && mode_opt->data("items").toStringList().size() > value.toInt()) {
            QString mode = mode_opt->data("items").toStringList()[value.toInt()];
            emit resolutionchanged(mode);
        }
    }

    if (key.startsWith("photosetting.photosdelay.photodelays")) {
        auto mode_opt1 = m_settings->option("photosetting.photosdelay.photodelays");
        if (value >= 0 && mode_opt1->data("items").toStringList().size() > value.toInt()) {
            QString mode1 = mode_opt1->data("items").toStringList()[value.toInt()];
            emit delayTimeChanged(mode1);
        }
    }

    if (key.startsWith("photosetting.mirrorMode.mirrorMode")){
        bool bMirror = m_settings->getOption("photosetting.mirrorMode.mirrorMode").toBool();
        emit mirrorModeChanged(bMirror);
    }

    if (key.startsWith("photosetting.Flashlight.Flashlight")){
        bool bLight = m_settings->getOption("photosetting.Flashlight.Flashlight").toBool();
        emit flashLightChanged(bLight);
    }
}

Settings::~Settings()
{
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

