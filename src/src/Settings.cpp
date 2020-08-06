/*
 * (c) 2017, Deepin Technology Co., Ltd. <support@deepin.org>
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
#include <QtCore>
#include <QtGui>
#include <QComboBox>

#include "Settings.h"
//#include "compositing_manager.h"
#include <qsettingbackend.h>

#include "camview.h"
#include "gviewv4l2core.h"

namespace dc {
using namespace Dtk::Core;
static Settings *_theSettings = nullptr;

Settings &Settings::get()
{
    if (!_theSettings) {
        _theSettings = new Settings;
    }

    return *_theSettings;
}

Settings::Settings()
    : QObject(0)
{
    _configPath = QString("%1/%2/%3/config.conf")
                  .arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
                  .arg(qApp->organizationName())
                  .arg(qApp->applicationName());
    qDebug() << "configPath" << _configPath;
    auto backend = new QSettingBackend(_configPath);
#if defined (__mips__) || defined (__sw_64__) || defined ( __aarch64__)
    /*if (!CompositingManager::get().composited()) {
        _settings = DSettings::fromJsonFile(":/resource/settings.json");
    } else {
        _settings = DSettings::fromJsonFile(":/resource/settings.json");
    }*/
    _settings = DSettings::fromJsonFile(":/resource/settings.json");
#else
    _settings = DSettings::fromJsonFile(":/resource/settings.json");
#endif
    _settings->setBackend(backend);

    connect(_settings, &DSettings::valueChanged,
    [ = ](const QString & key, const QVariant & value) {
        if (key.startsWith("outsetting.resolutionsetting.resolution")) {
            auto mode_opt = Settings::get().settings()->option("outsetting.resolutionsetting.resolution");
            if (value >= 0) {
                QString mode = mode_opt->data("items").toStringList()[value.toInt()];
                emit resolutionchanged(mode);
            }
        }
    });

    qDebug() << "keys" << _settings->keys();
    auto resolutionmodeFamily = _settings->option("outsetting.resolutionsetting.resolution");

    QStringList Database = resolutionmodeFamily->data("items").toStringList();

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
        int defres = 0;
        if (format_index >= 0 && resolu_index >= 0) {//format_index = -1 &&resolu_index = -1 表示设备被占用或者不存在
            for (int i = 0 ; i < list_stream_formats[format_index].list_stream_cap[resolu_index].numb_frates; i++) {

                if ((list_stream_formats[format_index].list_stream_cap[i].width > 0
                        && list_stream_formats[format_index].list_stream_cap[i].height > 0) &&
                        (list_stream_formats[format_index].list_stream_cap[i].width < 7680
                         && list_stream_formats[format_index].list_stream_cap[i].height < 4320)) {
                    //加入分辨率的字符串
                    QString res_str = QString( "%1x%2").arg(list_stream_formats[format_index].list_stream_cap[i].width).arg(list_stream_formats[format_index].list_stream_cap[i].height);
                    resolutionDatabase.append(res_str);
                }
            }
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
                if (flag == 1) {
                    continue;
                }
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
            settings()->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        } else {
            resolutionDatabase.clear();
            resolutionDatabase.append(QString(tr("None")));
            settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
            resolutionmodeFamily->setData("items", resolutionDatabase);
        }
    } else {
        //初始化分辨率字符串表
        QStringList resolutionDatabase = resolutionmodeFamily->data("items").toStringList();
        if (resolutionDatabase.size() > 0) {
            resolutionmodeFamily->data("items").clear();
        }
        resolutionDatabase.clear();
        resolutionDatabase.append(QString(tr("None")));
        settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
        resolutionmodeFamily->setData("items", resolutionDatabase);
    }
    settings()->sync();
}
QVariant Settings::generalOption(const QString &opt)
{
    return settings()->getOption(QString("base.general.%1").arg(opt));
}

QVariant Settings::getOption(const QString &opt)
{
    return settings()->getOption(opt);
}

void Settings::setNewResolutionList()
{
    auto resolutionmodeFamily = _settings->option("outsetting.resolutionsetting.resolution");

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
        int defres = 0;
        if (format_index >= 0 && resolu_index >= 0) {
            for (int i = 0 ; i < list_stream_formats[format_index].list_stream_cap[resolu_index].numb_frates; i++) {

                if ((list_stream_formats[format_index].list_stream_cap[i].width > 0
                        && list_stream_formats[format_index].list_stream_cap[i].height > 0) &&
                        (list_stream_formats[format_index].list_stream_cap[i].width < 7680
                         && list_stream_formats[format_index].list_stream_cap[i].height < 4320)) {
                    //加入分辨率的字符串
                    QString res_str = QString( "%1x%2").arg(list_stream_formats[format_index].list_stream_cap[i].width).arg(list_stream_formats[format_index].list_stream_cap[i].height);
                    resolutionDatabase.append(res_str);
                }
            }
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
                if (flag == 1) {
                    continue;
                }
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
            settings()->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        } else {
            resolutionDatabase.clear();
            resolutionDatabase.append(QString(tr("None")));
            settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
            resolutionmodeFamily->setData("items", resolutionDatabase);
        }
    } else {
        //初始化分辨率字符串表
        QStringList resolutionDatabase = resolutionmodeFamily->data("items").toStringList();
        if (resolutionDatabase.size() > 0) {
            resolutionmodeFamily->data("items").clear();
        }
        resolutionDatabase.clear();
        resolutionDatabase.append(QString(tr("None")));
        settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
        resolutionmodeFamily->setData("items", resolutionDatabase);
    }
    settings()->sync();
}

void Settings::setPathOption(const QString &opt, const QVariant &v)
{
    settings()->setOption(QString("base.save.%1").arg(opt), v);
    settings()->sync();
}

}

