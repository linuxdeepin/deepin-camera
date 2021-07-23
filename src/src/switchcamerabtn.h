/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     wuzhigang <wuzhigang@uniontech.com>
*            
* Maintainer: wuzhigang <wuzhigang@uniontech.com>
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
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    bool                m_bFocus;          //焦点   hover或者tab键选中
    bool                m_bPressed;
    QSvgRenderer        *m_normalSvg;
    QSvgRenderer        *m_pressedSvg;
};

#endif // CIRCLEPUSHBUTTON_H
