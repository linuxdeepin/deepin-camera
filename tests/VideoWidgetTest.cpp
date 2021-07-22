/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     wuzhigang <wuzhigang@uniontech.com>
* Maintainer: wuzhigang <wuzhigang@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "VideoWidgetTest.h"
#include "src/videowidget.h"
#include "stub.h"
#include "stub_function.h"
#include "src/capplication.h"
#include "ac-deepin-camera-define.h"
#include "stub/addr_pri.h"
#include "datamanager.h"


ACCESS_PRIVATE_FUN(videowidget, void(), showNocam);
ACCESS_PRIVATE_FUN(videowidget, void(), showCamUsed);
ACCESS_PRIVATE_FUN(videowidget, void(), startTakeVideo);
ACCESS_PRIVATE_FUN(videowidget, void(), itemPosChange);
ACCESS_PRIVATE_FUN(videowidget, void(), stopEverything);
ACCESS_PRIVATE_FUN(videowidget, void(), recoverTabWidget);
ACCESS_PRIVATE_FUN(videowidget, void(), onReachMaxDelayedFrames);
ACCESS_PRIVATE_FUN(videowidget, void(), flash);
ACCESS_PRIVATE_FUN(videowidget, void(QGraphicsView *view), forbidScrollBar);
ACCESS_PRIVATE_FUN(videowidget, void(PRIVIEW_ENUM_STATE state), showCountDownLabel);
ACCESS_PRIVATE_FUN(videowidget, void(const QString &resolution), slotresolutionchanged);

ACCESS_PRIVATE_FIELD(videowidget, QGraphicsTextItem *, m_pCamErrItem);
ACCESS_PRIVATE_FIELD(videowidget, DLabel *, m_dLabel);
ACCESS_PRIVATE_FIELD(videowidget, DLabel *, m_flashLabel);
ACCESS_PRIVATE_FIELD(videowidget, PreviewOpenglWidget *, m_openglwidget);
//ACCESS_PRIVATE_FIELD(videowidget, ThumbnailsBar *, m_thumbnail);
ACCESS_PRIVATE_FIELD(videowidget, QGraphicsView *, m_pNormalView);
ACCESS_PRIVATE_FIELD(videowidget, int, m_nInterval);
ACCESS_PRIVATE_FIELD(videowidget, int, m_Maxinterval);
ACCESS_PRIVATE_FIELD(videowidget, int, m_nCount);
ACCESS_PRIVATE_FIELD(videowidget, int, m_curTakePicTime);

ACCESS_PRIVATE_FIELD(MajorImageProcessingThread, QAtomicInt, m_stopped);

VideoWidgetTest::VideoWidgetTest()
{
    mainwindow = CamApp->getMainWindow();
}

VideoWidgetTest::~VideoWidgetTest()
{
    mainwindow = NULL;
}

void VideoWidgetTest::SetUp()
{
   
}

void VideoWidgetTest::TearDown()
{

}

