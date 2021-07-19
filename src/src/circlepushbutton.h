#ifndef CIRCLEPUSHBUTTON_H
#define CIRCLEPUSHBUTTON_H

#include <QPushButton>

class QColor;
class QSvgRenderer;

//自定义圆形按钮
class circlePushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit circlePushButton(QWidget *parent = nullptr);

    void setbackground(QColor color);
    void setPixmap(QString normalPath, QString hoverPath, QString pressPath);//设置正常显示图片、悬浮图片、点击图片
    void setSelected(bool selected);
    bool getButtonState();
    void setButtonRadius(int radius);//设置按钮半径

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    bool m_hover;
    bool m_mousePress;
    bool m_isSelected;

    QColor m_color;

    QSvgRenderer *m_normalSvg;//正常SVG
    QSvgRenderer *m_hoverSvg;//悬浮SVG
    QSvgRenderer *m_pressSvg;//点击SVG

    int           m_radius;
};

#endif // CIRCLEPUSHBUTTON_H
