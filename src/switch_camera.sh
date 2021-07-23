#!/bin/bash

#video0是前置（默认是前置）
#video1是后置


switch_camera()
{
    echo $1
    /etc/init.d/rkisp_3A.sh stop
    if [ "$1" = "/dev/video0" ]
    then
        echo "video011111"
        media-ctl -d /dev/media0 -l '"m01_f_gc2375h 1-0037":0->"rockchip-mipi-dphy-rx":0[1]'
        media-ctl -d /dev/media0 -l '"m00_b_imx258 1-0010":0->"rockchip-mipi-dphy-rx":0[0]'
        media-ctl --set-v4l2 '"m01_f_gc2375h 1-0037":0[fmt:SGBRG10/1600x1200]'

        media-ctl --set-v4l2 '"rkisp1-isp-subdev":0[fmt:SGBRG10/1600x1200]'
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":0[fmt:SGBRG10/1600x1200]' --set-v4l2 '"rkisp1-isp-subdev":0[crop:(0,0)/1600x1200]'
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":2[fmt:YUYV8_2X8/1600x1200]'
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":2[fmt:YUYV8_2X8/1600x1200]' --set-v4l2 '"rkisp1-isp-subdev":2[crop:(0,0)/1600x1200]' 
        echo "****camare switch /dev/video0 success****"
    elif [ "$1" = "/dev/video1" ];then
        media-ctl -d /dev/media0 -l '"m01_f_gc2375h 1-0037":0->"rockchip-mipi-dphy-rx":0[0]'
        media-ctl -d /dev/media0 -l '"m00_b_imx258 1-0010":0->"rockchip-mipi-dphy-rx":0[1]'
        media-ctl --set-v4l2 '"m00_b_imx258 1-0010":0[fmt:SRGGB10/2096x1560]'
       
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":0[fmt:SRGGB10/2096x1560]'
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":0[fmt:SRGGB10/2096x1560]' --set-v4l2 '"rkisp1-isp-subdev":0[crop:(0,0)/2096x1560]'
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":2[fmt:YUYV8_2X8/2096x1560]'
        media-ctl --set-v4l2 '"rkisp1-isp-subdev":2[fmt:YUYV8_2X8/2096x1560]' --set-v4l2 '"rkisp1-isp-subdev":2[crop:(0,0)/2096x1560]'
        echo "****camare switch /dev/video1 success****"
    else
        echo "Camare Switch Input parameter error!"
    fi
    sleep 1
    /etc/init.d/rkisp_3A.sh start  
}


mkfifo /tmp/pipe_camera
chmod 666 /tmp/pipe_camera

while true
do
   cmd=$(cat /tmp/pipe_camera)
   echo ${cmd}
   result=$(echo ${cmd}  | grep 'video')
   echo ${result}
   if [ "$result" != "" ]
   then
       switch_camera ${cmd}
   else
      echo "invalid params"
      sleep 1
   fi
done

