/*
 * (c) 2020, Uniontech Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#ifndef VIDEOPREVIEWWIDGET_H
#define VIDEOPREVIEWWIDGET_H

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

class videopreviewwidget : public DWidget
{
    Q_OBJECT
public:
    explicit videopreviewwidget(DWidget *parent = nullptr);
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
