// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GLOBALUTILS_H
#define GLOBALUTILS_H

#include <QString>
#include <QStringList>
#include <QVariant>

class GlobalUtils
{
public:
    // 判断是否为低性能主板
    static bool isLowPerformanceBoard();

    // 加载相机配置
    static void loadCameraConf();

    static QStringList parseStringListConfig(const QVariant &value);

private:
    static QStringList m_LowPerformanceBoards;
};

#endif // GLOBALUTILS_H