TEST_F(VideoWidgetTest, videowidget)
{
    videowidget *videowidgt = mainwindow->findChild<videowidget *>(VIDEO_PREVIEW_WIDGET);

    //调用videowidget构造函数中的wayland平台设置背景色为黑色分支
    Stub stub;
    stub.set(get_wayland_status, ADDR(Stub_Function, get_wayland_status));
    DGuiApplicationHelper::instance()->setThemeType(DGuiApplicationHelper::DarkType);
    videowidget *videowidgetTest = new videowidget();
    stub.reset(get_wayland_status);

    //调用delayInit函数
    QTest::qWait(500);
    //调用无设备状态分支
    DataManager::instance()->setdevStatus(CAM_CANNOT_USE);
    stub.set(camInit, ADDR(Stub_Function, camInit_NO_DEVICE_ERR));
    videowidgetTest->delayInit();
    //启动失败分支
    stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));
    videowidgetTest->delayInit();
    DGuiApplicationHelper::instance()->setPaletteType(DGuiApplicationHelper::LightType);
    videowidgetTest->delayInit();
    //启动成功分支
    stub.set(camInit, ADDR(Stub_Function, camInit_OK));
    stub.set(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
    videowidgetTest->delayInit();

    stub.reset(camInit);
    stub.reset(ADDR(MajorImageProcessingThread, start));

    //调用showNocam函数
    QTest::qWait(500);
    call_private_fun::videowidgetshowNocam(*videowidgt);
    DGuiApplicationHelper::instance()->setThemeType(DGuiApplicationHelper::LightType);//切换主题
    call_private_fun::videowidgetshowNocam(*videowidgt);

    //调用showCamUsed函数
    QTest::qWait(500);
    call_private_fun::videowidgetshowCamUsed(*videowidgt);
    DGuiApplicationHelper::instance()->setThemeType(DGuiApplicationHelper::DarkType);//切换主题
    call_private_fun::videowidgetshowCamUsed(*videowidgt);

    //调用onReachMaxDelayedFrames函数
    QTest::qWait(500);
    call_private_fun::videowidgetonReachMaxDelayedFrames(*videowidgt);

    //调用showCountDownLabel函数
    call_private_fun::videowidgetshowCountDownLabel(*videowidgt, PICTRUE);//调用PICTRUE分支
    call_private_fun::videowidgetshowCountDownLabel(*videowidgt, NODEVICE);//调用NODEVICE分支
    call_private_fun::videowidgetshowCountDownLabel(*videowidgt, VIDEO);//调用VIDEO分支,进入视频调用0时0分0秒分支

    access_private_field::videowidgetm_nCount(*videowidgt) = 21966;
    call_private_fun::videowidgetshowCountDownLabel(*videowidgt, VIDEO);//调用VIDEO分支，进入视频调用6时6分6秒分支
    access_private_field::videowidgetm_nCount(*videowidgt) = 40271;
    call_private_fun::videowidgetshowCountDownLabel(*videowidgt, VIDEO);//调用VIDEO分支，进入视频调用11时11分11秒分支
    access_private_field::videowidgetm_nCount(*videowidgt) = 0;
    //调用showCountdown函数
    //无相机停止定时器
    stub.set(ADDR(QTimer, isActive), ADDR(Stub_Function, isActive));
    //倒计时显示隐藏打桩
    stub.set(ADDR(DFloatingWidget, isVisible), ADDR(Stub_Function, isVisible_Countdown));
    //倒计时laber显示隐藏打桩
    stub.set(ADDR(DLabel, isVisible), ADDR(Stub_Function, isVisible_Label));

    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    //调用无相机分支
    videowidgt->showCountdown();
    //调用相机可用分支下的拍照连拍次数为0分支
    DataManager::instance()->setdevStatus(CAM_CANUSE);
    access_private_field::videowidgetm_curTakePicTime(*videowidgt) = 0;
    access_private_field::videowidgetm_nInterval(*videowidgt) = 0;

    videowidgt->showCountdown();
    //stub.reset(start_encoder_thread);
    //调用相机可用分支下的拍照连拍次数不为0分支
    access_private_field::videowidgetm_curTakePicTime(*videowidgt) = 1;
    videowidgt->showCountdown();
    //桩函数还原
    stub.reset(ADDR(DFloatingWidget, isVisible));
    stub.reset(ADDR(DLabel, isVisible));
    stub.reset(ADDR(QTimer, isActive));
    DataManager::instance()->setdevStatus(NOCAM);

    //调用showCountdown函数
    //调用时长不正常分支
    videowidgt->showRecTime();
    //由于时长m_nCount小于等于3视为为不正常时长，需要调用两次调用才能进入0时0分0秒状态
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_5));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_60));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);
    //调用6时6分6秒
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_21966));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);
    //调用11时11分11秒
    stub.set(get_video_time_capture, ADDR(Stub_Function, get_video_time_capture_hour_40271));
    videowidgt->showRecTime();
    stub.reset(get_video_time_capture);

    //调用flash函数
    access_private_field::videowidgetm_flashLabel(*videowidgt)->setVisible(true);
    call_private_fun::videowidgetflash(*videowidgt);
    access_private_field::videowidgetm_flashLabel(*videowidgt)->setVisible(false);

    //调用slotresolutionchanged函数
    //调用相机不可用，则直接返回
    call_private_fun::videowidgetslotresolutionchanged(*videowidgt, "1184x656");
    //调用相机可用
    DataManager::instance()->setdevStatus(CAM_CANUSE);
    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    call_private_fun::videowidgetslotresolutionchanged(*videowidgt, "1184x656");
    stub.reset(get_v4l2_device_handler);
    DataManager::instance()->setdevStatus(NOCAM);
    //调用onEndBtnClicked函数
    videowidgt->setCapStatus(true);
    DataManager::instance()->m_tabIndex = 9;
    //录像标志函数打桩
    stub.set(video_capture_get_save_video, ADDR(Stub_Function, video_capture_get_save_video));
    videowidgt->onEndBtnClicked();
    DataManager::instance()->m_tabIndex = 2;
    stub.reset(video_capture_get_save_video);

    //调用onChangeDev函数
    //进入设备数为2的无设备分支
    stub.set(get_device_list, ADDR(Stub_Function, get_device_list_2));
    videowidgt->onChangeDev();
    //进入设备数为2启动失败分支
    stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));
    videowidgt->onChangeDev();
    //进入设备数为2启动成功分支
    stub.set(camInit, ADDR(Stub_Function, camInit_OK));
    stub.set(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
//    videowidgt->onChangeDev();
    //桩函数还原
    stub.reset(camInit);
    stub.reset(get_device_list);
    //进入设备数为1和3的无设备分支
    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            stub.set(get_device_list, ADDR(Stub_Function, get_device_list_1));//设备数为1
        }
        if (i == 1) {
            stub.set(get_device_list, ADDR(Stub_Function, get_device_list_3));//设备数为2
            QTest::qWait(1010);
        }
