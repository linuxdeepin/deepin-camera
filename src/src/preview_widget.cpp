/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
* Maintainer: hujianbo <hujianbo@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "preview_widget.h"

PreviewWidget::PreviewWidget(DWidget *parent): DWidget (parent)
{

}

void PreviewWidget::setup_ui(DWidget *Form)
{
//    if (this->objectName().isEmpty())
//        this->setObjectName(QStringLiteral("this"));

//    this->resize(700, 800);
//    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy.setHorizontalStretch(0);
//    sizePolicy.setVerticalStretch(0);
//    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
//    this->setSizePolicy(sizePolicy);
//    centralwidget = new DWidget(this);
//    centralwidget->setObjectName(QStringLiteral("centralwidget"));
//    horizontalLayout_7 = new QHBoxLayout(centralwidget);
//    centralwidget->setBackgroundRole(QPalette::Window);

//    horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
//    verticalLayout_6 = new QVBoxLayout();
//    verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
//    horizontalLayout_3 = new QHBoxLayout();
//    horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
//    p_settlbtn = new QToolButton(centralwidget);
//    p_settlbtn->setObjectName(QStringLiteral("p_settlbtn"));
//    p_settlbtn->setArrowType(Qt::NoArrow);
//    p_settlbtn->setCheckable(true);
//    p_settlbtn->setChecked(false);


//    horizontalLayout_3->addWidget(p_settlbtn);

//    horizontalSpacer_5 = new QSpacerItem(988, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    horizontalLayout_3->addItem(horizontalSpacer_5);

//    horizontalLayout_3->setStretch(0, 1);
//    horizontalLayout_3->setStretch(1, 20);

//    verticalLayout_6->addLayout(horizontalLayout_3);

//    horizontalLayout_5 = new QHBoxLayout();
//    horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
//    verticalLayout = new QVBoxLayout();
//    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
//    verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

//    verticalLayout->addItem(verticalSpacer_5);

//    p_selectbtn = new QPushButton(centralwidget);
//    p_selectbtn->setObjectName(QStringLiteral("p_selectbtn"));
//    QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
//    sizePolicy1.setHorizontalStretch(0);
//    sizePolicy1.setVerticalStretch(0);
//    sizePolicy1.setHeightForWidth(p_selectbtn->sizePolicy().hasHeightForWidth());
//    p_selectbtn->setSizePolicy(sizePolicy1);

//    verticalLayout->addWidget(p_selectbtn);

//    verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

//    verticalLayout->addItem(verticalSpacer_4);


//    horizontalLayout_5->addLayout(verticalLayout);

//    horizontalLayout_2 = new QHBoxLayout();
//    horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
//    horizontalSpacer_7 = new QSpacerItem(40, 13, QSizePolicy::Fixed, QSizePolicy::Minimum);

//    horizontalLayout_2->addItem(horizontalSpacer_7);

//    verticalLayout_5 = new QVBoxLayout();
//    verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
//    verticalSpacer_8 = new QSpacerItem(13, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

//    verticalLayout_5->addItem(verticalSpacer_8);

//    p_preview = new PreviewWidget(centralwidget);
//    p_preview->setObjectName(QStringLiteral("p_preview"));
//    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    sizePolicy3.setHorizontalStretch(0);
//    sizePolicy3.setVerticalStretch(0);
//    sizePolicy3.setHeightForWidth(p_preview->sizePolicy().hasHeightForWidth());
//    p_preview->setSizePolicy(sizePolicy3);
//    p_preview->setMinimumSize(QSize(480, 320));
//    p_preview->setStyleSheet(QString::fromUtf8("background:url(:/\346\226\260\345\211\215\347\274\200/desktop.jpg);"));

//    verticalLayout_5->addWidget(p_preview);

//    verticalSpacer_9 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

//    verticalLayout_5->addItem(verticalSpacer_9);


//    horizontalLayout_2->addLayout(verticalLayout_5);

//    horizontalSpacer_6 = new QSpacerItem(40, 13, QSizePolicy::Fixed, QSizePolicy::Minimum);

//    horizontalLayout_2->addItem(horizontalSpacer_6);


//    horizontalLayout_5->addLayout(horizontalLayout_2);

//    verticalLayout_4 = new QVBoxLayout();
//    verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
//    verticalSpacer_7 = new QSpacerItem(20, 80, QSizePolicy::Minimum, QSizePolicy::Fixed);

//    verticalLayout_4->addItem(verticalSpacer_7);

//    p_verline = new QFrame(centralwidget);
//    p_verline->setObjectName(QStringLiteral("p_verline"));
//    p_verline->setWindowModality(Qt::NonModal);
//    QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Expanding);
//    sizePolicy2.setHorizontalStretch(0);
//    sizePolicy2.setVerticalStretch(0);
//    sizePolicy2.setHeightForWidth(p_verline->sizePolicy().hasHeightForWidth());
//    p_verline->setSizePolicy(sizePolicy2);
//    p_verline->setMinimumSize(QSize(20, 350));
//    p_verline->setLayoutDirection(Qt::LeftToRight);
//    p_verline->setFrameShadow(QFrame::Sunken);
//    p_verline->setMidLineWidth(0);
//    p_verline->setFrameShape(QFrame::VLine);

//    verticalLayout_4->addWidget(p_verline);

//    verticalSpacer_6 = new QSpacerItem(20, 35, QSizePolicy::Minimum, QSizePolicy::Fixed);

//    verticalLayout_4->addItem(verticalSpacer_6);

//    verticalLayout_4->setStretch(0, 1);
//    verticalLayout_4->setStretch(1, 10);
//    verticalLayout_4->setStretch(2, 1);

//    horizontalLayout_5->addLayout(verticalLayout_4);

//    verticalLayout_2 = new QVBoxLayout();
//    verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
//    verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

//    verticalLayout_2->addItem(verticalSpacer_3);

//    p_takecambtn = new QPushButton(centralwidget);
//    p_takecambtn->setObjectName(QStringLiteral("p_takecambtn"));
//    sizePolicy1.setHeightForWidth(p_takecambtn->sizePolicy().hasHeightForWidth());
//    p_takecambtn->setSizePolicy(sizePolicy1);
//    p_takecambtn->setCheckable(true);
//    p_takecambtn->setAutoExclusive(true);

//    verticalLayout_2->addWidget(p_takecambtn);

//    verticalSpacer = new QSpacerItem(20, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);

//    verticalLayout_2->addItem(verticalSpacer);

//    p_takevideobtn = new QPushButton(centralwidget);
//    p_takevideobtn->setObjectName(QStringLiteral("p_takevideobtn"));
//    sizePolicy1.setHeightForWidth(p_takevideobtn->sizePolicy().hasHeightForWidth());
//    p_takevideobtn->setSizePolicy(sizePolicy1);
//    p_takevideobtn->setCheckable(true);
//    p_takevideobtn->setAutoExclusive(true);

//    verticalLayout_2->addWidget(p_takevideobtn);

//    verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

//    verticalLayout_2->addItem(verticalSpacer_2);

//    verticalLayout_2->setStretch(0, 10);
//    verticalLayout_2->setStretch(1, 2);
//    verticalLayout_2->setStretch(2, 1);
//    verticalLayout_2->setStretch(3, 2);
//    verticalLayout_2->setStretch(4, 10);

//    horizontalLayout_5->addLayout(verticalLayout_2);

//    horizontalLayout_5->setStretch(0, 1);
//    horizontalLayout_5->setStretch(1, 20);
//    horizontalLayout_5->setStretch(2, 1);
//    horizontalLayout_5->setStretch(3, 1);

//    verticalLayout_6->addLayout(horizontalLayout_5);

//    verticalLayout_3 = new QVBoxLayout();
//    verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
//    horizontalLayout = new QHBoxLayout();
//    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
//    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    horizontalLayout->addItem(horizontalSpacer);

//    p_timelabl = new QLabel(centralwidget);
//    p_timelabl->setMinimumSize(QSize(100, 22));
//    p_timelabl->setObjectName(QStringLiteral("p_timelabl"));
//    p_timelabl->setAlignment(Qt::AlignCenter);
//    p_timelabl->setWordWrap(false);

//    horizontalLayout->addWidget(p_timelabl);

//    horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    horizontalLayout->addItem(horizontalSpacer_2);


//    verticalLayout_3->addLayout(horizontalLayout);

//    horizontalLayout_4 = new QHBoxLayout();
//    horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
//    horizontalSpacer_8 = new QSpacerItem(50, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

//    horizontalLayout_4->addItem(horizontalSpacer_8);

//    p_horline = new QFrame(centralwidget);
//    p_horline->setObjectName(QStringLiteral("p_horline"));
//    QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Fixed);
//    sizePolicy6.setHorizontalStretch(0);
//    sizePolicy6.setVerticalStretch(0);
//    sizePolicy6.setHeightForWidth(p_horline->sizePolicy().hasHeightForWidth());
//    p_horline->setSizePolicy(sizePolicy6);
//    p_horline->setMinimumSize(QSize(500, 30));
//    p_horline->setLineWidth(1);
//    p_horline->setMidLineWidth(0);

//    p_horline->setFrameShape(QFrame::HLine);
//    p_horline->setFrameShadow(QFrame::Sunken);

//    horizontalLayout_4->addWidget(p_horline);

//    horizontalSpacer_9 = new QSpacerItem(120, 60, QSizePolicy::Fixed, QSizePolicy::Minimum);

//    horizontalLayout_4->addItem(horizontalSpacer_9);


//    verticalLayout_3->addLayout(horizontalLayout_4);

//    horizontalLayout_6 = new QHBoxLayout();
//    horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
//    horizontalSpacer_3 = new QSpacerItem(68, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    horizontalLayout_6->addItem(horizontalSpacer_3);

//    p_scrlarea = new QScrollArea(centralwidget);
//    p_scrlarea->setObjectName(QStringLiteral("p_scrlarea"));
//    p_scrlarea->setEnabled(true);
//    p_scrlarea->setMinimumSize(QSize(120, 120));
//    p_scrlarea->setFrameShape(QFrame::StyledPanel);
//    p_scrlarea->setFrameShadow(QFrame::Sunken);
//    p_scrlarea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    p_scrlarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    p_scrlarea->setWidgetResizable(true);
//    scrollAreaWidgetContents = new QWidget();
//    scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
//    scrollAreaWidgetContents->setGeometry(QRect(0, 0, 388, 118));
//    p_scrlarea->setWidget(scrollAreaWidgetContents);

//    horizontalLayout_6->addWidget(p_scrlarea);
//    //horizontalLayout_6->addWidget(m_thumbnail);
//    horizontalLayout_6->setContentsMargins(-1, -1, -1, 10);

//    horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    horizontalLayout_6->addItem(horizontalSpacer_4);


//    verticalLayout_3->addLayout(horizontalLayout_6);

//    verticalLayout_3->setStretch(0, 1);
//    verticalLayout_3->setStretch(1, 1);
//    verticalLayout_3->setStretch(2, 5);

//    verticalLayout_6->addLayout(verticalLayout_3);

//    verticalLayout_6->setStretch(0, 1);
//    verticalLayout_6->setStretch(1, 40);
//    verticalLayout_6->setStretch(2, 8);

//    horizontalLayout_7->addLayout(verticalLayout_6);

//    this->setCentralWidget(centralwidget);
//    QWidget::setTabOrder(p_scrlarea, p_takecambtn);
//    QWidget::setTabOrder(p_takecambtn, p_takevideobtn);

//    p_setwdt = new Set_Widget(centralwidget);
//    p_setwdt->setBackgroundRole(QPalette::Background);
//    p_setwdt->setAutoFillBackground(true);
//    QPalette *plette = new QPalette();

//    plette->setBrush(QPalette::Background, QBrush(QColor(64, 64, 64, 180), Qt::SolidPattern));
//    plette->setBrush(QPalette::WindowText, QBrush(QColor(255, 255, 255, 255), Qt::SolidPattern));
//    p_setwdt->setPalette(*plette);

//    //p_setwdt->setWindowOpacity(0.5);
//    p_setwdt->update();
//    p_setwdt->setGeometry(0, 15 + p_settlbtn->height(), this->width(), this->height() - p_settlbtn->height());

//    retranslateUi();

//    QMetaObject::connectSlotsByName(this);
}

void PreviewWidget::retranslateUi()
{
//    this->setWindowTitle(DApplication::translate("this", "this", nullptr));
//    p_settlbtn->setText(DApplication::translate("this", "设置", nullptr));
//    p_selectbtn->setText(DApplication::translate("this", "\345\210\207\346\215\242", nullptr));
//    p_takecambtn->setText(DApplication::translate("this", "\346\213\215\347\205\247", nullptr));
//    p_takevideobtn->setText(DApplication::translate("this", "\345\275\225\345\203\217", nullptr));
//    p_timelabl->setText(DApplication::translate("this", "00:00:00", nullptr));
}
