#include "circlepushbutton.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QSvgRenderer>

circlePushButton::circlePushButton(QWidget *parent): QPushButton(parent), m_radius(20)
{
    m_color.setRgb(0, 0, 0, 102);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;

    m_normalSvg = nullptr;
    m_hoverSvg = nullptr;
    m_pressSvg = nullptr;
    setButtonRadius(m_radius);

}


void circlePushButton::setbackground(QColor color)
{
    m_color = color;
}

void circlePushButton::setPixmap(QString normalPath, QString hoverPath, QString pressPath)
{
    m_normalSvg = new QSvgRenderer(normalPath);
    m_hoverSvg = new QSvgRenderer(hoverPath);
    m_pressSvg = new QSvgRenderer(pressPath);
}

void circlePushButton::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

bool circlePushButton::getButtonState()
{
    return m_isSelected;
}

void circlePushButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int distance = 2;
    int tmpWidth = width();
    int tmpHeight = height();

    //绘制背景色
    QPainterPath path;
    path.addEllipse(rect());
    painter.fillPath(path, QBrush(QColor(m_color)));

    //绘制点击效果
    if (m_hover && !m_mousePress) {
        QRect focusBlue = this->rect();
        focusBlue.setTopLeft(QPoint(0, 0));
        focusBlue.setSize(QSize(tmpWidth, tmpHeight));
        painter.setPen(QPen(QColor(Qt::blue), distance));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(focusBlue);

        QRect focusWhite = this->rect();
        focusWhite.setTopLeft(QPoint(distance, distance));
        focusWhite.setSize(QSize(tmpWidth - 2 * distance, tmpHeight - 2 * distance));
        painter.setPen(QPen(QColor(Qt::white), distance));
        painter.drawEllipse(focusWhite);
    }

    //设置绘制的图案
    if (!m_mousePress && m_isSelected) {
        m_hoverSvg->render(&painter);
    } else if (!m_mousePress && !m_isSelected) {
        m_normalSvg->render(&painter);
    } else {
        m_pressSvg->render(&painter);
    }
}

void circlePushButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void circlePushButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    m_mousePress = false;
    update();
}

void circlePushButton::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = true;
    setSelected(!m_isSelected);
    update();
    QPushButton::mousePressEvent(event);
}

void circlePushButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = false;
    update();
    QPushButton::mouseReleaseEvent(event);
}

void circlePushButton::setButtonRadius(int radius)
{
    if (radius <= 0)
        return;

    m_radius = radius;
    resize(2 * m_radius, 2 * m_radius);
}
