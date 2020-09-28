#include "LPF_V4L2Test.h"

LPF_V4L2Test::LPF_V4L2Test()
{
}
TEST_F(LPF_V4L2Test, camInit)
{
    camInit("dev/video0");
    camInit("dev/video1");
    camInit("dev/video2");
    camInit("dev/video3");
}

