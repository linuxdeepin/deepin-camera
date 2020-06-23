#include "closedialog.h"
#include <DMessageBox>
#include <QRegExp>

#include <DLabel>
#include <QDebug>

DWIDGET_USE_NAMESPACE
typedef DLabel QLbtoDLabel;

CloseDialog::CloseDialog(QWidget *parent, QString strText, QString btnName1, QString btnName2)
    : DDialog(parent)
{
    this->setIcon(QIcon(":/images/icons/warning.svg"));
    setFixedSize(380, 140);
    DWidget *widet = new DWidget(this);
    addContent(widet);
    m_vlayout = new QVBoxLayout(widet);
    m_hlayout = new QHBoxLayout();
    m_edtlayout = new QHBoxLayout();
    m_vlayout->setContentsMargins(2, 0, 2, 1);
    m_vlayout->addStretch();
    addButton(btnName1);
    addButton(btnName2);
    QLbtoDLabel *labtitle = new QLbtoDLabel();
    labtitle->setText(strText);
    labtitle->setAlignment(Qt::AlignCenter);
    m_vlayout->addWidget(labtitle);
    m_vlayout->addStretch();
    m_vlayout->addLayout(m_hlayout);
    widet->setLayout(m_vlayout);
}
