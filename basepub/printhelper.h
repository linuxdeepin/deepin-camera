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

#ifndef PRINTHELPER_H
#define PRINTHELPER_H

#include <QObject>
#include "printoptionspage.h"

class PrintHelper : public QObject
{
    Q_OBJECT

public:
    explicit PrintHelper(QObject *parent = nullptr);

    static void showPrintDialog(const QStringList &paths, QWidget *parent = nullptr);

    static QSize adjustSize(PrintOptionsPage* optionsPage, QImage img, int resolution, const QSize & viewportSize);
    static QPoint adjustPosition(PrintOptionsPage* optionsPage, const QSize& imageSize, const QSize & viewportSize);
};

#endif // PRINTHELPER_H