//        stub.set(camInit, ADDR(Stub_Function, camInit_OK));//OK分支
//        videowidgt->onChangeDev();
//        stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));//E_FORMAT_ERR分支
//        videowidgt->onChangeDev();
//        stub.set(camInit, ADDR(Stub_Function, camInit_NO_DEVICE_ERR));//E_NO_DEVICE_ERR分支
//        videowidgt->onChangeDev();
    }

//    stub.reset(get_v4l2_device_handler);
//    //进入设备数为3,str为空
//    stub.set(camInit, ADDR(Stub_Function, camInit_OK));//设备数为1,str=str1,OK分支
//    videowidgt->onChangeDev();
//    stub.set(camInit, ADDR(Stub_Function, camInit_FORMAT_ERR));//设备数为1,str=str1,E_FORMAT_ERR分支
//    videowidgt->onChangeDev();
//    stub.set(camInit, ADDR(Stub_Function, camInit_NO_DEVICE_ERR));//设备数为1,str=str1,E_NO_DEVICE_ERR分支
//    videowidgt->onChangeDev();
//    stub.reset(camInit);

    //还原桩函数
    stub.reset(camInit);
    stub.reset(ADDR(MajorImageProcessingThread, start));
    stub.reset(get_device_list);

//    //调用onRestartDevices函数
//    stub.set(get_device_list, ADDR(Stub_Function, get_device_list_2));
//    stub.set(camInit, ADDR(Stub_Function, camInit_OK));//OK分支
//    stub.set(ADDR(MajorImageProcessingThread, start), ADDR(Stub_Function, start));
//    videowidgt->onRestartDevices();
//    stub.reset(camInit);
//    stub.reset(ADDR(MajorImageProcessingThread, start));
//    stub.reset(get_device_list);
//    videowidgt->onChangeDev();//无相机状态还原

    //调用onTakePic函数

    access_private_field::videowidgetm_pCamErrItem(*videowidgt)->setVisible(true);
    access_private_field::videowidgetm_dLabel(*videowidgt)->setVisible(true);
    access_private_field::MajorImageProcessingThreadm_stopped(*videowidgt->m_imgPrcThread) = 0;
    access_private_field::videowidgetm_flashLabel(*videowidgt)->setVisible(true);
    access_private_field::videowidgetm_openglwidget(*videowidgt)->setVisible(false);
