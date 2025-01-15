// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SWITCH_CAMERA_BTN_H
#define SWITCH_CAMERA_BTN_H

#include <QPushButton>

class QColor;
class QSvgRenderer;
class SwitchCameraBtn : public QWidget
{
    Q_OBJECT
public:
    explicit SwitchCameraBtn(QWidget *parent = nullptr);
    ~SwitchCameraBtn();
signals:
    void clicked();
protected:
    void paintEvent(QPaintEvent *event) override;
#if QT_VERSION_MAJOR > 5
    void enterEvent(QEnterEvent *event) override;
#else
    void enterEvent(QEvent *event) override;
#endif
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    bool                m_bFocus;          //焦点   hover或者tab键选中
    bool                m_bPressed;
    QSvgRenderer        *m_normalSvg;
    QSvgRenderer        *m_pressedSvg;
};

#endif // CIRCLEPUSHBUTTON_H
