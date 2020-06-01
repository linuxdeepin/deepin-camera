#ifndef ENCODE_VOICE_H
#define ENCODE_VOICE_H

#include <QThread>

class encode_voice_Thread : public QThread
{
    Q_OBJECT
public:
    encode_voice_Thread();
    void init();
    void stop();

protected:
    void run();

};

#endif // ENCODE_VOICE_H
