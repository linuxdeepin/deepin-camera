#ifndef BUTTON_H
#define BUTTON_H

#include <QWidget>

class QColor;
class QSvgRenderer;

class button : public QWidget
{
    Q_OBJECT
public:
    explicit button(QWidget *parent = nullptr);

    void setbackground(QColor color);
    void setPixmap(QString normalPath, QString hoverPath, QString pressPath);
    void setSelected(bool selected);
    bool getButtonState();

signals:
    void clicked();
public slots:

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    bool m_hover;
    bool m_mousePress;
    bool m_isSelected;

    QColor m_color;

    QSvgRenderer *m_normalSvg;
    QSvgRenderer *m_hoverSvg;
    QSvgRenderer *m_pressSvg;

};

class photoRecordBtn : public QWidget
{
    Q_OBJECT
public:
    typedef enum _record_state{
        Normal     = 0,
        preRecord  = 1,
        Recording  = 2
    }RecordState;

    explicit photoRecordBtn(QWidget *parent = nullptr);

    void setState(bool bPhoto) {m_bPhoto = bPhoto;}
    void setRecordState(int state) {m_recordState = state;}
signals:
    void clicked();
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;

private:
    bool m_bPhoto;          //拍照按钮
    bool m_bFocus;          //焦点   hover或者tab键选中
    bool m_bPress;          //press
    int  m_recordState;     //录制状态
};

#endif // BUTTON_H
