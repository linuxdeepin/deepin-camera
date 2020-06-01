#ifndef AVCODEC_H
#define AVCODEC_H

#include <QObject>

//解码库，服务于音视频数据，为上层提供数据

class avCodec : public QObject
{
    Q_OBJECT
public:
    explicit avCodec(QObject *parent = nullptr);

signals:

public slots:
};

#endif // AVCODEC_H
