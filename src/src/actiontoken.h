#ifndef ACTIONTOKEN_H
#define ACTIONTOKEN_H

#include <QObject>
#include "encode_voice.h"

//sct
//拍照 录像 连拍 按钮联动 具体功能实现
class actionToken : public QObject
{
    Q_OBJECT
public:
    explicit actionToken(QObject *parent = nullptr);

signals:

public slots:
    void onTakePic();
    void onThreeShots();
    void onTakeVideo();

private:
    int is_active;
    encode_voice_Thread *encode_thread;

};

#endif // ACTIONTOKEN_H
