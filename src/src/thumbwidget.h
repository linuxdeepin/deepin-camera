#ifndef THUMBWIDGET_H
#define THUMBWIDGET_H

#include <QObject>
#include <DLabel>

DWIDGET_USE_NAMESPACE

class ThumbWidget : public DLabel
{
    Q_OBJECT
public:
    ThumbWidget(QWidget *parent = 0, int outlinewidth = 2);

protected:

    /**
    * @brief focusInEvent　焦点进入事件
    * @arg event 事件
    */
    void focusInEvent(QFocusEvent *event)override;

    /**
    * @brief focusOutEvent　焦点离开事件
    * @arg event 事件
    */
    void focusOutEvent(QFocusEvent *event)override;

    /**
    * @brief paintEvent 绘制事件
    * @arg event 事件
    */
    void paintEvent(QPaintEvent *event)override;

private:
    /**
    * @arg m_tabFocusStatus tab焦点状态
    */
    bool m_tabFocusStatus;

    /**
    * @arg m_outLineWidth 矩形线框宽度
    */
    const int m_outLineWidth;
};

#endif // THUMBWIDGET_H
