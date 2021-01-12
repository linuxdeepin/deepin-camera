#ifndef STUB_FUNCTION_H
#define STUB_FUNCTION_H

#include <QObject>
#include "../../src/devnummonitor.h"
#include "../../src/imageitem.h"
#include "MainwindowTest.h"
extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}
class Stub_Function
{
public:
    Stub_Function();
    //Setting
    v4l2_dev_t *get_v4l2_device_handler();
    int v4l2core_get_frame_format_index(v4l2_dev_t *vd, int format);
    int v4l2core_get_format_resolution_index(v4l2_dev_t *vd, int format, int width, int height);
    v4l2_stream_formats_t *v4l2core_get_formats_list(v4l2_dev_t *vd);
    int size();
    int toInt();

    v4l2_device_list_t m_devlist2;
};

#endif // STUB_FUNCTION_H
