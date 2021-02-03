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

#include "closedialog.h"
#include "ac-deepin-camera-define.h"

#include <DMessageBox>
#include <DLabel>

#include <QRegExp>
#include <QDebug>

DWIDGET_USE_NAMESPACE

typedef DLabel QLbtoDLabel;

CloseDialog::CloseDialog(QWidget *parent, QString strText)
    : DDialog(parent), m_leftNamebtn(tr("Cancel")), m_rightNamebtn(tr("Close"))
{
    this->setIcon(QIcon(":/images/icons/warning.svg"));
    setFixedSize(380, 140);
    m_widget = new DWidget(this);
    addContent(m_widget);
    m_vlayout = new QVBoxLayout(m_widget);
    m_vlayout->setContentsMargins(2, 0, 2, 1);
    m_vlayout->addStretch();
    addButton(m_leftNamebtn);
    addButton(m_rightNamebtn);

    QAbstractButton *cancelBtn = getButton(0);
    cancelBtn->setObjectName(CANCEL_BUTTON);
    cancelBtn->setAccessibleName(CANCEL_BUTTON);

    QAbstractButton *closeBtn = getButton(1);
    closeBtn->setObjectName(CLOSE_BUTTON);
    closeBtn->setAccessibleName(CLOSE_BUTTON);

    QPalette plt = closeBtn->palette();
    QColor clr(255, 87, 54);
    plt.setColor(QPalette::ButtonText, clr);
    closeBtn->setPalette(plt);

    m_labtitle = new QLbtoDLabel();
    m_labtitle->setText(strText);
    m_labtitle->setAlignment(Qt::AlignCenter);
    m_vlayout->addWidget(m_labtitle);
    m_vlayout->addStretch();
    m_widget->setLayout(m_vlayout);
}
