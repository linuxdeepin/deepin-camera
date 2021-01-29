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
    enum DeviceStatus {NOCAM, CAM_CANNOT_USE, CAM_CANUSE};
    Stub_Function();
    //Setting
    v4l2_dev_t *get_v4l2_device_handler();
    int v4l2core_get_frame_format_index(v4l2_dev_t *vd, int format);
    int v4l2core_get_format_resolution_index(v4l2_dev_t *vd, int format, int width, int height);
    v4l2_stream_formats_t *v4l2core_get_formats_list(v4l2_dev_t *vd);
    int size();
    int toInt();

    v4l2_device_list_t m_devlist2;
    //thumbnailsbar
    enum DeviceStatus getdevStatus();
    qint64 msecsTo(const QDateTime &);

    //videowidget
    int get_wayland_status();//获取wayland状态函数打桩
    //调用delayInit分支
    int camInit_OK(const char *devicename);//返回值为E_OK(进入该分支会出现错误)
    int camInit_FORMAT_ERR(const char *devicename);//返回值为E_FORMAT_ERR
    int camInit_NO_DEVICE_ERR(const char *devicename);//返回值为E_NO_DEVICE_ERR
    //调用showCountdown分支
    bool isActive();//定时器使用中函数打桩
    bool isVisible_Countdown();//倒计时可见函数打桩
    bool isVisible_Label();//闪光灯Label可见函数打桩
    enum DeviceStatus getdevStatus_CAM_CANUSE();//获取相机可用状态函数打桩
    //调用showRecTime
    //获取写video的时间m_nCount打桩函数
    double get_video_time_capture_hour_5();//0时0分5秒,小于3为不正常时长
    double get_video_time_capture_hour_60();//0时1分0秒,调用秒数为0分支
    double get_video_time_capture_hour_21966();//6时6分6秒
    double get_video_time_capture_hour_40271();//11时11分11秒
    //调用onEndBtnClicked
    int video_capture_get_save_video();
    //获取设备列表，进入设备数目为1分支
    v4l2_device_list_t *get_device_list_1();
    //获取设备列表，进入设备数目为2分支
    v4l2_device_list_t *get_device_list_2();
    //开启线程打桩
    int start_encoder_thread();
    //MajorImageProcessingThread开始函数打桩
    void start();




};

#endif // STUB_FUNCTION_H
