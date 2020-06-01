#ifndef WIDGETPROXY_H
#define WIDGETPROXY_H

#include <QObject>

//上层与mainwindow通信，下层与codec通信

class widgetProxy : public QObject
{
    Q_OBJECT
public:
    explicit widgetProxy(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WIDGETPROXY_H
