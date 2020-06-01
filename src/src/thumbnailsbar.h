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

DWIDGET_USE_NAMESPACE
#define IMAGE_HEIGHT_DEFAULT    100
#define THUMBNAIL_WIDTH         130
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
class ThumbnailsBar : public DWidget
{
    Q_OBJECT
public:

    explicit ThumbnailsBar(DWidget *parent = nullptr);
    void load();
    void loadInterface(QString strPath);

    QHBoxLayout *horizontalLayout;
    DButtonBoxButton *pushButton_8;
    DButtonBoxButton *pushButton_9;
    DWidget *m_wgt;
    QHBoxLayout *m_hBOx;
    //QListWidget *imageList;
    QMap<QString, QPixmap> m_imagemap;

private:
    QString m_strPath;
    mutable QReadWriteLock m_readlock;
    mutable QReadWriteLock m_writelock;
    volatile bool m_bFlag;
    int m_current = 0;
    DBImgInfoList m_infos;
    void resizeEvent(QResizeEvent *size) Q_DECL_OVERRIDE;
signals:

public slots:
    void onFileChanged(const QString &strDirectory);
};

#endif // THUMBNAILSBAR_H
