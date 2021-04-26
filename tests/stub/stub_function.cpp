#include "stub_function.h"
extern "C"
{
#include "v4l2_devices.h"
#include "camview.h"
}

v4l2_dev_t *Stub_Function::m_v4l2_dev = nullptr;
v4l2_stream_formats_t *Stub_Function::m_list_stream_formats = nullptr;
v4l2_device_list_t *Stub_Function::m_v4l2_device_list1 = nullptr;
v4l2_device_list_t *Stub_Function::m_v4l2_device_list2 = nullptr;
v4l2_device_list_t *Stub_Function::m_v4l2_device_list3 = nullptr;
v4l2_frame_buff_t *Stub_Function::m_v4l2_frame_buff = nullptr;
 
Stub_Function::Stub_Function()
{

}

//Settings
v4l2_dev_t *Stub_Function::get_v4l2_device_handler()
{
    if (m_v4l2_dev == nullptr) {
        m_v4l2_dev = (v4l2_dev_t *)malloc(sizeof(v4l2_dev_t));
        m_v4l2_dev->list_stream_formats = (v4l2_stream_formats_t *)malloc(sizeof(v4l2_stream_formats_t));
        m_v4l2_dev->list_stream_formats[0].list_stream_cap = (v4l2_stream_cap_t *)malloc(sizeof(v4l2_stream_cap_t));
        m_v4l2_dev->list_stream_formats[0].list_stream_cap[0].width = 640;
        m_v4l2_dev->list_stream_formats[0].list_stream_cap[0].width = 480;
        QString str1 = "/dev/video0";
        m_v4l2_dev->videodevice = (char *)malloc(strlen(str1.toLatin1().data()) + 1);
        strcpy(m_v4l2_dev->videodevice, str1.toLatin1().data());
        m_v4l2_dev->this_device = 0;
    }
    return m_v4l2_dev;
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
    if (m_list_stream_formats == nullptr) {
        m_list_stream_formats = (v4l2_stream_formats_t *)malloc(sizeof(v4l2_stream_formats_t));
        m_list_stream_formats[0].numb_res = 2;
        m_list_stream_formats[0].list_stream_cap = (v4l2_stream_cap_t *)malloc(sizeof(v4l2_stream_formats_t));
        m_list_stream_formats[0].list_stream_cap[0].width = 640;
        m_list_stream_formats[0].list_stream_cap[0].height = 480;
        m_list_stream_formats[0].list_stream_cap[1].width = 960;
        m_list_stream_formats[0].list_stream_cap[1].height = 720;
    }

    return m_list_stream_formats;
}

int Stub_Function::size()
{
    return 2;
}

int Stub_Function::toInt()
{
    return v4l2core_get_frame_width(get_v4l2_device_handler());
}

//thumbnailsbar
Stub_Function::DeviceStatus Stub_Function::getdevStatus()
{
    return CAM_CANUSE;
}

qint64 Stub_Function::msecsTo(const QDateTime &)
{
    return 0;
}

int Stub_Function::get_wayland_status()
{
    return true;
}

bool Stub_Function::isVisible()
{
    return true;
}

int Stub_Function::camInit_OK(const char *devicename)
{
    return E_OK;
}

int Stub_Function::camInit_FORMAT_ERR(const char *devicename)
{
    return E_FORMAT_ERR;
}

int Stub_Function::camInit_NO_DEVICE_ERR(const char *devicename)
{
    return E_NO_DEVICE_ERR;
}

bool Stub_Function::isActive()
{
    return true;
}

bool Stub_Function::isVisible_Countdown()
{
    return true;
}

bool Stub_Function::isVisible_Label()
{
    return true;
}

Stub_Function::DeviceStatus Stub_Function::getdevStatus_CAM_CANUSE()
{
    return CAM_CANUSE;
}

double Stub_Function::get_video_time_capture_hour_5()
{
    return 5;
}

double Stub_Function::get_video_time_capture_hour_60()
{
    return 60;
}

double Stub_Function::get_video_time_capture_hour_21966()
{
    return 21966;
}

double Stub_Function::get_video_time_capture_hour_40271()
{
    return 40271;
}

int Stub_Function::video_capture_get_save_video()
{
    return 1;
}

v4l2_device_list_t *Stub_Function::get_device_list_1()
{

    if (m_v4l2_device_list1 == nullptr) {
        m_v4l2_device_list1 = (v4l2_device_list_t *)malloc(sizeof(v4l2_device_list_t));
        m_v4l2_device_list1->list_devices = new v4l2_dev_sys_data_t[1];
        m_v4l2_device_list1->num_devices = 1;
        QString str1 = "/dev/video0";
        m_v4l2_device_list1->list_devices[0].device = (char *)malloc(strlen(str1.toLatin1().data()) + 1);
        strcpy(m_v4l2_device_list1->list_devices[0].device, str1.toLatin1().data());
    }

    return m_v4l2_device_list1;
}

