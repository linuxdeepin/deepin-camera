#include "datamanager.h"

DataManager *DataManager::m_dataManager=nullptr;
bool DataManager::getbMultiSlt()
{
    return m_bMultiSlt;
}

void DataManager::setbMultiSlt(bool bMultiSlt)
{
    m_bMultiSlt=bMultiSlt;
}

int DataManager::getindexNow()
{
    return m_indexNow;
}

void DataManager::setindexNow(int indexNow)
{
    m_indexNow=indexNow;
}

//QSet<int> DataManager::getindex()
//{
//    return m_setIndex;
//}

//void DataManager::clearindex()
//{
//    m_setIndex.clear();
//}

//void DataManager::insertindex(int index)
//{
//    m_setIndex.insert(index);
//}

//void DataManager::setindex(int index)
//{
//    m_setIndex.insert(index);
//}

QString &DataManager::getstrFileName()
{
    return m_strFileName;
}

void DataManager::setstrFileName(QString strFileName)
{
    m_strFileName=strFileName;
}

int &DataManager::getvideoCount()
{
    return m_videoCount;
}

void DataManager::setvideoCount(int videoCount)
{
    m_videoCount=videoCount;
}

int DataManager::getdevStatus()
{
    return m_devStatus;
}

void DataManager::setdevStatus(int devStatus)
{
    m_devStatus=devStatus;
}

//QMap<int, ImageItem *> &DataManager::getindexImage()
//{
//    return m_indexImage;
//}

//void DataManager::setindexImage(int tIndex, ImageItem *pLabel)
//{
//    m_indexImage.insert(tIndex,pLabel);
//}

DataManager *DataManager::instance()
{
    if (m_dataManager == nullptr) {
        m_dataManager = new DataManager;
    }
    return m_dataManager;
}

DataManager::DataManager()
{

}

