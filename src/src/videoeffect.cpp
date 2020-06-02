/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
*
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

#include "videoeffect.h"
#include <QImage>

videoEffect::videoEffect(QObject *parent) : QObject(parent)
{
    setEffName();
    for (int i = 0 ; i < EFFECTS_NUM; ++i) {
        if (m_nameVec.length() > i)
            m_mapEffName.insert( m_nameVec[i], i);
    }
}

void videoEffect::chooseEffect(QImage *img, int eff)
{
    //NO:无效果，XRay:x光,FlowerScope:万花筒,OutStand:突出,KongFu:功夫,Border:加边,Saturat:增加饱和度,Square:广场,Distort:扭曲,
    //TShadow:拖影,Twist:拧转,Shrink:收缩,Radiat:放射,Show:显现,Tan:显现,WaveForm:波形,Rippe:波纹,Lavender:淡紫,
    //Vortex:漩涡,Thermal:热成像,Vertigo:眩晕,Blinding:眩目,Flip:翻转,OldPhoto:老照片,HalfConscious:迷幻,Mirror:镜像,Sieves:筛子,
    //BW:黑白,
    switch (eff) {
    case NO:

        break;
    case Flip:
        ShowRotate180(img, 180); //默认180度
        break;
    case FlowerScope:

        break;
    case OutStand:

        break;
    case KongFu:

        break;
    case Border:

        break;
    case Saturat:
        AdjustSaturation(img, 70); //大概70吧
        break;
    case Square:

        break;
    case Distort:

        break;
    case TShadow:

        break;
    case Twist:

        break;
    case Shrink:

        break;
    case Radiat:

        break;
    case Show:

        break;
    case Tan:

        break;
    case WaveForm:

        break;
    case Rippe:

        break;
    case Lavender:

        break;
    case Vortex:

        break;
    case Thermal:

        break;
    case Vertigo:

        break;
    case Blinding:

        break;
    case XRay:

        break;
    case OldPhoto:

        break;
    case HalfConscious:

        break;
    case Mirror:
        MirrorEff(img, true, false); //当前效果如此
        break;
    case Sieves:

        break;
    case BW:
        greyScale(img);
        break;
    }
}

//翻转
void videoEffect::ShowRotate180(QImage *img, int scale)
{
    if (img->isNull())
        return;
    QMatrix matrix;
    matrix.rotate(scale);
    *img = img->transformed(matrix);
}

//镜像
void videoEffect::MirrorEff(QImage *img, bool isHFlip, bool isVFlip)
{
    if (img->isNull())
        return;
    *img = img->mirrored(isHFlip, isVFlip);
}

//饱和度
void videoEffect::AdjustSaturation(QImage *img, int iSaturateValue)
{
    QColor oldColor;
    QColor newColor;
    int h, s, l;

    for (int x = 0; x < img->width(); x++) {
        for (int y = 0; y < img->height(); y++) {
            oldColor = QColor(img->pixel(x, y));

            newColor = oldColor.toHsl();
            h = newColor.hue();
            s = newColor.saturation() + iSaturateValue;
            l = newColor.lightness();

            //we check if the new value is between 0 and 255
            s = qBound(0, s, 255);;
            newColor.setHsl(h, s, l);
            img->setPixel(x, y, qRgb(newColor.red(), newColor.green(), newColor.blue()));
        }
    }
}

void videoEffect::greyScale(QImage *img)
{
    QColor oldColor;

    for (int x = 0; x < img->width(); x++) {
        for (int y = 0; y < img->height(); y++) {
            oldColor = QColor(img->pixel(x, y));
            int average = (oldColor.red() + oldColor.green() + oldColor.blue()) / 3;
            img->setPixel(x, y, qRgb(average, average, average));
        }
    }
}

void videoEffect::setEffList()
{

}
void videoEffect::setEffName()
{
    //从配置文件中读取
    for (int i = 0 ; i < EFFECTS_NUM; ++i)
        m_nameVec.push_back(NAME[i]);
}

int videoEffect::FindEffIndexByName(const QString name)
{
    if (name.isEmpty()) {
        return 0;
    }
    auto find_index = m_mapEffName.find(name);
    if (find_index != m_mapEffName.end())
        return find_index.value();
    else {
        return 0;
    }
}
