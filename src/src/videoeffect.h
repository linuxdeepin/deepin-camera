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


#ifndef VIDEOEFFECT_H
#define VIDEOEFFECT_H

#include <QObject>
#include <QMap>
#include <QVector>

static int EFFECTS_NUM = 28;
//NO:无效果，XRay:x光,FlowerScope:万花筒,OutStand:突出,KongFu:功夫,Border:加边,Saturat:增加饱和度,Square:广场,Distort:扭曲,
//TShadow:拖影,Twist:拧转,Shrink:收缩,Radiat:放射,Show:显现,Tan:棕褐色,WaveForm:波形,Rippe:波纹,Lavender:淡紫,
//Vortex:漩涡,Thermal:热成像,Vertigo:眩晕,Blinding:眩目,Flip:翻转,OldPhoto:老照片,HalfConscious:迷幻,Mirror:镜像,Sieves:筛子,
//BW:黑白,

static QVector<QString> NAME = {"无效果", "翻转", "饱和度", "镜像", "黑白", "加边", "万花筒", "广场", "扭曲",
                                "拖影", "拧转", "收缩", "放射", "显现", "棕褐色", "波形", "波纹", "淡紫",
                                "漩涡", "热成像", "眩晕", "眩目", "x光", "老照片", "迷幻", "突出", "筛子",
                                "功夫"
                               };
enum EFFECTS {NO = 0, Flip, Saturat, Mirror, BW, Border, FlowerScope, Square, Distort,
              TShadow, Twist, Shrink, Radiat, Show, Tan, WaveForm, Rippe, Lavender,
              Vortex, Thermal, Vertigo, Blinding, XRay, OldPhoto, HalfConscious, OutStand, Sieves,
              KongFu
             };

class videoEffect : public QObject
{
    Q_OBJECT
public:
    explicit videoEffect(QObject *parent = nullptr);

    void chooseEffect(QImage *img, int index);
    void AdjustSaturation(QImage *img, int iSaturateValue = 30); //饱和度
    void ShowRotate180(QImage *img, int scale); //翻转
    void MirrorEff(QImage *img, bool isHFlip, bool isVFlip);//镜像
    void greyScale(QImage *img);//黑白
    int FindEffIndexByName(const QString name);
private:
    QMap<QString, int> m_mapEffName;
    QVector<QString> m_nameVec;
    void setEffList();
    void setEffName();
signals:

public slots:
};

#endif // VIDEOEFFECT_H
