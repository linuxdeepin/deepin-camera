#include "button.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>
#include <QSvgRenderer>

button::button(QWidget *parent) : QWidget(parent)
{
    m_color.setRgb(0, 0, 0, 102);

    m_hover = false;
    m_mousePress = false;
    m_isSelected = false;

    m_normalSvg = nullptr;
    m_hoverSvg = nullptr;
    m_pressSvg = nullptr;
}

void button::setbackground(QColor color)
{
    m_color = color;
}

void button::setPixmap(QString normalPath, QString hoverPath, QString pressPath)
{
    m_normalSvg = new QSvgRenderer(normalPath);
    m_hoverSvg = new QSvgRenderer(hoverPath);
    m_pressSvg = new QSvgRenderer(pressPath);
}

void button::setSelected(bool selected)
{
    m_isSelected = selected;
    update();
}

bool button::getButtonState()
{
    return m_isSelected;
}

void button::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath path;
    QRect grayRect = this->rect();
    if (nullptr == m_normalSvg
        || nullptr == m_hoverSvg
            || nullptr == m_pressSvg){
        return;
    }

    if (m_hover) {
        if (!m_mousePress) {
            QRect focusBlue = this->rect();
            focusBlue.setTopLeft(QPoint(0, 0));
            focusBlue.setSize(QSize(40, 40));
            painter.setPen(QPen(QColor(Qt::blue), 2));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(focusBlue);

            QRect focusWhite = this->rect();
            focusWhite.setTopLeft(QPoint(2, 2));
            focusWhite.setSize(QSize(38, 38));
            painter.setPen(QPen(QColor(Qt::white), 2));
            painter.drawEllipse(focusWhite);
        }

        grayRect.setTopLeft(QPoint(3, 3));
        grayRect.setSize(QSize(36, 36));
        path.addEllipse(grayRect);
        painter.setBrush(QBrush(QColor(m_color)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(grayRect);
    } else {
        path.addEllipse(grayRect);
        painter.fillPath(path, QBrush(QColor(m_color)));
    }

    if (!m_mousePress) {
        if (m_isSelected) {
            m_hoverSvg->render(&painter);
        } else {
            m_normalSvg->render(&painter);
        }
    } else {
        m_pressSvg->render(&painter);
    }
}

void button::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = true;
    update();
}

void button::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_hover = false;
    update();
}

void button::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = true;
    setSelected(!m_isSelected);
    update();
}

void button::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_mousePress = false;
    update();
    emit clicked();
}

void button::focusInEvent(QFocusEvent *event)
{
    m_hover = true;
    update();
}

photoRecordBtn::photoRecordBtn(QWidget *parent/* = nullptr*/)
    :QWidget (parent),
      m_bPhoto(true),
      m_bFocus(false),
      m_bPress(false),
      m_recordState(Normal)
{

}



void photoRecordBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter.setBrush(Qt::NoBrush);

    QPainterPath path;
    QRect rect = this->rect();

    int offset1 = 4;
    int offset2 = 8;
    int offset3 = 10;
    int offset4 = 5;

    QRect rect1,rect2,rect3,rect4;
    //m_bFocus = true;
    QColor color = m_bFocus ? QColor(0,0,255,255) : QColor(0,0,0,25);
    painter.setPen(QPen(color, offset1));
    rect1.setTopLeft(QPoint(rect.top()+offset1/2, rect.left()+offset1/2));
    rect1.setSize(QSize(rect.width()-offset1, rect.height()-offset1));
    painter.drawEllipse(rect1);


    rect2.setTopLeft(QPoint(rect1.x()+offset2/2, rect1.y()+offset2/2));
    rect2.setSize(QSize(rect1.width()-offset2, rect1.height()-offset2));
    painter.setPen(QPen(QColor(255,255,255,100), offset2/2));
    painter.drawEllipse(rect2);

    rect3.setTopLeft(QPoint(rect2.x()+offset3/2, rect2.y()+offset3/2));
    rect3.setSize(QSize(rect2.width()-offset3, rect2.height()-offset3));
    painter.setPen(QPen(QColor(0,0,0,12), offset3/2));
    painter.drawEllipse(rect3);

    rect4.setTopLeft(QPoint(rect3.x()+offset4/2, rect3.y()+offset4/2));
    rect4.setSize(QSize(rect3.width()-offset4, rect3.height()-offset4));
    //painter.setBrush(QBrush(QColor(Qt::blue)));
    path.addEllipse(rect4);
    QColor fillColor = QColor(255,255,255,100);
    if (!m_bPhoto)
    {
        int Rect5Width = 9;
        QRect rect5;
        rect5.setTopLeft(QPoint((rect.width() - Rect5Width)/2, (rect.width() - Rect5Width)/2));
        rect5.setSize(QSize(Rect5Width,Rect5Width));
        if (Normal == m_recordState){
            fillColor = QColor(0xff,0x57,0x36,0xff);
            painter.fillPath(path, QBrush(fillColor));
        }
        else if (preRecord == m_recordState){
            fillColor = QColor(0,0,0,12);
            painter.setPen(QPen(QColor(255,255,255,0xff), Rect5Width));
            painter.fillPath(path, QBrush(fillColor));
            painter.drawRoundedRect(rect5,2,2);
        }
        else if(Recording == m_recordState){
            painter.setPen(QPen(QColor(0xff,0x57,0x36,0xff), Rect5Width));
            painter.fillPath(path, QBrush(fillColor));
            painter.drawRoundedRect(rect5,2,2);
        }

    }
    else {
       painter.fillPath(path, QBrush(fillColor));
    }
}

void photoRecordBtn::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}

void photoRecordBtn::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = false;
    update();
}

void photoRecordBtn::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bPress = true;
    update();
}

void photoRecordBtn::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_bPress = false;
    update();
    emit clicked();
}

void photoRecordBtn::focusInEvent(QFocusEvent *event)
{
    Q_UNUSED(event);
    m_bFocus = true;
    update();
}

