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

    //MajorImageProcessingThread
    //开始启动流
    int v4l2core_start_stream(v4l2_dev_t *vd);
    //停止启动流
    int v4l2core_stop_stream(v4l2_dev_t *vd);
    //获得解码数据库帧
    v4l2_frame_buff_t *v4l2core_get_decoded_frame(v4l2_dev_t *vd);
    //获取重启状态
    int get_resolution_status();
    //清理缓存区
    void v4l2core_clean_buffers(v4l2_dev_t *vd);
    //更新当前格式
    int v4l2core_update_current_format_OK(v4l2_dev_t *vd);//返回零
    int v4l2core_update_current_format_Not_OK(v4l2_dev_t *vd);//返回非零
    //有效格式
    void v4l2core_prepare_valid_format(v4l2_dev_t *vd);
    //宽度高度获取
    void v4l2core_prepare_valid_resolution(v4l2_dev_t *vd);
    //设置设备名
    void set_device_name(const char *name);
    //进入V4L2_PIX_FMT_H264分支
    int v4l2core_get_requested_frame_format(v4l2_dev_t *vd);
    //获得暂停标志
    int get_capture_pause();
    //编码器任务调度
    double encoder_buff_scheduler(int mode, double thresh, double max_time);
    //h264帧率设置
    int v4l2core_set_h264_frame_rate_config(v4l2_dev_t *vd, uint32_t framerate);

public:
    static v4l2_dev_t *m_v4l2_dev;
    static v4l2_stream_formats_t *m_list_stream_formats;
    static v4l2_device_list_t *m_v4l2_device_list1;
    static v4l2_device_list_t *m_v4l2_device_list2;
    static v4l2_frame_buff_t *m_v4l2_frame_buff;
};

#endif // STUB_FUNCTION_H
