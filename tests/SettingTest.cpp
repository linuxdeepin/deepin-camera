// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SettingTest.h"
#include "src/Settings.h"
#include <gtest/gtest.h>

void InitSetting()
{
    dc::Settings::get().setOption(QString("base.photogrid.photogrids"), 0);
    dc::Settings::get().setOption(QString("photosetting.photosnumber.takephotos"), 0);
    dc::Settings::get().setOption(QString("photosetting.photosdelay.photodelays"), 0);
    dc::Settings::get().setOption(QString("photosetting.audiosetting.soundswitchbtn"), true);
    dc::Settings::get().setOption(QString("photosetting.mirrorMode.mirrorMode"), true);
    dc::Settings::get().setOption(QString("photosetting.Flashlight.Flashlight"), false);
    dc::Settings::get().setOption(QString("outsetting.resolutionsetting.resolution"), " ");
    dc::Settings::get().setPathOption("picdatapath","~/Pictures/Camera");
    dc::Settings::get().setBackOption("device","/dev/video0");
    dc::Settings::get().settings()->sync();
}


/**
 *  @brief 默认设置检查
 */
TEST(Setting, defaultSetting)
{
    int gridType = dc::Settings::get().getOption(QString("base.photogrid.photogrids")).toInt();
    int defaultTakePhotos = dc::Settings::get().getOption(QString("photosetting.photosnumber.takephotos")).toInt();
    int defaultPhotoDelays = dc::Settings::get().getOption(QString("photosetting.photosdelay.photodelays")).toInt();
    bool defaultAudioSetting = dc::Settings::get().getOption(QString("photosetting.audiosetting.soundswitchbtn")).toBool();
    bool defaultMirrorMode = dc::Settings::get().getOption(QString("photosetting.mirrorMode.mirrorMode")).toBool();
    bool defaultFlashlight = dc::Settings::get().getOption(QString("photosetting.Flashlight.Flashlight")).toBool();
    QString defaultDevice = dc::Settings::get().getBackOption("device").toString();
    //EXPECT_STREQ("/dev/video0", defaultDevice.toStdString().c_str());
    EXPECT_EQ(0, gridType);
    EXPECT_EQ(0, defaultTakePhotos);
    EXPECT_EQ(0, defaultPhotoDelays);
    EXPECT_EQ(true, defaultAudioSetting);
    EXPECT_EQ(true, defaultMirrorMode);
    EXPECT_EQ(false, defaultFlashlight);
}

TEST(Setting, option)
{
    dc::Settings::get().setPathOption("picdatapath","~/Pictures/Camera");
    dc::Settings::get().setBackOption("device","/dev/video0");
    dc::Settings::get().getBackOption("device").toString();
}

