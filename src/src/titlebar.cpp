// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "titlebar.h"
#include "datamanager.h"
#include <QDebug>

class TitlebarPrivate
{
public:
    explicit TitlebarPrivate(Titlebar *parent) : q_ptr(parent) {}

    QColor          lightColor                  = QColor(255, 255, 255, 204);
    QColor          darkColor                   = QColor(98, 110, 136, 225);
    QColor          darkEffectColor             = QColor(30, 30, 30, 50);
    qreal           offsetX                     = 0;
    qreal           offsetY                     = 15;
    qreal           blurRadius                  = 52;
    QGraphicsDropShadowEffect *m_shadowEffect   = nullptr;
    DTitlebar       *m_titlebar                 = nullptr;

    Titlebar *q_ptr;
    Q_DECLARE_PUBLIC(Titlebar)
};

Titlebar::Titlebar(QWidget *parent) : DBlurEffectWidget(parent), d_ptr(new TitlebarPrivate(this))
{
    Q_D(Titlebar);
    qDebug() << "Initializing Titlebar";

    // 设置DBlurEffectWidget为完全透明
    setBlurEnabled(false);          // 关闭模糊效果
    setMaskAlpha(0);               // 设置遮罩完全透明
    setMaskColor(QColor(0, 0, 0, 0)); // 设置遮罩颜色为完全透明

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    d->m_titlebar = new DTitlebar(this);
    d->m_titlebar->setBackgroundTransparent(true);
    d->m_titlebar->setBlurBackground(false);
    layout->addWidget(d->m_titlebar);

    d->m_shadowEffect = new QGraphicsDropShadowEffect(this);
    d->m_shadowEffect->setOffset(d->offsetX, d->offsetX);
    d->m_shadowEffect->setBlurRadius(d->offsetX);
    d->m_shadowEffect->setColor(Qt::transparent);

    this->setGraphicsEffect(d->m_shadowEffect);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &Titlebar::slotThemeTypeChanged);
    qDebug() << "Titlebar initialization complete";
}

DTitlebar *Titlebar::titlebar()
{
    // qDebug() << "Function started: titlebar";
    Q_D(const Titlebar);
    return d->m_titlebar;
}

void Titlebar::slotThemeTypeChanged()
{
    // qDebug() << "Function started: slotThemeTypeChanged";
    Q_D(const Titlebar);
    QPalette pa;
    if(DataManager::instance()->getdevStatus() != NOCAM) {
        // qDebug() << "Setting light color palette for active camera";
        pa.setColor(QPalette::ButtonText, d->lightColor);
        d->m_titlebar->setPalette(pa);
    } else {
        if(DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType) {
            // qDebug() << "Setting light color palette for dark theme";
            pa.setColor(QPalette::ButtonText, d->lightColor);
            d->m_titlebar->setPalette(pa);
        } else {
            // qDebug() << "Setting dark color palette for light theme";
            pa.setColor(QPalette::ButtonText, d->darkColor);
            d->m_titlebar->setPalette(pa);
        }
    }
    // qDebug() << "Function completed: slotThemeTypeChanged";
}

void Titlebar::paintEvent(QPaintEvent *pe)
{
    // qDebug() << "Function started: Titlebar::paintEvent";
    Q_D(const Titlebar);

    QPainter painter(this);
    QPalette pa;

    if(DataManager::instance()->getdevStatus() != NOCAM) {
        // qDebug() << "Setting light color palette for active camera";
        pa.setColor(QPalette::ButtonText, d->lightColor);
        d->m_titlebar->setPalette(pa);
    }

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::transparent);
    painter.drawRect(rect());
    // qDebug() << "Function completed: Titlebar::paintEvent";
}

Titlebar::~Titlebar()
{
    // qDebug() << "Cleaning up Titlebar";
}
