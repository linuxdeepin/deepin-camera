/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
* Maintainer:  shicetu <shicetu@uniontech.com>
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
#include "closedialog.h"
#include <DMessageBox>
#include <QRegExp>

#include <DLabel>
#include <QDebug>

DWIDGET_USE_NAMESPACE
typedef DLabel QLbtoDLabel;

CloseDialog::CloseDialog(QWidget *parent, QString strText, QString btnName1, QString btnName2)
    : DDialog(parent)
{
    this->setIcon(QIcon(":/images/icons/warning.svg"));
    setFixedSize(380, 140);
    widet = new DWidget(this);
    addContent(widet);
    m_vlayout = new QVBoxLayout(widet);
    m_hlayout = new QHBoxLayout();
    m_edtlayout = new QHBoxLayout();
    m_vlayout->setContentsMargins(2, 0, 2, 1);
    m_vlayout->addStretch();
    addButton(btnName1);
    addButton(btnName2);
    labtitle = new QLbtoDLabel();
    labtitle->setText(strText);
    labtitle->setAlignment(Qt::AlignCenter);
    m_vlayout->addWidget(labtitle);
    m_vlayout->addStretch();
    m_vlayout->addLayout(m_hlayout);
    widet->setLayout(m_vlayout);
}
