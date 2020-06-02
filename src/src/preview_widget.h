#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

#include <DWidget>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

DWIDGET_USE_NAMESPACE

class PreviewWidget : public DWidget
{
    Q_OBJECT
public:
    PreviewWidget(DWidget *parent = nullptr);

    void setup_ui(DWidget *widget);
    void retranslateUi();
private:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *p_selectbtn;//切换
    QVBoxLayout *verticalLayout_2;
    QFrame *p_preview;
    QLabel *p_timelabl;
    QHBoxLayout *horizontalLayout_2;
    QFrame *p_verline;
    QGroupBox *PhViGBox;
    QVBoxLayout *PhViGBoxLayout;
    QPushButton *p_takevideobtn;//录像
    QPushButton *p_takecambtn;//拍照
    QHBoxLayout *horizontalLayout_4;
    QFrame *p_horline;
    QHBoxLayout *horizontalLayout_6;
    QWidget *scrollAreaWidgetContents;
};

#endif // PREVIEW_WIDGET_H
