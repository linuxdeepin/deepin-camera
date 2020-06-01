#include "actiontoken.h"
#include <QDebug>
#include "LPF_V4L2.h"

actionToken::actionToken(QObject *parent) : QObject(parent)
{
    is_active = 0;
    encode_thread = new encode_voice_Thread();
}

void actionToken::onTakePic()
{
    qDebug() << "onTakePic";
}

void actionToken::onThreeShots()
{
    qDebug() << "onThreeShots";
}

void actionToken::onTakeVideo()
{
    qDebug() << "onTakeVideo";

    if (is_active) {
        encode_thread->stop();
        is_active = 0;
        reset_video_timer();
    } else {
        encode_thread->start();
        is_active = 1;
    }
}



