/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
*             zhangcheng <zhangchenga@uniontech.com>
* Maintainer: shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
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

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QSet>

enum DeviceStatus {NOCAM, CAM_CANNOT_USE, CAM_CANUSE};
enum MultiType {None, Ctrl, Shift, Both};

class DataManager: public QObject
{
public:

    static DataManager *instance();

    QSet<int> m_setIndex;
    Q_DISABLE_COPY(DataManager)
    
    /**
     * @brief getMultiType 返回ctrl多选或者shift多选
     */
    MultiType getMultiType();
    /**
     * @brief setCtrlMulti 设置ctrl多选
     * @param bCtrlMulti
     */
    void setCtrlMulti(bool bCtrlMulti);
    /**
     * @brief setShiftMulti 设置shift连续多选
     * @param bShiftMultiSlt
     */
    void setShiftMulti(bool bShiftMultiSlt);
    /**
     * @brief getbindexNow 获得当前索引
     */
    int getindexNow();
    /**
     * @brief setindexNow 设置当前索引
     * @param indexNow
     */
    void setindexNow(int indexNow);
    /**
     * @brief setLastIndex 设置上一次图片索引
     * @param index
     */
    void setLastIndex(int index)
    {
        m_indexLast = index;
    }

    /**
     * @brief getLastIndex 获取图片索引
     */
    int getLastIndex()
    {
        return m_indexLast;
    }
    /**
     * @brief getbindexNow 获得缩略图索引
     */
//    QSet<int> getindex();
//    /**
//     * @brief clearindex 清除缩略图索引
//     */
//    void clearindex();
//    /**
//     * @brief insertindex 插入缩略图索引
//     */
//    void insertindex(int index);
//    /**
//     * @brief getbindexNow 设置缩略图索引
//     */
//    void setindex(int index);
    /**
     * @brief getstrFileName 获得文件名
     */
    QString &getstrFileName();
    /**
     * @brief getstrFileName 设置文件名
     * @param strFileName
     */
    void setstrFileName(QString strFileName);
    /**
     * @brief getvideoCount 获得视频数目
     */
    int &getvideoCount();
    /**
     * @brief setvideoCount 设置视频数目
     */
    void setvideoCount(int videoCount);
    /**
     * @brief getdevStatus 获得设备状态
     */
    enum DeviceStatus getdevStatus();
    /**
     * @brief setdevStatus 设置设备状态
     */
    void setdevStatus(enum DeviceStatus devStatus);
//    /**
//     * @brief getindexImage 获得图片索引
//     */
//    QMap<int, ImageItem *> &getindexImage();
//    /**
//     * @brief setdevStatus 设置图片索引
//     */
//    void setindexImage(int tIndex,ImageItem * pLabel);



private:
    DataManager();
    static DataManager *m_dataManager;
    bool m_bCtrlMulti=false;//ctrl键多选
    bool m_bShitfMulti=false;//shift键连续多选
    int m_indexLast=-1;
    int m_indexNow=0;
//    QMap<int, ImageItem *> m_indexImage;
    QString m_strFileName;
    int m_videoCount;
    volatile enum DeviceStatus m_devStatus;
};
#endif // DATAMANAGER_H
