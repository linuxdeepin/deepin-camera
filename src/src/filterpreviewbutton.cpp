// Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "filterpreviewbutton.h"

extern "C" {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <libimagevisualresult6/visualresult.h>
#else
    #include <libimagevisualresult/visualresult.h>
#endif
}

#include <QPainter>
#include <QPainterPath>

#if QT_VERSION_MAJOR <= 5
#include <DApplicationHelper>
#else
#include <DGuiApplicationHelper>
DGUI_USE_NAMESPACE
#endif

DWIDGET_USE_NAMESPACE;

#define MARGIN 5
#define IMAGE_SIZE 40
#define BUTTON_SIZE (IMAGE_SIZE + MARGIN * 2)

filterPreviewButton::filterPreviewButton(QWidget *parent, efilterType filter/* = filter_Normal*/): QWidget(parent)
  , m_filterType(filter)
{
    qDebug() << "Function started: filterPreviewButton constructor";

    QString objName = filterName_CUBE(filter);
    if (objName.isEmpty()) {
        qDebug() << "Condition check: Filter name empty, using 'normal'";
        objName = "normal";
    }
    setObjectName(objName);

    m_color.setRgb(44, 44, 44);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;
    m_disableSelect = false;

    setWindowOpacity(0.1);
    resize(BUTTON_SIZE, BUTTON_SIZE);

    qDebug() << "Function completed: filterPreviewButton constructor";
}

void filterPreviewButton::setbackground(QColor color)
{
    // qDebug() << "Setting button background color:" << color;
    m_color = color;
}

void filterPreviewButton::setImage(QImage *img)
{
    // qDebug() << "Function started: setImage";
    Q_ASSERT(img);

    if (isHidden() && !img->isNull()) {
        // qDebug() << "Condition check: Widget hidden and image not null, skipping";
        return;
    }

    uint8_t* frame = img->bits();
    int width = img->width();
    int height = img->height();
    QString filterName_CUBE = filterPreviewButton::filterName_CUBE(m_filterType);
    if (!filterName_CUBE.isEmpty()) {
        // qDebug() << "Applying filter:" << filterName_CUBE;
        imageFilter24(frame, width, height, filterName_CUBE.toStdString().c_str(), 100);
    }

    m_pixmap = QPixmap::fromImage(QImage(frame, width, height, QImage::Format_RGB888));
    update();
    // qDebug() << "Function completed: setImage";
}

void filterPreviewButton::setSelected(bool selected)
{
    // qDebug() << "Setting button selected state to:" << selected;
    m_isSelected = selected;
    update();
}

bool filterPreviewButton::isSelected()
{
    // qDebug() << "Function started: isSelected: " << m_isSelected;
    return m_isSelected;
}

void filterPreviewButton::setButtonRadius(int radius)
{
    // qDebug() << "Setting button radius to:" << radius;
    m_radius = radius;
    update();
}

void filterPreviewButton::paintEvent(QPaintEvent *event)
{
    // qDebug() << "Function started: paintEvent";
    Q_UNUSED(event);
    QPainter painter(this);
#if QT_VERSION_MAJOR > 5
    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#else
    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);
#endif

    QRect imageRect = QRect(MARGIN, MARGIN, IMAGE_SIZE, IMAGE_SIZE);

    QPainterPath roundPixmapRectPath;
    roundPixmapRectPath.addRoundedRect(imageRect, m_radius, m_radius);

    //绘制滤镜预览图
    if (!m_pixmap.isNull()) {
        // qDebug() << "Drawing pixmap";
        painter.save();
        painter.setClipPath(roundPixmapRectPath);
        painter.drawPixmap(imageRect, m_pixmap);
        painter.restore();
    } else {
        // qDebug() << "Drawing color";
        painter.fillPath(roundPixmapRectPath, QBrush(QColor(m_color)));
    }

    // 预览图边缘绘制透明描边
    painter.save();
    QColor borderPenColor("#000000");
    borderPenColor.setAlphaF(0.12);
    QPen borderPen(borderPenColor);
    borderPen.setWidthF(2);
    painter.setPen(borderPen);
    painter.drawRoundedRect(imageRect.adjusted(-1, -1, 1, 1), m_radius, m_radius);
    painter.restore();

    QColor highlightColor = DGuiApplicationHelper::instance()->applicationPalette().highlight().color();

    //绘制点击效果
    QRect grayRect = imageRect;
    if (m_mousePress) {
        // qDebug() << "Drawing mouse press effect";
        if (m_pixmap.isNull())
            painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 1)));
        else
            painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 0.4)));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(grayRect, m_radius, m_radius);
    } else if (m_hover) {
        // qDebug() << "Drawing hover effect";
        if (m_pixmap.isNull())
            painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 0)));
        else
            painter.setBrush(QBrush(QColor(0, 0, 0, 255 * 0.2)));
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(grayRect, m_radius, m_radius);
    }

    QRect focusWhite = imageRect;
    QRect focusBlue = imageRect.adjusted(-2, -2, 2, 2);
    if (m_hover || m_isSelected && !m_disableSelect) {
        // qDebug() << "Drawing focus effect";
        painter.setPen(QPen(QColor(Qt::white), m_radius / 4));
        painter.drawRoundedRect(focusWhite, m_radius, m_radius);

        if (m_hover)
            highlightColor = Qt::white;
        painter.setPen(QPen(QColor(highlightColor), m_radius / 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(focusBlue, m_radius, m_radius);
    }

    QWidget::paintEvent(event);
    // qDebug() << "Function completed: paintEvent";
}

#if QT_VERSION_MAJOR > 5
void filterPreviewButton::enterEvent(QEnterEvent *event)
#else
void filterPreviewButton::enterEvent(QEvent *event)
#endif
{
    // qDebug() << "Function started: enterEvent";
    Q_UNUSED(event);
    this->setFocus();
    m_hover = true;
    update();
}

void filterPreviewButton::focusInEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: focusInEvent";
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void filterPreviewButton::focusOutEvent(QFocusEvent *event)
{
    // qDebug() << "Function started: focusOutEvent";
    Q_UNUSED(event);
    m_hover = false;
    update();
}

void filterPreviewButton::leaveEvent(QEvent *event)
{
    // qDebug() << "Function started: leaveEvent";
    Q_UNUSED(event);
    this->clearFocus();
    m_hover = false;
    m_mousePress = false;
    update();
}

void filterPreviewButton::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: mousePressEvent";
    Q_UNUSED(event);
    m_mousePress = true;
    setSelected(true);
    update();
    DWidget::mousePressEvent(event);
}

