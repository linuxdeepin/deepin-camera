#ifndef ROLLINGBOX_H
#define ROLLINGBOX_H

#include <QWidget>
#include <QPropertyAnimation>

class RollingBox : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int deviation READ readDeviation WRITE setDeviation)

public:
    explicit RollingBox(QWidget *parent = 0);

    /**
    * @brief setRange 设置范围
    * @param min 最小值
    * @param max 最大值
    */
    void setRange(int min,int max);

    /**
    * @brief setContentList 设置选项列表
    * @param content字符串列表
    */
    void setContentList(const QList<QString> &content);

    /**
    * @brief getCurrentValue 获取当前选项
    */
    int getCurrentValue();

protected:
    /**
    * @brief 鼠标按下事件
    */
    void mousePressEvent(QMouseEvent *);

    /**
    * @brief 鼠标移动事件
    */
    void mouseMoveEvent(QMouseEvent *);

    /**
    * @brief 鼠标释放事件
    */
    void mouseReleaseEvent(QMouseEvent *);

    /**
    * @brief 鼠标滚动事件
    */
    void wheelEvent(QWheelEvent *);

    /**
    * @brief 绘制事件
    */
    void paintEvent(QPaintEvent *);

    /**
    * @brief paintContent 绘制内容
    */
    void paintContent(QPainter &painter,int num,int deviation);

    /**
    * @brief 使选中的内容回到控件中间
    */
    void homing();

    /**
    * @brief readDeviation 获取鼠标移动偏移量，默认为0
    */
    int readDeviation();

    /**
    * @brief setDeviation 设置偏移量
    * @param n 偏移量
    */
    void setDeviation(int n);

signals:
    /**
    * @brief 当前数值改变信号
    */
    void currentValueChanged(int value);

private:
    int                 m_rangeMin;
    int                 m_rangeMax;
    int                 m_currentValue;
    bool                isDragging;
    int                 m_deviation;
    int                 m_mouseSrcPos;
    int                 m_textSize;    //字符尺寸
    QPropertyAnimation  *m_homingAnimation;
    QTimer              *m_pressResetTimer;//重置鼠标按下状态
    QList<QString>      m_content;
};

#endif // ROLLINGBOX_H
