#ifndef SET_WIDGET_H
#define SET_WIDGET_H

#include <DWidget>
#include <QWidget>
#include <DLabel>
#include <QVBoxLayout>
#include <QKeyEvent>

DWIDGET_USE_NAMESPACE

class Set_Widget : public DWidget
{
    Q_OBJECT
public:
    explicit Set_Widget(DWidget *parent = nullptr);
    void setup_ui(DWidget *widget);

private:
    void retranslateUi(DWidget *Form);
    //void  Set_widget(DWidget *parent, Qt::WindowFlags s);

signals:

public slots:
    void write_save_file();

private:
    QVBoxLayout *WdgtLayout;   //主窗口布局
    QVBoxLayout *SaPosVerlay;   //保存位置垂直布局
    DLabel *SaPosLbl;           //保存位置Label
    DGroupBox *SaPosGBox;
    QHBoxLayout *SaPosGBoxLayout;
    DRadioButton *DeskRBtn;
    DRadioButton *AppFileRBtn;
    QVBoxLayout *PhtVerlay;
    DLabel *PhtLbl;
    DGroupBox *PhtGBox;
    QHBoxLayout *PhtGBoxLayout;
    DRadioButton *PhZeroRBtn;
    DRadioButton *PhFourRBtn;
    DRadioButton *PhTenRBtn;
    QVBoxLayout *TiDeVerlay;
    DLabel *TiDeLbl;
    DGroupBox *TiDeGBox;
    QHBoxLayout *TiDeGBoxLayout;
    DRadioButton *TiZeroRBtn;
    DRadioButton *TiThreeRBtn;
    DRadioButton *TiSixRBtn;
    QVBoxLayout *PhFormVerlay;
    DLabel *PhFormLbl;
    DGroupBox *PhFormGBox;
    QHBoxLayout *PhFormGBoxLayout;
    DRadioButton *PFpngRBtn;
    QVBoxLayout *ViFormVerlay;
    DLabel *ViFormLbl;
    DGroupBox *ViFormGBox;
    QHBoxLayout *ViFormGBoxLayout;
    DRadioButton *VFmp4RBtn;
    QSpacerItem *VerSp;


protected:
//    void keyReleaseEvent(QKeyEvent *ev) override;
//    void keyPressEvent(QKeyEvent *ev) override;

};

#endif // SET_WIDGET_H