void filterPreviewButton::mouseMoveEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: mouseMoveEvent";
    //解决bug 在按钮中可拖动相机界面，bug 100647
    Q_UNUSED(event);
    return;
}

void filterPreviewButton::mouseReleaseEvent(QMouseEvent *event)
{
    // qDebug() << "Function started: mouseReleaseEvent";
    Q_UNUSED(event);
    m_mousePress = false;
    update();
    emit clicked(false);
    DWidget::mouseReleaseEvent(event);
    // qDebug() << "Function completed: mouseReleaseEvent";
}

void filterPreviewButton::setDisableSelect(bool disable)
{
    // qDebug() << "Setting disable select to:" << disable;
    m_disableSelect = disable;
}

void filterPreviewButton::setOpacity(int opacity)
{
    // qDebug() << "Setting button opacity to:" << opacity;
    m_opacity = opacity;
    m_color.setAlpha(opacity);
    update();
}

efilterType filterPreviewButton::getFiltertype()
{
    // qDebug() << "Function started: getFiltertype";
    return m_filterType;
}

filterPreviewButton::~filterPreviewButton()
{
    // qDebug() << "Destroying filter preview button";
}

QString filterPreviewButton::filterName(efilterType type)
{
    // qDebug() << "Function started: filterName";
    QString name = tr("Normal");
    switch (type) {
    case filter_Normal:
        qDebug() << "Condition check: Normal filter";
        name = tr("Normal");
        break;
    case filter_Vivid:
        qDebug() << "Condition check: Vivid filter";
        name = tr("Vivid");
        break;
    case filter_Warm:
        qDebug() << "Condition check: Warm filter";
        name = tr("Warm");
        break;
    case filter_Cold:
        qDebug() << "Condition check: Cold filter";
        name = tr("Cold");
        break;
    case filter_Retro:
        qDebug() << "Condition check: Retro filter";
        name = tr("Retro");
        break;
    case filter_Film:
        qDebug() << "Condition check: Film filter";
        name = tr("Film");
        break;
    case filter_Gray:
        qDebug() << "Condition check: Gray filter";
        name = tr("Gray");
        break;
    case filter_BAndW:
        qDebug() << "Condition check: B&W filter";
        name = tr("B&W");
        break;
    default:
        qDebug() << "Condition check: Unknown filter type, using Normal";
        name = tr("Normal");
        break;
    }

    qDebug() << "Function completed: filterName:" << name;
    return name;
}

QString filterPreviewButton::filterName_CUBE(efilterType type)
{
    // qDebug() << "Function started: filterName_CUBE";
    QString name = "";
    switch (type) {
    case filter_Normal:
        qDebug() << "Condition check: Normal filter (no CUBE)";
        name = "";
        break;
    case filter_Vivid:
        qDebug() << "Condition check: Vivid filter";
        name = "bright";
        break;
    case filter_Warm:
        qDebug() << "Condition check: Warm filter";
        name = "warm";
        break;
    case filter_Cold:
        qDebug() << "Condition check: Cold filter";
        name = "cold";
        break;
    case filter_Retro:
        qDebug() << "Condition check: Retro filter";
        name = "classic";
        break;
    case filter_Film:
        qDebug() << "Condition check: Film filter";
        name = "print";
        break;
    case filter_Gray:
        qDebug() << "Condition check: Gray filter";
        name = "gray";
        break;
    case filter_BAndW:
        qDebug() << "Condition check: B&W filter";
        name = "black";
        break;
    default:
        qDebug() << "Condition check: Unknown filter type, using empty";
        name = "";
        break;
    }

    qDebug() << "Function completed: filterName_CUBE:" << name;
    return name;
}
