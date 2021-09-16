/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     fengli <fengli@uniontech.com>
*
* Maintainer: fengli <fengli@uniontech.com>
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
#include "exposureslider.h"

#include <QVBoxLayout>
#include <QPalette>

#include <DFontSizeManager>

#define WIDTH 40
#define HEIGHT 192
ExposureSlider::ExposureSlider(QWidget *parent) : QWidget(parent)
  ,m_pLabShowValue(nullptr)
  ,m_slider(nullptr)
  ,m_valueMax(100)
  ,m_valueMin(-100)
  ,m_curValue(0)
{
    setFixedSize(WIDTH, HEIGHT);
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    setLayout(vLayout);
    setContentsMargins(0, 0, 0, 0);

    QFont font;
    font.setFamily("SourceHanSansSC");
    font.setWeight(QFont::Medium);

    m_pLabShowValue = new DLabel(this);
//    DFontSizeManager::instance()->bind(m_pLabShowValue, DFontSizeManager::T6);
    m_pLabShowValue->setForegroundRole(QPalette::BrightText);
    m_pLabShowValue->setFont(font);
    m_pLabShowValue->setAlignment(Qt::AlignCenter);
    m_pLabShowValue->setText("0");

    QPalette pa = m_pLabShowValue->palette();
    pa.setColor(QPalette::WindowText, Qt::white);
    m_pLabShowValue->setPalette(pa);

    vLayout->addStretch();
    vLayout->addWidget(m_pLabShowValue, 0, Qt::AlignCenter);

    m_slider = new DSlider(Qt::Vertical, this);
    m_slider->setFixedHeight(150);
    m_slider->setIconSize(QSize(15, 15));
    m_slider->show();
    m_slider->slider()->setRange(-100, 100);
    m_slider->setValue(m_curValue);
    m_slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred));
    vLayout->addWidget(m_slider, 1, Qt::AlignCenter);
    vLayout->addStretch();

    connect(m_slider, &DSlider::valueChanged, this, &ExposureSlider::onValueChanged);
}

void ExposureSlider::moveToParentLeft()
{
    if (nullptr == parent())
        return;

    auto pParentWidget = static_cast<QWidget *>(parent());

    if (nullptr == pParentWidget)
        return;

    move(20, pParentWidget->height() / 2 - height() / 2);
}

int ExposureSlider::value()
{
    return m_curValue;
}

void ExposureSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    QRectF topRect(rect().topLeft(), QSize(WIDTH, WIDTH));
    QRectF centerRect(rect().topLeft() + QPoint(0, WIDTH / 2.0), QSize(WIDTH, HEIGHT - WIDTH));
    QRectF bottomRect(rect().topLeft() + QPoint(0, HEIGHT - WIDTH), QSize(WIDTH, WIDTH));

    QPainterPath path;
    path.moveTo(topRect.bottomRight());
    path.arcTo(topRect, 0, 180);
    path.lineTo(centerRect.bottomLeft());
    path.arcTo(bottomRect, 180, 180);
    path.lineTo(centerRect.topRight());

    painter.setPen(Qt::NoPen);
    painter.fillPath(path, QBrush(QColor(0, 0, 0, 0.4 * 255)));
}

void ExposureSlider::wheelEvent(QWheelEvent *event)
{
    if (m_curValue >= m_valueMax)
        m_curValue = m_valueMax;

    if (m_curValue <= m_valueMin)
        m_curValue = m_valueMin;

    m_curValue += event->angleDelta().y() / 60;
    m_slider->setValue(m_curValue);
}

void ExposureSlider::onValueChanged(int value)
{
    if (value > m_valueMax || value < m_valueMin)
        return;

    m_curValue = value;

    m_pLabShowValue->setText(QString("%1").arg(value));
}

ExposureSlider::~ExposureSlider()
{

}
