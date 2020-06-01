#ifndef EFFECTPROXY_H
#define EFFECTPROXY_H

#include <QObject>


//特效代理
class effectproxy : public QObject
{
    Q_OBJECT
public:
    explicit effectproxy(QObject *parent = nullptr);

signals:

public slots:
};

#endif // EFFECTPROXY_H
