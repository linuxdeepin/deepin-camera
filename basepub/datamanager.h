#ifndef DATAMANAGER_H
#define DATAMANAGER_H

//#include "src/src/imageitem.h"

#include <QObject>
#include <QSet>

class DataManager: public QObject
{
public:
    QSet<int> m_setIndex;
    Q_DISABLE_COPY(DataManager)
    /**
     * @brief getbMultiSlt 是否多选
     */
    bool getbMultiSlt();
    /**
     * @brief setbMultiSlt 设置多选
     * @param bMultiSlt
     */
    void setbMultiSlt(bool bMultiSlt);
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
    int getdevStatus();
    /**
     * @brief setdevStatus 设置设备状态
     */
    void setdevStatus(int devStatus);
//    /**
//     * @brief getindexImage 获得图片索引
//     */
//    QMap<int, ImageItem *> &getindexImage();
//    /**
//     * @brief setdevStatus 设置图片索引
//     */
//    void setindexImage(int tIndex,ImageItem * pLabel);

    static DataManager *instance();

private:
    DataManager();
    static DataManager *m_dataManager;
    bool m_bMultiSlt=false;
    int m_indexNow=0;
//    QMap<int, ImageItem *> m_indexImage;
    QString m_strFileName;
    int m_videoCount;
    volatile int m_devStatus;
};
#endif // DATAMANAGER_H
