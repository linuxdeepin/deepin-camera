#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <DWidget>
#include <QDateTime>
#include "LPF_V4L2.h"
#include "majorimageprocessingthread.h"
#include "videoeffect.h"
#include <DLabel>

DWIDGET_USE_NAMESPACE

class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsTextItem;
class QGridLayout;
class MyScene;
class QVBoxLayout;
class QHBoxLayout;
class QSpacerItem;


//预览代理
//xxj
enum PRIVIEW_STATE {NORMALVIDEO, NODEVICE, EFFECT, AUDIO, SHOOT};
static PRIVIEW_STATE VIDEO_STATE = NORMALVIDEO;
static int EFFECT_INDEX = NO;
static QImage *CURRENT_IMAGE = new QImage;

class videowidget : public DWidget
{
    Q_OBJECT
public:
    explicit videowidget(DWidget *parent = nullptr);
    void showEvent(QShowEvent *event);

    QImage getCurrentImg();
private:
    bool isFindedDevice = false;
    videoEffect *eff;
    DLabel labTimer;

    QGraphicsView *m_pNormalView;
    QGraphicsScene *m_pNormalScene;
    QGraphicsPixmapItem *m_pNormalItem;
    QGraphicsTextItem *m_pCountItem;
    QGraphicsTextItem *m_pTimeItem;

    QVector<QGraphicsTextItem *> m_VEffectName;
    QVector<QGraphicsPixmapItem *> m_VPixmapItem;
    QVector<QGraphicsView *> m_VEffectPreview;
    QVector<MyScene *> m_VEffectScene;
    QGridLayout *m_pGridLayout;

    int countDown;
    QTimer *countTimer;
    QTimer *flashTimer;
    QDateTime begin_time ;

    PRIVIEW_STATE STATE = NORMALVIDEO;
    int EFFECT_PAGE = 0;

    int m_curTakePicTime;//当前连拍次数
    int m_countdownLen = 1;



    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;
    void resizePixMap();

    void transformImage(QImage *img);
    void resizeImage(QImage *img);
    void setFont(QGraphicsTextItem *item, int size, QString str); //自体

    void showCountDownLabel(PRIVIEW_STATE state);
    void hideCountDownLabel();
    void hideTimeLabel();
    void forbidScrollBar(QGraphicsView *view);

    void newPreViewByState(PRIVIEW_STATE state);//新建view
    void sceneAddItem();
    void updateEffectName();
    void newEffectPreview();//创建特效view
    void delEffectPreview();//释放view
    void showPreviewByState(PRIVIEW_STATE state);//展示view

    int err11, err19;

    MajorImageProcessingThread *imageprocessthread;
    bool m_bTakePic;
    void init();
    void showCountDownLabel();

private:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *verticalLayout_6;

    QHBoxLayout *horizontalLayout;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_7;

    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_4;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_8;
    QSpacerItem *horizontalSpacer_9;

    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer_4;
    QSpacerItem *verticalSpacer_5;
    QSpacerItem *verticalSpacer_6;
    QSpacerItem *verticalSpacer_7;
    QSpacerItem *verticalSpacer_8;
    QSpacerItem *verticalSpacer_9;

    QFrame *p_verline;

    DWidget *centralwidget;

    DPushButton *p_takecambtn; //拍照

    DPushButton *p_takevideobtn; //录像

    DPushButton *p_selectbtn; //切换

    QLabel *p_timelabl;

    QFrame *p_horline;

    QScrollArea *p_scrlarea;//缩略图

    QWidget *scrollAreaWidgetContents;

    void retranslateUi();
signals:
    void finishTakedCamera();//结束拍照或三连拍
    void finishEffectChoose();//结束特效选择
    void sigFlash();
    void disableButtons();
    void ableButtons();
public slots:
    void onShowCountdown();
    void onShowThreeCountdown();
    void onTShowTime();
    void onCancelThreeShots();
    void onTakeVideoOver();
    void onChooseEffect();
    void onMoreEffectLeft();
    void onMoreEffectRight();
    void onBtnPhoto();
    void onBtnThreeShots();
    void onBtnVideo();

    void changePicture(PRIVIEW_STATE state, QImage *img, int effectIndex);
    void showCountdown();
    void effectChoose(QString name);

private slots:
    void ReceiveMajorImage(QImage image, int result);
    void flash();
};

#endif // VIDEOWIDGET_H
