#include "set_widget.h"

#include <DWidget>
#include <DGroupBox>
#include <DApplication>
#include <DRadioButton>
#include <QVBoxLayout>
#include <QToolButton>

DWIDGET_USE_NAMESPACE

Set_Widget::Set_Widget(DWidget *parent) : DWidget(parent)
{
    setup_ui(this);
    this->hide();
}

void Set_Widget::setup_ui(DWidget *Form)
{
    if (Form->objectName().isEmpty())
        Form->setObjectName(QStringLiteral("Form"));
    Form->setEnabled(true);
    Form->resize(990, 757);
    Form->setMaximumSize(65535, 65535);
    Form->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    WdgtLayout = new QVBoxLayout(Form);
    WdgtLayout->setObjectName(QStringLiteral("WdgtLayout"));
    WdgtLayout->setContentsMargins(10, -1, -1, -1);
    SaPosVerlay = new QVBoxLayout();
    SaPosVerlay->setObjectName(QStringLiteral("SaPosVerlay"));
    SaPosLbl = new DLabel(Form);
    SaPosLbl->setObjectName(QStringLiteral("SaPosLbl"));
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(SaPosLbl->sizePolicy().hasHeightForWidth());
    SaPosLbl->setSizePolicy(sizePolicy);
    SaPosLbl->setMinimumSize(QSize(70, 30));
    SaPosLbl->setMaximumSize(QSize(70, 30));
    SaPosLbl->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    SaPosVerlay->addWidget(SaPosLbl);

    SaPosGBox = new DGroupBox(Form);
    SaPosGBox->setObjectName(QStringLiteral("SaPosGBox"));
    SaPosGBox->setMinimumSize(QSize(170, 40));
    SaPosGBox->setMaximumSize(QSize(170, 40));
    SaPosGBox->setAlignment(Qt::AlignCenter);
    SaPosGBoxLayout = new QHBoxLayout(SaPosGBox);
    SaPosGBoxLayout->setSpacing(3);
    SaPosGBoxLayout->setObjectName(QStringLiteral("SaPosGBoxLayout"));
    SaPosGBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    SaPosGBoxLayout->setContentsMargins(1, 1, 1, 0);
    SaPosGBox->setFlat(true);

    DeskRBtn = new DRadioButton(SaPosGBox);

    DeskRBtn->setObjectName(QStringLiteral("DeskRBtn"));
    sizePolicy.setHeightForWidth(DeskRBtn->sizePolicy().hasHeightForWidth());
    DeskRBtn->setSizePolicy(sizePolicy);
    DeskRBtn->setMinimumSize(QSize(60, 30));
    DeskRBtn->setFocusPolicy(Qt::NoFocus);
    DeskRBtn->setChecked(true);

    SaPosGBoxLayout->addWidget(DeskRBtn);

    AppFileRBtn = new DRadioButton(SaPosGBox);
    AppFileRBtn->setObjectName(QStringLiteral("AppFileRBtn"));
    sizePolicy.setHeightForWidth(AppFileRBtn->sizePolicy().hasHeightForWidth());
    AppFileRBtn->setSizePolicy(sizePolicy);
    AppFileRBtn->setMinimumSize(QSize(60, 30));
    AppFileRBtn->setFocusPolicy(Qt::NoFocus);

    SaPosGBoxLayout->addWidget(AppFileRBtn);

    SaPosGBoxLayout->setStretch(0, 1);

    SaPosVerlay->addWidget(SaPosGBox);


    WdgtLayout->addLayout(SaPosVerlay);

    PhtVerlay = new QVBoxLayout();
    PhtVerlay->setObjectName(QStringLiteral("PhtVerlay"));
    PhtLbl = new DLabel(Form);
    PhtLbl->setObjectName(QStringLiteral("PhtLbl"));
    sizePolicy.setHeightForWidth(PhtLbl->sizePolicy().hasHeightForWidth());
    PhtLbl->setSizePolicy(sizePolicy);
    PhtLbl->setMinimumSize(QSize(70, 30));
    PhtLbl->setMaximumSize(QSize(70, 30));
    PhtLbl->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    PhtVerlay->addWidget(PhtLbl);

    PhtGBox = new DGroupBox(Form);
    PhtGBox->setObjectName(QStringLiteral("PhtGBox"));
    PhtGBox->setMinimumSize(QSize(210, 40));
    PhtGBox->setMaximumSize(QSize(210, 40));
    PhtGBox->setFlat(true);
    PhtGBoxLayout = new QHBoxLayout(PhtGBox);
    PhtGBoxLayout->setObjectName(QStringLiteral("PhtGBoxLayout"));
    PhZeroRBtn = new DRadioButton(PhtGBox);
    PhZeroRBtn->setObjectName(QStringLiteral("PhZeroRBtn"));
    sizePolicy.setHeightForWidth(PhZeroRBtn->sizePolicy().hasHeightForWidth());
    PhZeroRBtn->setSizePolicy(sizePolicy);
    PhZeroRBtn->setMinimumSize(QSize(60, 30));
    PhZeroRBtn->setMaximumSize(QSize(60, 30));
    PhZeroRBtn->setFocusPolicy(Qt::NoFocus);
    PhZeroRBtn->setChecked(true);

    PhtGBoxLayout->addWidget(PhZeroRBtn);

    PhFourRBtn = new DRadioButton(PhtGBox);
    PhFourRBtn->setObjectName(QStringLiteral("PhFourRBtn"));
    sizePolicy.setHeightForWidth(PhFourRBtn->sizePolicy().hasHeightForWidth());
    PhFourRBtn->setSizePolicy(sizePolicy);
    PhFourRBtn->setMinimumSize(QSize(60, 30));
    PhFourRBtn->setMaximumSize(QSize(60, 30));
    PhFourRBtn->setFocusPolicy(Qt::NoFocus);

    PhtGBoxLayout->addWidget(PhFourRBtn);

    PhTenRBtn = new DRadioButton(PhtGBox);
    PhTenRBtn->setObjectName(QStringLiteral("PhTenRBtn"));
    sizePolicy.setHeightForWidth(PhTenRBtn->sizePolicy().hasHeightForWidth());
    PhTenRBtn->setSizePolicy(sizePolicy);
    PhTenRBtn->setMinimumSize(QSize(60, 30));
    PhTenRBtn->setMaximumSize(QSize(60, 30));
    PhTenRBtn->setFocusPolicy(Qt::NoFocus);

    PhtGBoxLayout->addWidget(PhTenRBtn);

    PhtVerlay->addWidget(PhtGBox);

    WdgtLayout->addLayout(PhtVerlay);

    TiDeVerlay = new QVBoxLayout();
    TiDeVerlay->setObjectName(QStringLiteral("TiDeVerlay"));
    TiDeLbl = new DLabel(Form);
    TiDeLbl->setObjectName(QStringLiteral("TiDeLbl"));
    sizePolicy.setHeightForWidth(TiDeLbl->sizePolicy().hasHeightForWidth());
    TiDeLbl->setSizePolicy(sizePolicy);
    TiDeLbl->setMinimumSize(QSize(110, 30));
    TiDeLbl->setMaximumSize(QSize(110, 30));
    TiDeLbl->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    TiDeVerlay->addWidget(TiDeLbl);

    TiDeGBox = new DGroupBox(Form);
    TiDeGBox->setObjectName(QStringLiteral("TiDeGBox"));
    TiDeGBox->setMinimumSize(QSize(210, 40));
    TiDeGBox->setMaximumSize(QSize(210, 40));
    TiDeGBox->setFlat(true);
    TiDeGBoxLayout = new QHBoxLayout(TiDeGBox);
    TiDeGBoxLayout->setObjectName(QStringLiteral("TiDeGBoxLayout"));
    TiZeroRBtn = new DRadioButton(TiDeGBox);
    TiZeroRBtn->setObjectName(QStringLiteral("TiZeroRBtn"));
    sizePolicy.setHeightForWidth(TiZeroRBtn->sizePolicy().hasHeightForWidth());
    TiZeroRBtn->setSizePolicy(sizePolicy);
    TiZeroRBtn->setMinimumSize(QSize(60, 30));
    TiZeroRBtn->setMaximumSize(QSize(60, 30));
    TiZeroRBtn->setFocusPolicy(Qt::NoFocus);
    TiZeroRBtn->setChecked(true);

    TiDeGBoxLayout->addWidget(TiZeroRBtn);

    TiThreeRBtn = new DRadioButton(TiDeGBox);
    TiThreeRBtn->setObjectName(QStringLiteral("TiThreeRBtn"));
    sizePolicy.setHeightForWidth(TiThreeRBtn->sizePolicy().hasHeightForWidth());
    TiThreeRBtn->setSizePolicy(sizePolicy);
    TiThreeRBtn->setMinimumSize(QSize(60, 30));
    TiThreeRBtn->setMaximumSize(QSize(60, 30));
    TiThreeRBtn->setFocusPolicy(Qt::NoFocus);

    TiDeGBoxLayout->addWidget(TiThreeRBtn);

    TiSixRBtn = new DRadioButton(TiDeGBox);
    TiSixRBtn->setObjectName(QStringLiteral("TiSixRBtn"));
    sizePolicy.setHeightForWidth(TiSixRBtn->sizePolicy().hasHeightForWidth());
    TiSixRBtn->setSizePolicy(sizePolicy);
    TiSixRBtn->setMinimumSize(QSize(60, 30));
    TiSixRBtn->setMaximumSize(QSize(60, 30));
    TiSixRBtn->setFocusPolicy(Qt::NoFocus);

    TiDeGBoxLayout->addWidget(TiSixRBtn);

    TiDeVerlay->addWidget(TiDeGBox);

    WdgtLayout->addLayout(TiDeVerlay);

    PhFormVerlay = new QVBoxLayout();
    PhFormVerlay->setObjectName(QStringLiteral("PhFormVerlay"));
    PhFormLbl = new DLabel(Form);
    PhFormLbl->setObjectName(QStringLiteral("PhFormLbl"));
    sizePolicy.setHeightForWidth(PhFormLbl->sizePolicy().hasHeightForWidth());
    PhFormLbl->setSizePolicy(sizePolicy);
    PhFormLbl->setMinimumSize(QSize(70, 30));
    PhFormLbl->setMaximumSize(QSize(110, 30));
    PhFormLbl->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    PhFormVerlay->addWidget(PhFormLbl);

    PhFormGBox = new DGroupBox(Form);
    PhFormGBox->setObjectName(QStringLiteral("PhFormGBox"));
    PhFormGBox->setMinimumSize(QSize(70, 30));
    PhFormGBox->setMaximumSize(QSize(70, 30));
    PhFormGBox->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    PhFormGBox->setFlat(true);
    PhFormGBox->setCheckable(false);
    PhFormGBoxLayout = new QHBoxLayout(PhFormGBox);
    PhFormGBoxLayout->setSpacing(1);
    PhFormGBoxLayout->setObjectName(QStringLiteral("PhFormGBoxLayout"));
    PhFormGBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    PhFormGBoxLayout->setContentsMargins(1, 1, 1, 1);
    PFpngRBtn = new DRadioButton(PhFormGBox);
    PFpngRBtn->setObjectName(QStringLiteral("PFpngRBtn"));
    PFpngRBtn->setEnabled(false);
    sizePolicy.setHeightForWidth(PFpngRBtn->sizePolicy().hasHeightForWidth());
    PFpngRBtn->setSizePolicy(sizePolicy);
    PFpngRBtn->setMinimumSize(QSize(60, 30));
    PFpngRBtn->setMaximumSize(QSize(60, 30));
    PFpngRBtn->setFocusPolicy(Qt::NoFocus);
    PFpngRBtn->setChecked(true);

    PhFormGBoxLayout->addWidget(PFpngRBtn);

    PhFormGBoxLayout->setStretch(0, 1);

    PhFormVerlay->addWidget(PhFormGBox);

    WdgtLayout->addLayout(PhFormVerlay);

    ViFormVerlay = new QVBoxLayout();
    ViFormVerlay->setObjectName(QStringLiteral("ViFormVerlay"));
    ViFormLbl = new DLabel(Form);
    ViFormLbl->setObjectName(QStringLiteral("ViFormLbl"));
    sizePolicy.setHeightForWidth(ViFormLbl->sizePolicy().hasHeightForWidth());
    ViFormLbl->setSizePolicy(sizePolicy);
    ViFormLbl->setMinimumSize(QSize(70, 30));
    ViFormLbl->setMaximumSize(QSize(70, 30));
    ViFormLbl->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

    ViFormVerlay->addWidget(ViFormLbl);

    ViFormGBox = new DGroupBox(Form);
    ViFormGBox->setFlat(true);
    ViFormGBox->setObjectName(QStringLiteral("ViFormGBox"));
    ViFormGBox->setMinimumSize(QSize(70, 30));
    ViFormGBox->setMaximumSize(QSize(70, 30));
    ViFormGBox->setAlignment(Qt::AlignCenter);
    ViFormGBoxLayout = new QHBoxLayout(ViFormGBox);
    ViFormGBoxLayout->setSpacing(1);
    ViFormGBoxLayout->setObjectName(QStringLiteral("ViFormGBoxLayout"));
    ViFormGBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);
    ViFormGBoxLayout->setContentsMargins(1, 1, 1, 1);
    VFmp4RBtn = new DRadioButton(ViFormGBox);
    VFmp4RBtn->setObjectName(QStringLiteral("VFmp4RBtn"));
    VFmp4RBtn->setEnabled(false);
    sizePolicy.setHeightForWidth(VFmp4RBtn->sizePolicy().hasHeightForWidth());
    VFmp4RBtn->setSizePolicy(sizePolicy);
    VFmp4RBtn->setMinimumSize(QSize(60, 30));
    VFmp4RBtn->setMaximumSize(QSize(60, 30));
    VFmp4RBtn->setFocusPolicy(Qt::NoFocus);
    VFmp4RBtn->setChecked(true);

    ViFormGBoxLayout->addWidget(VFmp4RBtn);

    ViFormGBoxLayout->setStretch(0, 1);

    ViFormVerlay->addWidget(ViFormGBox);

    WdgtLayout->addLayout(ViFormVerlay);

    VerSp = new QSpacerItem(20, 348, QSizePolicy::Minimum, QSizePolicy::Expanding);

    WdgtLayout->addItem(VerSp);

    retranslateUi(Form);

    QMetaObject::connectSlotsByName(Form);
}

