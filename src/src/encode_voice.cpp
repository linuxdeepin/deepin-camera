#include "encode_voice.h"
#include "LPF_V4L2.h"

encode_voice_Thread::encode_voice_Thread()
{

}

void encode_voice_Thread::init()
{

}

void encode_voice_Thread::stop()
{
    stop_encoder_thread();
}

void encode_voice_Thread::run()
{
    start_encoder_thread();
}
