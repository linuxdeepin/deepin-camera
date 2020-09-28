#ifndef GTESTVIEW_H
#define GTESTVIEW_H

//#ifdef __cplusplus
//extern "C" {
//#endif

//#include "../libcam/libcam/camview.h"
//#include "../libcam/libcam_audio/gviewaudio.h"
//#include "../libcam/libcam_v4l2core/gviewv4l2core.h"
//#include "../libcam/libcam_v4l2core/v4l2_devices.h"
//#include "../libcam/libcam_v4l2core/v4l2_core.h"
//#include "../libcam/libcam_encoder/gviewencoder.h"
//#include "../libcam/libcam_render/gviewrender.h"
//#include "../libcam/libcam_audio/audio.h"
//#include "../libcam/libcam_v4l2core/core_io.h"

//#ifdef __cplusplus
//}
//#endif

//#include <QTest>


#include <QtTest/QTest>
#include <DApplication>
#include "../src/src/LPF_V4L2.h"
#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

class LPF_V4L2Test : public ::testing::Test
{
public:
    LPF_V4L2Test();
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }
};

#endif // GTESTVIEW_H
