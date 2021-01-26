/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*             hujianbo <hujianbo@uniontech.com>
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

#ifndef IMAGEITEM_H
#define IMAGEITEM_H

#include <DLabel>

#include <QMenu>
#include <QAction>
#include <QFileInfo>

DWIDGET_USE_NAMESPACE

#define THUMBNAIL_WIDTH 30//缩略图图元宽度
#define THUMBNAIL_HEIGHT 40//缩略图图元高度
#define THUMBNAIL_PIXMAP_SIZE 200//缩略图图像缩放前尺寸,考虑了内存占用和清晰度两方面情况的取值
#define SELECTED_WIDTH 58//选中的缩略图图元宽高

/**
 * @brief ImageItem 处理图片视频相关的内容
 * 图片路径获取与设置，视频时长，图片单选多选，打印，删除，鼠标事件
 * 若为图片有打印功能，视频没有
 */
class ImageItem : public DLabel
{
    Q_OBJECT
public:
    ImageItem(int index = 0, QString path = nullptr, QWidget *parent = nullptr);

    ~ImageItem() override;

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
     * @brief SetPath 设置路径
     * @param path
     */
    void SetPath(QString path)
    {
        m_path = path;
    }

    /**
     * @brief getPath 获取路径
     */
    inline QString getPath()
    {
        return m_path;
    }

    /**
     * @brief getIndex 获取图片索引
     */
    inline int getIndex()
    {
        return m_index;
    }

    /**
     * @brief getDuration 获取录像时长
     */
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

    /**
     * @brief showMenu 显示菜单
     */
    void showMenu();
signals:

    /**
     * @brief trashFile 删除文件信号
     */
    void trashFile();

    /**
     * @brief showDuration 录像时长信号
     */
    void showDuration(QString strDuration);

    /**
     * @brief needFit 图片适应界面大小信号
     */
    void needFit();

    /**
     * @brief ShiftMulti 多选
     */
    void shiftMulti();

public slots:
    /**
     * @brief onPrint 打印信号
     */
    void onPrint();

protected:
    /**
     * @brief mouseDoubleClickEvent 鼠标双击事件
     */
    void mouseDoubleClickEvent(QMouseEvent *ev) override;

    /**
     * @brief mousePressEvent 鼠标按下事件
     */
    void mousePressEvent(QMouseEvent *ev) override;

    /**
     * @brief paintEvent 鼠标弹起事件
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief mouseMoveEvent 用于取消鼠标拖拽，避免拖拽时窗口移动
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief showPrintDialog 显示打印对话框
     */
    void showPrintDialog(const QStringList &paths, QWidget *parent);

private:
    bool     m_bVideo;//是否视频
    int      m_index;//索引
    int64_t  m_nDuration = 0; //视频文件时长,int形式时间

    QString  m_path;//文件路径
    QPixmap  m_pixmap;//缩略图
    QString  m_pixmapstring;//缩略图路径
    QString  m_strDuratuion;//视频文件时长,形式为00：00：00
    QMenu    *m_menu;//菜单

    QAction  *m_actCopy;//复制
    QAction  *m_actDel;//删除
    QAction  *m_actOpenFolder;//打开文件夹
    QAction  *m_actPrint;//打印

};

#endif // IMAGEITEM_H
