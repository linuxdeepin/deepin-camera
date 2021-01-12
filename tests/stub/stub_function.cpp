#include "stub_function.h"
extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}

Stub_Function::Stub_Function()
{

}

//Settings
v4l2_dev_t *Stub_Function::get_v4l2_device_handler()
{
    v4l2_dev_t *v4l2_devlist = (v4l2_dev_t *)malloc(sizeof(v4l2_dev_t));
    return v4l2_devlist;
}

int Stub_Function::v4l2core_get_frame_format_index(v4l2_dev_t *vd, int format)
{
    return 0;
}
int Stub_Function::v4l2core_get_format_resolution_index(v4l2_dev_t *vd, int format, int width, int height)
{
    return 0;
}

v4l2_stream_formats_t *Stub_Function::v4l2core_get_formats_list(v4l2_dev_t *vd)
{
    v4l2_stream_formats_t *list_stream_formats = (v4l2_stream_formats_t *)malloc(sizeof(v4l2_stream_formats_t));
    list_stream_formats[0].numb_res = 2;
    list_stream_formats[0].list_stream_cap = (v4l2_stream_cap_t *)malloc(sizeof(v4l2_stream_formats_t));
    list_stream_formats[0].list_stream_cap[0].width = 640;
    list_stream_formats[0].list_stream_cap[0].height = 480;
    list_stream_formats[0].list_stream_cap[1].width = 960;
    list_stream_formats[0].list_stream_cap[1].height = 720;
    return list_stream_formats;
}

int Stub_Function::size()
{
    return 2;
}

int Stub_Function::toInt()
{
    return v4l2core_get_frame_width(get_v4l2_device_handler());
}


