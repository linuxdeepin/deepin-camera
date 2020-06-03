#ifndef DEVMONITORTHREAD_H
#define DEVMONITORTHREAD_H

#include <QWidget>

class DevMonitorThread : public QWidget
{
    Q_OBJECT
public:
    explicit DevMonitorThread(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // DEVMONITORTHREAD_H