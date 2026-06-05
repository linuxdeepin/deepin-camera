// Copyright (C) 2020 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GLOBALUTILS_H
#define GLOBALUTILS_H

#include <QString>

class GlobalUtils
{
public:
    // 判断是否为低性能主板
    static bool isLowPerformanceBoard();

    // 加载相机配置
    static void loadCameraConf();

private:
    static QStringList m_LowPerformanceBoards;
    static bool m_IsLowPerformanceDevice;
};

#endif // GLOBALUTILS_H