//    access_private_field::videowidgetm_thumbnail(*videowidgt)->setVisible(false);
    videowidgt->onTakePic(true);
    videowidgt->onTakePic(false);
    access_private_field::videowidgetm_pCamErrItem(*videowidgt)->setVisible(false);
    access_private_field::videowidgetm_dLabel(*videowidgt)->setVisible(false);
    access_private_field::MajorImageProcessingThreadm_stopped(*videowidgt->m_imgPrcThread) = 1;
    access_private_field::videowidgetm_flashLabel(*videowidgt)->setVisible(false);
    access_private_field::videowidgetm_openglwidget(*videowidgt)->setVisible(true);
//    access_private_field::videowidgetm_thumbnail(*videowidgt)->setVisible(true);

    //调用onTakeVideo函数
    //进入倒计时期间处理
    access_private_field::videowidgetm_nInterval(*videowidgt) = 1;
    videowidgt->onTakeVideo();
    //录制完成处理
    videowidgt->setCapStatus(true);
    videowidgt->onTakeVideo();
    //进入最大间隔为0直接录制分支
    access_private_field::videowidgetm_nInterval(*videowidgt) = 0;
    videowidgt->setCapStatus(false);
    access_private_field::videowidgetm_Maxinterval(*videowidgt) = 0;
    videowidgt->onTakeVideo();
    //进入最大间隔不为0分支
    access_private_field::videowidgetm_Maxinterval(*videowidgt) = 3;
    videowidgt->onTakeVideo();

    //调用forbidScrollBar函数
    call_private_fun::videowidgetforbidScrollBar(*videowidgt, access_private_field::videowidgetm_pNormalView(*videowidgt));

    //调用startTakeVideo函数
    //录制完成处理
    stub.set(start_encoder_thread, ADDR(Stub_Function, start_encoder_thread));
    videowidgt->setCapStatus(true);
    call_private_fun::videowidgetstartTakeVideo(*videowidgt);
    videowidgt->setCapStatus(false);
    //调用相机不可用分支
    call_private_fun::videowidgetstartTakeVideo(*videowidgt);
    //调用相机可用分支
    DataManager::instance()->setdevStatus(CAM_CANUSE);
    call_private_fun::videowidgetstartTakeVideo(*videowidgt);
    //stub.reset(start_encoder_thread);

    //调用itemPosChange函数
    call_private_fun::videowidgetitemPosChange(*videowidgt);

    //调用stopEverything函数
    access_private_field::videowidgetm_flashLabel(*videowidgt)->setVisible(true);
    access_private_field::videowidgetm_pCamErrItem(*videowidgt)->setVisible(true);
    access_private_field::videowidgetm_openglwidget(*videowidgt)->setVisible(false);
    call_private_fun::videowidgetstopEverything(*videowidgt);
    access_private_field::videowidgetm_flashLabel(*videowidgt)->setVisible(false);
    access_private_field::videowidgetm_pCamErrItem(*videowidgt)->setVisible(false);
    access_private_field::videowidgetm_openglwidget(*videowidgt)->setVisible(true);

    //调用recoverTabWidget函数
    DataManager::instance()->setNowTabIndex(2);
    DataManager::instance()->m_tabIndex = 1;//选中相机选择按钮
    call_private_fun::videowidgetrecoverTabWidget(*videowidgt);
    DataManager::instance()->m_tabIndex = 2;//选中标题栏拍照按钮
    call_private_fun::videowidgetrecoverTabWidget(*videowidgt);
    DataManager::instance()->m_tabIndex = 3;//选中标题栏录像按钮
    call_private_fun::videowidgetrecoverTabWidget(*videowidgt);
    DataManager::instance()->m_tabIndex = 8;//选中缩略图部分拍照录像复合按钮
    call_private_fun::videowidgetrecoverTabWidget(*videowidgt);
    DataManager::instance()->m_tabIndex = 10;//选中缩略图框按钮
    call_private_fun::videowidgetrecoverTabWidget(*videowidgt);
}


/**
 *  @brief 删除快捷键
 */
TEST_F(VideoWidgetTest, DelShortCut)
{
    videowidget *prevideo = mainwindow->findChild<videowidget *>(VIDEO_PREVIEW_WIDGET);

    //点击鼠标右键
    QTest::qWait(1000);
    QTest::mouseMove(prevideo, QPoint(0, 0), 500);
    for (int i = 0; i < 3; i++) {
        QTest::keyClick(prevideo, Qt::Key_Delete, Qt::NoModifier, 0);
        QTest::qWait(500);
    }
}

