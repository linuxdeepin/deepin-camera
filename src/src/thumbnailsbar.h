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

#ifndef THUMBNAILSBAR_H
#define THUMBNAILSBAR_H

#include <DWidget>
#include <QDateTime>
#include <QObject>
#include <QDebug>
#include <QDirIterator>
#include <QReadWriteLock>
#include <QListWidget>
#include <QHBoxLayout>
#include <QFileSystemWatcher>
#include <QPushButton>
#include <DButtonBox>
#include <DFloatingWidget>
#include <DIconButton>

DWIDGET_USE_NAMESPACE
#define IMAGE_HEIGHT_DEFAULT    100
#define THUMBNAIL_WIDTH         80
#define THUMBNAIL_HEIGHT        100
//缩略图
//hjb
struct DBImgInfo {
    QString filePath;
    QString fileName;
    QString dirHash;
    QDateTime time;

    bool operator==(const DBImgInfo &other)
    {
        return (filePath == other.filePath &&
                fileName == other.fileName &&
                time == other.time);
    }

    friend QDebug operator<<(QDebug &dbg, const DBImgInfo &info)
    {
        dbg << "(DBImgInfo)["
            << "Path:" << info.filePath
            << "Name:" << info.fileName
            << "Dir:" << info.dirHash
            << "Time:" << info.time
            << "]";
        return dbg;
    }
};
typedef QList<DBImgInfo> DBImgInfoList;

enum ActType {ActTakePic, ActTakeVideo}; // 定义枚举类型ActType
enum CamStatus {STATNULL, STATPicIng, STATVdIng}; // 定义枚举类型CamStatus

class ThumbnailsBar : public DFloatingWidget
{
    Q_OBJECT
public:

    explicit ThumbnailsBar(DWidget *parent = nullptr);
    void load();
    void loadInterface(QString strPath);
    int getItemCount(){return m_nItemCount;}
    void ChangeActType(int nType);
    void addPath(QString strPath);

    QHBoxLayout *m_mainLayout;
    DWidget *m_wgt;
    QHBoxLayout *m_hBOx;
    //QListWidget *imageList;
    QMap<QString, QPixmap> m_imagemap;
    int m_nMaxItem;
    int m_nStatus; //当前状态
private:
    int m_nItemCount;
    mutable QReadWriteLock m_readlock;
    mutable QReadWriteLock m_writelock;
    volatile bool m_bFlag;
    int m_current = 0;

    int m_nActTpye;//拍照或者视频模式，默认拍照

    QStringList m_strlstFolders;

    DBImgInfoList m_infos;
    DPushButton *m_lastButton {nullptr};

private:
    //void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;
signals:
    void fitToolBar();//调整工具栏
    void enableTitleBar(int nType);//1、禁用标题栏视频；2、禁用标题栏拍照；3、恢复标题栏视频；4、恢复标题栏拍照
    void takePic();
    void takeVd();
    void enableSettings(bool bTrue);
public slots:
    void onFoldersChanged(const QString &strDirectory);
    void onBtnClick();
};

#endif // THUMBNAILSBAR_H