v4l2_device_list_t *Stub_Function::get_device_list_2()
{

    if (m_v4l2_device_list2 == nullptr) {
        m_v4l2_device_list2 = (v4l2_device_list_t *)malloc(sizeof(v4l2_device_list_t));
        m_v4l2_device_list2->list_devices = new v4l2_dev_sys_data_t[2];
        m_v4l2_device_list2->num_devices = 2;
        QString str1 = "/dev/video0";
        QString str2 = "/dev/video2";
        m_v4l2_device_list2->list_devices[0].device = (char *)malloc(strlen(str1.toLatin1().data()) + 1);
        strcpy(m_v4l2_device_list2->list_devices[0].device, str1.toLatin1().data());
        m_v4l2_device_list2->list_devices[1].device = (char *)malloc(strlen(str2.toLatin1().data()) + 1);
        strcpy(m_v4l2_device_list2->list_devices[1].device, str2.toLatin1().data());
    }

    return m_v4l2_device_list2;

}

v4l2_device_list_t *Stub_Function::get_device_list_3()
{

    if (m_v4l2_device_list3 == nullptr) {
        m_v4l2_device_list3 = (v4l2_device_list_t *)malloc(sizeof(v4l2_device_list_t));
        m_v4l2_device_list3->list_devices = new v4l2_dev_sys_data_t[3];
        m_v4l2_device_list3->num_devices = 3;
        QString str1 = "/dev/video0";
        QString str2 = "/dev/video1";
        QString str3 = "/dev/video2";
        m_v4l2_device_list3->list_devices[0].device = (char *)malloc(strlen(str1.toLatin1().data()) + 1);
        strcpy(m_v4l2_device_list3->list_devices[0].device, str1.toLatin1().data());
        m_v4l2_device_list3->list_devices[1].device = (char *)malloc(strlen(str2.toLatin1().data()) + 1);
        strcpy(m_v4l2_device_list3->list_devices[1].device, str2.toLatin1().data());
        m_v4l2_device_list3->list_devices[2].device = (char *)malloc(strlen(str3.toLatin1().data()) + 1);
        strcpy(m_v4l2_device_list3->list_devices[2].device, str3.toLatin1().data());
    }

    return m_v4l2_device_list3;

}

int Stub_Function::start_encoder_thread()
{
    return 0;
}

void Stub_Function::start()
{

}

int Stub_Function::v4l2core_start_stream(v4l2_dev_t *vd)
{
    return 0;
}

int Stub_Function::v4l2core_stop_stream(v4l2_dev_t *vd)
{
    return 0;
}

v4l2_frame_buff_t *Stub_Function::v4l2core_get_decoded_frame(v4l2_dev_t *vd)
{
    if (m_v4l2_frame_buff == nullptr) {
        m_v4l2_frame_buff = (v4l2_frame_buff_t *)malloc(sizeof (v4l2_frame_buff_t));
        m_v4l2_frame_buff->yuv_frame = (uint8_t *)malloc(sizeof (uint8_t));
    }

    return m_v4l2_frame_buff;
}

int Stub_Function::get_resolution_status()
{
    return 1;
}

void Stub_Function::v4l2core_clean_buffers(v4l2_dev_t *vd)
{

}

int Stub_Function::v4l2core_update_current_format_OK(v4l2_dev_t *vd)
{
    return 0;
}

int Stub_Function::v4l2core_update_current_format_Not_OK(v4l2_dev_t *vd)
{
    return -10;
}

void Stub_Function::v4l2core_prepare_valid_format(v4l2_dev_t *vd)
{

}

void Stub_Function::v4l2core_prepare_valid_resolution(v4l2_dev_t *vd)
{

}

void Stub_Function::set_device_name(const char *name)
{

}

int Stub_Function::v4l2core_get_requested_frame_format(v4l2_dev_t *vd)
{
    return V4L2_PIX_FMT_H264;
}

int Stub_Function::get_capture_pause()
{
    return 1;
}

double Stub_Function::encoder_buff_scheduler(int mode, double thresh, double max_time)
{
    return 1;
}

int Stub_Function::v4l2core_set_h264_frame_rate_config(v4l2_dev_t *vd, uint32_t framerate)
{
    return 0;
}

QVariant Stub_Function::toString()
{
    return "/a";
}

bool Stub_Function::mkdir(const QString &dirName)
{
    return false;
}

QString Stub_Function::currentPath()
{
    return "~/a";
}

QString Stub_Function::suffix()
{
    return "webm";
}

bool Stub_Function::parseFromFile()
{
    return true;
}