void Set_Widget::retranslateUi(DWidget *Form)
{
    Form->setWindowTitle(QApplication::translate("Form", "Form", nullptr));
    SaPosLbl->setText(QApplication::translate("Form", "\344\277\235\345\255\230\344\275\215\347\275\256\357\274\232", nullptr));
    SaPosGBox->setTitle(QString());
    DeskRBtn->setText(QApplication::translate("Form", "\346\241\214\351\235\242", nullptr));
    AppFileRBtn->setText(QApplication::translate("Form", "\345\272\224\347\224\250\346\226\207\344\273\266\345\244\271", nullptr));
    PhtLbl->setText(QApplication::translate("Form", "\350\277\236\346\213\215:", nullptr));
    PhtGBox->setTitle(QString());
    PhZeroRBtn->setText(QApplication::translate("Form", "\346\227\240", nullptr));
    PhFourRBtn->setText(QApplication::translate("Form", "&4\345\274\240", nullptr));
    PhTenRBtn->setText(QApplication::translate("Form", "&10\345\274\240", nullptr));
    TiDeLbl->setText(QApplication::translate("Form", "\346\213\215\347\205\247\345\273\266\346\227\266\350\256\276\347\275\256\357\274\232", nullptr));
    TiDeGBox->setTitle(QString());
    TiZeroRBtn->setText(QApplication::translate("Form", "\346\227\240", nullptr));
    TiThreeRBtn->setText(QApplication::translate("Form", "&3\347\247\222", nullptr));
    TiSixRBtn->setText(QApplication::translate("Form", "&6\347\247\222", nullptr));
    PhFormLbl->setText(QApplication::translate("Form", "\347\205\247\347\211\207\346\240\274\345\274\217\357\274\232", nullptr));
    PhFormGBox->setTitle(QString());
    PFpngRBtn->setText(QApplication::translate("Form", "&png", nullptr));
    ViFormLbl->setText(QApplication::translate("Form", "\350\247\206\351\242\221\346\240\274\345\274\217\357\274\232", nullptr));
    ViFormGBox->setTitle(QString());
    VFmp4RBtn->setText(QApplication::translate("Form", "&mp4", nullptr));
}

void Set_Widget::write_save_file()
{

} // retranslateUi

//void Set_Widget::keyReleaseEvent(QKeyEvent *ev)
//{
//    if(ev->key() == Qt::Key_Escape){
//        this->hide();
//    }
//    QWidget::keyReleaseEvent(ev);
//}

//void Set_Widget::keyPressEvent(QKeyEvent *ev)
//{
//    if(ev->key() == Qt::Key_Escape){
//        this->hide();
//    }
//    QWidget::keyReleaseEvent(ev);
//}

