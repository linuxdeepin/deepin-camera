#include "majorimageprocessingthread.h"

MajorImageProcessingThread::MajorImageProcessingThread()
{
    stopped = false;
    majorindex = -1;
}

void MajorImageProcessingThread::stop()
{
    stopped = true;
}

void MajorImageProcessingThread::init()
{
    //condition = new QWaitCondition();
}

void MajorImageProcessingThread::run()
{

    v4l2_dev_t *vd1 = get_v4l2_dev();
    v4l2_frame_buff_t *frame;
    unsigned char *rgb24;
    render_init(RENDER_NONE, 640, 480, 2, 0, 0);

    v4l2core_start_stream(vd1);
    QString *img_filename  = new QString();
    while (!stopped) {
        msleep(1000 / 20);
        frame = v4l2core_get_decoded_frame(vd1);
        if (frame == nullptr) {
            continue;
        }
        render_frame_fx(frame->yuv_frame, my_render_mask);
        int result = -1;
        if (frame != nullptr) {
            result = 0;
        }

        printf("(raw)frame->timestamp:%llu\n", frame->timestamp);

        if (video_capture_get_save_video()) {
            int size = (frame->width * frame->height * 3) / 2;

            uint8_t *input_frame = frame->yuv_frame;
            /*
             * TODO: check codec_id, format and frame flags
             * (we may want to store a compressed format
             */
            if (get_video_codec_ind() == 0) { //raw frame
                switch (v4l2core_get_requested_frame_format(vd1)) {
                case  V4L2_PIX_FMT_H264:
                    input_frame = frame->h264_frame;
                    size = (int) frame->h264_frame_size;
                    break;
                default:
                    input_frame = frame->raw_frame;
                    size = (int) frame->raw_frame_size;
                    break;
                }
            }
            /*add the frame to the encoder buffer*/
            encoder_add_video_frame(input_frame, size, frame->timestamp, frame->isKeyframe);

            //printf("(video)frame->timestamp:%llu\n", frame->timestamp);

            /*
             * exponencial scheduler
             *  with 50% threshold (milisec)
             *  and max value of 250 ms (4 fps)
             */
            double time_sched = encoder_buff_scheduler(ENCODER_SCHED_LIN, 0.5, 250);
            if (time_sched > 0) {
                switch (v4l2core_get_requested_frame_format(vd1)) {
                case  V4L2_PIX_FMT_H264: {
                    uint32_t framerate = lround(time_sched * 1E6); /*nanosec*/
                    v4l2core_set_h264_frame_rate_config(vd1, framerate);
                    break;
                }
                default: {
                    struct timespec req = {
                        .tv_sec = 0,
                        .tv_nsec = (uint32_t) (time_sched * 1E6)
                    };/*nanosec*/
                    nanosleep(&req, NULL);
                    break;
                }
                }
            }
        }
        rgb24 = (unsigned char *)malloc(frame->width * frame->height * 3 * sizeof(char));
        //*img_filename = "/home/shicetu/Desktop/shicetu/image/" + QString::number(n) + ".jpg";
        //img_filename.;
        //v4l2core_save_image(frame, ( *img_filename).toLatin1().data(), IMG_FMT_JPG);
        convert_yuv_to_rgb_buffer((unsigned char *)frame->raw_frame, rgb24, frame->width, frame->height);
        QImage img;
        img = QImage(rgb24, frame->width, frame->height, QImage::Format_RGB888);
        //img.save(img_filename, "JPG");
        emit SendMajorImageProcessing(img, result);
//        jpeg_encoder_ctx_t *jpeg_ctx = (jpeg_encoder_ctx_t *)calloc(1, sizeof(jpeg_encoder_ctx_t));
//        if (jpeg_ctx != nullptr) {
//            uint8_t *jpeg = (uint8_t *)calloc((frame->width * frame->height) >> 1, sizeof(uint8_t));
//            if (jpeg != nullptr)
//            }
        render_frame_osd(frame->yuv_frame);
        render_frame(frame->yuv_frame);
        v4l2core_release_frame(vd1, frame);
        msleep(1000 / 30);
    }
    v4l2core_stop_stream(vd1);
    render_close();
}
//    if (majorindex != -1) {
//        while (!stopped) {
//            msleep(1000 / 30);

//            QImage img;
//            int ret = LPF_GetFrame();
//            if (ret == 0) {
//                int WV = LPF_GetCurResWidth();
//                int HV = LPF_GetCurResHeight();
//                img = QImage(rgb24, WV, HV, QImage::Format_RGB888);
//            }

//            emit SendMajorImageProcessing(img, ret);
//        }
//    }
//}

