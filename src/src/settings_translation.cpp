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

#include <DSettings>

void GenerateSettingTranslate()
{
    auto group_settings = QObject::tr("Settings");
    auto group_baseName = QObject::tr("Basic");
    auto group_base_savaName = QObject::tr("File location");
    auto base_save_datapathName = QObject::tr("Save to:");
    auto group_photosettingName = QObject::tr("Shoot");
    auto group_photosetting_photosnumberName = QObject::tr("Burst mode");
    auto group_photosetting_photosdelayName = QObject::tr("Self-timer delay");
    auto group_outsettingName = QObject::tr("Output");
    auto outsetting_audiosettingName = QObject::tr("Audio Setting");
    auto outsetting_audiosetting_soundreminderName = QObject::tr("Sound");
    auto outsetting_audiosetting_soundreminderTextName = QObject::tr("Sound");
    auto group_outsetting_outformatName = QObject::tr("Output format");
    auto outsetting_outformat_picformatName = QObject::tr("Photo:");
    auto outsetting_outformat_vidformatName = QObject::tr("Video:");
    auto outsetting_4pcs = QObject::tr("4 pcs");
    auto outsetting_10pcs = QObject::tr("10 pcs");
    auto outsetting_none = QObject::tr("None");
    auto outsetting_resolutionsettingName = QObject::tr("Resolution settings");
    auto outsetting_resolutionsetting_resolutionName = QObject::tr("Resolution:");
}
