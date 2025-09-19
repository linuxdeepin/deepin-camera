// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "qtsingleapplication.h"
#include <QWidget>

/*!
    Creates a QtSingleApplication object. The application identifier
    will be QCoreApplication::applicationFilePath(). \a argc, \a
    argv, and \a GUIenabled are passed on to the QAppliation constructor.

    If you are creating a console application (i.e. setting \a
    GUIenabled to false), you may consider using
    QtSingleCoreApplication instead.
*/

QtSingleApplication::QtSingleApplication(int &argc, char **argv)
    : QObject(nullptr)
{
    // qDebug() << "Function started: QtSingleApplication constructor";
#if (DTK_VERSION < DTK_VERSION_CHECK(5, 4, 0, 0))
    _app = new DApplication(argc, argv);
#else
    _app = DApplication::globalApplication(argc, argv);
#endif
    // qDebug() << "Function completed: QtSingleApplication constructor";
}

DApplication *QtSingleApplication::dApplication()
{
    // qDebug() << "Entering dApplication";
    return _app;
}


QtSingleApplication::~QtSingleApplication()
{
    // qDebug() << "Function started: QtSingleApplication destructor";
    if (_app != nullptr) {
        // qDebug() << "QtSingleApplication destructor: Enter if branch (app exists, deleting)";
        _app->deleteLater();
        _app = nullptr;
    }
    // qDebug() << "Function completed: QtSingleApplication destructor";
}
