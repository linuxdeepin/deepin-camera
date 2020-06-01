#ifndef CAMERADETECT_H
#define CAMERADETECT_H

#include <QObject>

//设备通信

class CameraDetect : public QObject
{
    Q_OBJECT
public:
    explicit CameraDetect(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CAMERADETECT_H
