#include "titlebar.h"
#include "datamanager.h"

class TitlebarPrivate
{
public:
    explicit TitlebarPrivate(Titlebar *parent) : q_ptr(parent) {}

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

    setAttribute(Qt::WA_TranslucentBackground, true);
    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    d->m_titlebar = new DTitlebar(this);
    d->m_titlebar->setFocusPolicy(Qt::NoFocus);
//    d->m_titlebar->setWindowFlags(Qt::WindowMinMaxButtonsHint |
//                                  Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    d->m_titlebar->setBackgroundTransparent(true);
    d->m_titlebar->setBlurBackground(false);
    layout->addWidget(d->m_titlebar);

//    d->m_shadowEffect = new QGraphicsDropShadowEffect(this);
//    d->m_shadowEffect->setOffset(d->offsetX, d->offsetY);
//    d->m_shadowEffect->setBlurRadius(d->offsetY);
//    d->m_shadowEffect->setColor(d->darkEffectColor);
//    this->setGraphicsEffect(d->m_shadowEffect);
}

Titlebar::~Titlebar()
{

}

DTitlebar *Titlebar::titlebar()
{
    Q_D(const Titlebar);
    return d->m_titlebar;
}

void Titlebar::paintEvent(QPaintEvent *pe)
{
    Q_D(const Titlebar);

    if(DataManager::instance()->getdevStatus() != NOCAM) {
        QPainter painter(this);
//        QPalette palette = this->palette();
        QPen pen(QColor(0, 0, 0, 0));
        QLinearGradient linearGradient(width(), 0, width(), height());

        linearGradient.setColorAt(0, QColor(0, 0, 0, 255 * 0.5));   //垂直线性渐变
        linearGradient.setColorAt(1, QColor(0, 0, 0, 0));

        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush(QBrush(linearGradient));
        painter.setPen(pen);
        painter.drawRect(rect());

//        palette.setColor(QPalette::Background, QColor(0, 0, 0, 0));
//        this->setPalette(palette);
    } else {
        DBlurEffectWidget::paintEvent(pe);
    }
}
