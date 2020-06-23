#ifndef CLOSEDIALOG_H
#define CLOSEDIALOG_H

#include <DDialog>
#include <DWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <DLineEdit>
#include <DPushButton>
#include <DSuggestButton>
#include <DLabel>
DWIDGET_USE_NAMESPACE
class CloseDialog : public DDialog
{
    Q_OBJECT
public:
    CloseDialog(QWidget *parent = nullptr, QString strText = "", QString btnName1 = tr("Cancel"), QString btnName2 = tr("Ok"));

private:
    QVBoxLayout *m_vlayout;
    QHBoxLayout *m_hlayout;
    QHBoxLayout *m_edtlayout;
};

#endif // CLOSEDIALOG_H
