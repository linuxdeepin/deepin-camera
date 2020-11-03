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
    /**
     * @brief setPic 取消image带来的警告
     * @param image
     */
    void setPic(QImage image)
    {
        Q_UNUSED(image);
    }
    /**
     * @brief updatePic 更新图片
     * @param pixmap
     */
    void updatePic(QPixmap pixmap)
    {
        m_pixmap = pixmap;
        update();
    }
    /**
     * @brief setIndex 设置图片索引
     * @param index
     */
    void setIndex(int index)
    {
        m_index = index;
    }
    /**
     * @brief SetPath 设置存放路径
     * @param path
     */
    void SetPath(QString path)
    {
        m_path = path;
    }
    /**
     * @brief getPath 返回路径
     */
    inline QString getPath()
    {
        return m_path;
    }
    /**
     * @brief getIndex 返回图片索引
     */
    inline int getIndex()
    {
        return m_index;
    }
    QString getDuration()
    {
        return m_strDuratuion;
    }
    /**
     * @brief parseFromFile 解析文件
     * @param fi
     */
    bool parseFromFile(const QFileInfo &fi);
    /**
     * @brief getIsVideo 判断是否为视频
     */
    bool getIsVideo()
    {
        return m_bVideo;
    }
signals:
    /**
     * @brief imageItemclicked 图片点击事件
     */
    void imageItemclicked(int index, int indexNow);
    /**
     * @brief trashFile 右键删除
     */
    void trashFile();//右键删除
    void showDuration(QString strDuration);
protected:
    /**
     * @brief mouseDoubleClickEvent 鼠标双击事件
     */
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    /**
     * @brief mouseReleaseEvent 鼠标弹起事件
     */
    void mouseReleaseEvent(QMouseEvent *ev) override;
    /**
     * @brief mousePressEvent 鼠标按下事件
     */
    void mousePressEvent(QMouseEvent *ev) override;
    /**
     * @brief paintEvent 鼠标弹起事件
     */
    void paintEvent(QPaintEvent *event) override;
    /**
     * @brief mouseMoveEvent 鼠标移动事件
     */
    void mouseMoveEvent(QMouseEvent *event) override;//用于取消鼠标拖拽，避免拖拽时窗口移动

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
    QString m_strDuratuion;
    bool m_bVideo;//是否视频
};

#endif // IMAGEITEM_H
