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

#include "Settings.h"
//#include "compositing_manager.h"
#include <qsettingbackend.h>

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

    qDebug() << "keys" << _settings->keys();
    //fontFamliy->setValue(0);
}

QVariant Settings::generalOption(const QString &opt)
{
    return settings()->getOption(QString("base.general.%1").arg(opt));
}

QVariant Settings::getOption(const QString &opt)
{
    return settings()->getOption(opt);
}

void Settings::setPathOption(const QString &opt, const QVariant &v)
{
    settings()->setOption(QString("base.save.%1").arg(opt), v);
    settings()->sync();
}

}

