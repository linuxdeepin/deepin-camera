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

#ifndef PRINTOPTIONSPAGE_H
#define PRINTOPTIONSPAGE_H

#include <DDialog>
#include <QRadioButton>
#include <DDoubleSpinBox>
#include <QButtonGroup>
#include <DComboBox>
#include <QCheckBox>
#include <QSettings>

DWIDGET_USE_NAMESPACE
typedef DDialog QDlgToDialog;
typedef DDoubleSpinBox QDSBToDDoubleSpinBox;
typedef DComboBox QCBToDComboBox;

class PrintOptionsPage : public QDlgToDialog
{
    Q_OBJECT

public:

    enum Unit {
        Millimeters = 0,
        Centimeters,
        Inches
    };

    enum ScaleMode {
        NoScale = 0,
        ScaleToExpanding,
        ScaleToPage,
        ScaleToCustomSize
    };

    explicit PrintOptionsPage(QWidget *parent = nullptr);

    ScaleMode scaleMode();
    Unit scaleUnit();
    double scaleWidth();
    double scaleHeight();
    Qt::Alignment alignment();

signals:
    void valueChanged();

private:
    void init();
    void updateStatus();
    double unitToInches(Unit unit);

private:
    QRadioButton *m_noScaleBtn;
    QRadioButton *m_fitToImageBtn;
    QRadioButton *m_fitToPageBtn;
    QRadioButton *m_scaleBtn;
    QDSBToDDoubleSpinBox *m_printWidth;
    QDSBToDDoubleSpinBox *m_printHeight;
    QCBToDComboBox *m_printUnit;
    QButtonGroup *m_buttonGroup;
    QButtonGroup *m_posBtnGroup;
    QSettings m_settings;
};

#endif // PRINTOPTIONSPAGE_H
