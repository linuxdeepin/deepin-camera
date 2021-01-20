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
#ifndef CLOSEDIALOG_H
#define CLOSEDIALOG_H

#include <DDialog>
#include <DWidget>
#include <DLabel>
#include <DLineEdit>
#include <DPushButton>
#include <DSuggestButton>

#include <QVBoxLayout>
#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

typedef DLabel QLbtoDLabel;

/**
* @brief CloseDialog　关闭对话框
*/
class CloseDialog : public DDialog
{
    Q_OBJECT
public:
    CloseDialog(QWidget *parent = nullptr, QString strText = "", QString btnName1 = tr("Cancel"), QString btnName2 = tr("Close"));

private:
    QVBoxLayout  *m_vlayout;
    DWidget      *m_widget;
    QLbtoDLabel  *m_labtitle;
};

#endif // CLOSEDIALOG_H
