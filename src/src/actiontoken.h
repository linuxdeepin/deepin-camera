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

#ifndef ACTIONTOKEN_H
#define ACTIONTOKEN_H

#include <QObject>
#include "encode_voice.h"

//sct
//拍照 录像 连拍 按钮联动 具体功能实现
class actionToken : public QObject
{
    Q_OBJECT
public:
    explicit actionToken(QObject *parent = nullptr);

signals:

public slots:
    void onTakePic();
    void onThreeShots();
    void onTakeVideo();

private:
    int is_active;
    encode_voice_Thread *encode_thread;

};

#endif // ACTIONTOKEN_H
