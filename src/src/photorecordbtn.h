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

#ifndef PHOTO_RECORD_BTN_H
#define PHOTO_RECORD_BTN_H

#include <QPushButton>

class QColor;
class QSvgRenderer;
class photoRecordBtn : public QWidget
{
    Q_OBJECT
public:
    typedef enum _record_state{
        Normal     = 0,         //未开始
        preRecord  = 1,         //倒计时
        Recording  = 2          //正在录像
    }RecordState;

    explicit photoRecordBtn(QWidget *parent = nullptr);

    /**
     * @brief setState 设置拍照，录像状态
     * @param bPhoto  true 拍照状态， false 录像状态
     */
    void setState(bool bPhoto) {m_bPhoto = bPhoto; update();}

    /**
     * @brief setRecordState 设置录像状态
     * @param state 录像状态，相见上枚举定义
     */
    void setRecordState(int state) {m_recordState = state; update();}

    /**
     * @brief photoState 获取是否拍照状态
     */
    bool photoState() {return m_bPhoto;}
signals:
    void clicked();
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    bool m_bPhoto;          //true 拍照状态， false 录像状态
    bool m_bFocus;          //焦点   hover或者tab键选中
    bool m_bPress;          //press
    int  m_recordState;     //录制状态
};

#endif // CIRCLEPUSHBUTTON_H