/**
 *  @brief majorimageprocessingthread类打桩
 */
TEST_F(VideoWidgetTest, majorimageprocessingthread)
{
    Stub stub;
    videowidget *videowidgt = mainwindow->findChild<videowidget *>(VIDEO_PREVIEW_WIDGET);
    //进入默认分支
    stub.set(v4l2core_start_stream, ADDR(Stub_Function, v4l2core_start_stream));
    stub.set(v4l2core_stop_stream, ADDR(Stub_Function, v4l2core_stop_stream));
    stub.set(v4l2core_get_decoded_frame, ADDR(Stub_Function, v4l2core_get_decoded_frame));
    stub.set(get_resolution_status, ADDR(Stub_Function, get_resolution_status));
    stub.set(v4l2core_clean_buffers, ADDR(Stub_Function, v4l2core_clean_buffers));
    stub.set(v4l2core_update_current_format, ADDR(Stub_Function, v4l2core_update_current_format_Not_OK));
    stub.set(v4l2core_prepare_valid_format, ADDR(Stub_Function, v4l2core_prepare_valid_format));
    stub.set(v4l2core_prepare_valid_resolution, ADDR(Stub_Function, v4l2core_prepare_valid_resolution));
    stub.set(get_v4l2_device_handler, ADDR(Stub_Function, get_v4l2_device_handler));
    stub.set(set_device_name, ADDR(Stub_Function, set_device_name));
    stub.set(v4l2core_release_frame, ADDR(Stub_Function, v4l2core_release_frame));
    videowidgt->m_imgPrcThread->start();
    QTest::qWait(500);
    videowidgt->m_imgPrcThread->stop();

    //录像分支
    //设置暂停时长,进入解码任务调度分支
    stub.reset(v4l2core_update_current_format);
    stub.set(v4l2core_update_current_format, ADDR(Stub_Function, v4l2core_update_current_format_OK));
    access_private_field::MajorImageProcessingThreadm_stopped(*videowidgt->m_imgPrcThread) = 0;
    stub.set(get_capture_pause, ADDR(Stub_Function, get_capture_pause));
    stub.set(video_capture_get_save_video, ADDR(Stub_Function, video_capture_get_save_video));
    stub.set(encoder_buff_scheduler, ADDR(Stub_Function, encoder_buff_scheduler));
    stub.set(get_wayland_status, ADDR(Stub_Function, get_wayland_status));
    videowidgt->m_imgPrcThread->start();
    QTest::qWait(500);
    videowidgt->m_imgPrcThread->stop();
    stub.reset(get_wayland_status);

    //进入H264分支
    access_private_field::MajorImageProcessingThreadm_stopped(*videowidgt->m_imgPrcThread) = 0;
    stub.set(v4l2core_get_requested_frame_format, ADDR(Stub_Function, v4l2core_get_requested_frame_format));
    stub.set(v4l2core_set_h264_frame_rate_config, ADDR(Stub_Function, v4l2core_set_h264_frame_rate_config));
    videowidgt->m_imgPrcThread->start();
    QTest::qWait(500);
    videowidgt->m_imgPrcThread->stop();
    while (videowidgt->m_imgPrcThread->isRunning());

    //还原
    stub.reset(v4l2core_update_current_format);
    stub.reset(v4l2core_start_stream);
    stub.reset(v4l2core_stop_stream);
    stub.reset(v4l2core_get_decoded_frame);
    stub.reset(get_resolution_status);
    stub.reset(v4l2core_clean_buffers);
    stub.reset(v4l2core_prepare_valid_format);
    stub.reset(v4l2core_prepare_valid_resolution);
    stub.reset(get_v4l2_device_handler);
    stub.reset(set_device_name);
    stub.reset(get_capture_pause);
    stub.reset(video_capture_get_save_video);
    stub.reset(encoder_buff_scheduler);
    stub.reset(v4l2core_get_requested_frame_format);
    stub.reset(v4l2core_set_h264_frame_rate_config);

}
