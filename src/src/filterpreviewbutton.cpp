/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     tanlang <tanlang@uniontech.com>
*             
* Maintainer: tanlang <tanlang@uniontech.com>
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

#include "filterpreviewbutton.h"

extern "C" {
#include <libvisualresult/visualresult.h>
}

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>

DWIDGET_USE_NAMESPACE;

filterPreviewButton::filterPreviewButton(QWidget *parent, efilterType filter/* = filter_Normal*/): DWidget(parent)
  , m_radius(20)
  , m_filterType(filter)
{
    m_label = new QLabel(this);
    m_label->setFixedSize(40, 40);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(3,3,3,3);
    m_mainLayout->setSpacing(0);

    m_mainLayout->addWidget(m_label);

    this->setLayout(m_mainLayout);

    m_color.setRgb(0, 0, 0, m_opacity);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;
    m_disableSelect = false;

    resize(46, 46);
    setButtonRadius(m_radius);
}

void filterPreviewButton::setbackground(QColor color)
{
    m_color = color;
}

void filterPreviewButton::setImage(QImage *img)
{
    Q_ASSERT(img);
    Q_ASSERT(m_label);

    if (img->isNull())
        return;

    uint8_t* frame = img->bits();
    int width = img->width();
    int height = img->height();
    QString filterName_CUBE = filterPreviewButton::filterName_CUBE(m_filterType);
    if (!filterName_CUBE.isEmpty())
        imageFilter24(frame, width, height, filterName_CUBE.toStdString().c_str(), 100);

    QRect labelRt = m_label->rect();
    m_label->setPixmap(QPixmap::fromImage(QImage(frame, width, height, QImage::Format_RGB888)/*.scaled(40,40,Qt::IgnoreAspectRatio)*/));
}

void filterPreviewButton::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

bool filterPreviewButton::isSelected()
{
    return m_isSelected;
}

void filterPreviewButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

    //绘制背景色
    QPainterPath path;
    path.addRoundRect(m_label->geometry(),20,20);
    painter.fillPath(path, QBrush(QColor(m_color)));
}

void filterPreviewButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void filterPreviewButton::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void filterPreviewButton::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    update();
}

void filterPreviewButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    m_mousePress = false;
    update();
}

void filterPreviewButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = true;
    setSelected(!m_isSelected);
    update();
    DWidget::mousePressEvent(event);
}

void filterPreviewButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = false;
    update();
    emit clicked(false);
    DWidget::mouseReleaseEvent(event);
}

void filterPreviewButton::setButtonRadius(int radius)
{
    if (radius <= 0)
        return;

    m_radius = radius;
}

void filterPreviewButton::setDisableSelect(bool disable)
{
    m_disableSelect = disable;
}

void filterPreviewButton::setOpacity(int opacity)
{
    m_opacity = opacity;
    m_color.setAlpha(opacity);
    update();
}

void filterPreviewButton::setRotate(int rotate)
{
    m_rotate = rotate;
    update();
}

efilterType filterPreviewButton::getFiltertype()
{
    return m_filterType;
}

filterPreviewButton::~filterPreviewButton()
{

}

QString filterPreviewButton::filterName(efilterType type)
{
    QString name = tr("Normal");
    switch (type) {
    case filter_Normal:
        name = tr("Normal");
        break;
    case filter_Vivid:
        name = tr("Vivid");
        break;
    case filter_Warm:
        name = tr("Warm");
        break;
    case filter_Cold:
        name = tr("Cold");
        break;
    case filter_Retro:
        name = tr("Retro");
        break;
    case filter_Film:
        name = tr("Film");
        break;
    case filter_Gray:
        name = tr("Gray");
        break;
    case filter_BAndW:
        name = tr("B&W");
        break;
    default:
        name = tr("Normal");
        break;
    }

    return name;
}

QString filterPreviewButton::filterName_CUBE(efilterType type)
{
    QString name = "";
    switch (type) {
    case filter_Normal:
        name = "";
        break;
    case filter_Vivid:
        name = "bright";
        break;
    case filter_Warm:
        name = "warm";
        break;
    case filter_Cold:
        name = "cold";
        break;
    case filter_Retro:
        name = "classic";
        break;
    case filter_Film:
        name = "print";
        break;
    case filter_Gray:
        name = "gray";
        break;
    case filter_BAndW:
        name = "black";
        break;
    default:
        name = "";
        break;
    }

    return name;
}
