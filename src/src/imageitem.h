/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*
* Maintainer: hujianbo <hujianbo@uniontech.com>
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

#ifndef IMAGEITEM_H
#define IMAGEITEM_H
#include <DLabel>
#include <QMenu>
#include <QAction>
#include <QFileInfo>
DWIDGET_USE_NAMESPACE
#define THUMBNAIL_WIDTH 30
#define THUMBNAIL_HEIGHT 40

class ImageItem : public DLabel
{
    Q_OBJECT
public:
    ImageItem(int index = 0, QString path = nullptr, QWidget *parent = nullptr);
    ~ImageItem() override;
    void setPic(QImage image)
    {
        Q_UNUSED(image);
    }
    void updatePic(QPixmap pixmap)
    {
        m_pixmap = pixmap;
        update();
    }
    void setIndex(int index)
    {
        m_index = index;
    }
    void SetPath(QString path)
    {
        m_path = path;
    }
    inline QString getPath()
    {
        return m_path;
    }
    inline int getIndex()
    {
        return m_index;
    }
    bool parseFromFile(const QFileInfo &fi);
signals:
    void imageItemclicked(int index, int indexNow);

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *event) override;

private:
    int m_index;
    QString m_path;
    QPixmap m_pixmap;
    QString m_pixmapstring;
    bool bFirstUpdate = true;
    QMenu *m_menu;
    QAction *m_actCopy;
    QAction *m_actDel;
    QAction *m_actOpenFolder;

    int64_t m_nDuration = 0; //视频文件时长
};

#endif // IMAGEITEM_H
