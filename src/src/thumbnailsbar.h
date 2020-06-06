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
class ThumbnailsBar : public DFloatingWidget
{
    Q_OBJECT
public:

    explicit ThumbnailsBar(DWidget *parent = nullptr);
    void load();
    void loadInterface(QString strPath);
    int getItemCount(){return m_nItemCount;}

    QHBoxLayout *m_mainLayout;
    DButtonBoxButton *pushButton_8;
    DButtonBoxButton *pushButton_9;
    DWidget *m_wgt;
    QHBoxLayout *m_hBOx;
    //QListWidget *imageList;
    QMap<QString, QPixmap> m_imagemap;
    int m_nMaxItem;
private:
    int m_nItemCount;
    QString m_strPath;
    mutable QReadWriteLock m_readlock;
    mutable QReadWriteLock m_writelock;
    volatile bool m_bFlag;
    int m_current = 0;
    DBImgInfoList m_infos;
    DIconButton *m_lastButton {nullptr};

private:
    //void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;
signals:
    void fitToolBar();//调整工具栏
public slots:
    void onFileChanged(const QString &strDirectory);
};

#endif // THUMBNAILSBAR_H
