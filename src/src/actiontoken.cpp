/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
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

#include "actiontoken.h"
#include <QDebug>
#include "camview.h"

actionToken::actionToken(QObject *parent) : QObject(parent)
{
    is_active = 0;
    encode_thread = new encode_voice_Thread();
}

void actionToken::onTakePic()
{
    qDebug() << "onTakePic";
}

void actionToken::onThreeShots()
{
    qDebug() << "onThreeShots";
}

void actionToken::onTakeVideo()
{
    qDebug() << "onTakeVideo";

    if (is_active) {
        encode_thread->stop();
        is_active = 0;
        reset_video_timer();
    } else {
        encode_thread->start();
        is_active = 1;
    }
}



