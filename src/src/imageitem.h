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
#include <QDateTime>
#include <QPropertyAnimation>
//#ifdef private
//#undef private
//#include <dprintpreviewwidget.h>
//#include <dprintpreviewdialog.h>
//#define private public
//#else

#include <dprintpreviewwidget.h>
#include <dprintpreviewdialog.h>
//#endif
DWIDGET_USE_NAMESPACE

//#define THUMBNAIL_WIDTH 50//缩略图图元宽度
//#define THUMBNAIL_HEIGHT 50//缩略图图元高度
#define THUMBNAIL_PIXMAP_SIZE 50//缩略图图像缩放前尺寸,考虑了内存占用和清晰度两方面情况的取值
#define SELECTED_WIDTH 58//选中的缩略图图元宽高

#define ANIMATION_DURATION 160 //动画时长

/**
 * @brief AnimationWidget 文件更新动画的上层控件
 * 用于实现图像预览区域文件更新的动画效果
 */
class AnimationWidget : public DLabel
{
    Q_OBJECT
public:
    AnimationWidget(QPixmap pixmap, QWidget * parent = nullptr) : m_animatePix(pixmap)
    {
        this->setParent(parent);
        setMargin(0);
        setContentsMargins(0, 0, 0, 0);
        resize(THUMBNAIL_PIXMAP_SIZE, THUMBNAIL_PIXMAP_SIZE);
    }
    ~AnimationWidget()
    {
    }

    /**
     * @brief setPixmap 更新缩略图
     * @param pixmap
     */
    void setPixmap(QPixmap pixmap)
    {
        m_animatePix = pixmap;
    }
    /**
     * @brief getPixmap 获取缩略图
     * @return 缩略图
     */
    QPixmap getPixmap()
    {
        return m_animatePix;
    }

protected:
    /**
     * @brief paintEvent 控件绘制事件
     */
    void paintEvent(QPaintEvent *e) override
    {
        Q_UNUSED(e);
        QRect pixmapRect = rect();
        QPainter painter(this);
        QPainterPath path;

        painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

        path.addRoundedRect(pixmapRect, width(), height());
        painter.fillPath(path, QBrush(m_animatePix));
    }

private:
    QPixmap m_animatePix;      //缩略图
};

/**
 * @brief ImageItem 处理图片视频相关的内容
 * 图片路径获取与设置，视频时长，图片单选多选，打印，删除，鼠标事件
 * 若为图片有打印功能，视频没有
 */
class ImageItem : public DLabel
{
    Q_OBJECT
public:
    ImageItem(QWidget *parent = nullptr);

    ~ImageItem() override;

    /**
     * @brief updatePicPath 更新文件路径
     * @param filePath 文件路径
     */
    void updatePicPath(const QString &filePath);

    /**
     * @brief updatePic 更新图片
     * @param pixmap
     */
    void updatePic(QPixmap pixmap)
    {
        m_pAniWidget->setVisible(true);
        m_pAniWidget->setPixmap(pixmap);


        m_pAnimation = new QPropertyAnimation(m_pAniWidget, "geometry", this);
        m_pAnimation->setEasingCurve(QEasingCurve::Linear);
        m_pAnimation->setDuration(ANIMATION_DURATION);
//        m_pAnimation->setStartValue(QRect(width() / 2, height() / 2, 0, 0));
//        m_pAnimation->setEndValue(rect());
//        qInfo() << rect() <<endl;
        m_pAnimation->setKeyValueAt(0, QRect(width() / 2, height() / 2, 0, 0));
        m_pAnimation->setKeyValueAt(0.3, QRect((width() - 40) / 2, (height() - 40) / 2, 40, 40));
        m_pAnimation->setKeyValueAt(1, QRect(1, 1, width() - 1, height() - 1)); //去除1像素外边框
        m_pAnimation->start(/*QAbstractAnimation::DeleteWhenStopped*/);
        connect(m_pAnimation, &QPropertyAnimation::finished, [ & ]() {
            m_pAnimation->deleteLater();
            m_pAnimation = nullptr;
            m_pixmap = m_pAniWidget->getPixmap();
            update();
            m_pAniWidget->setVisible(false);
        });
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
    void SetPath(QString &path)
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
     * @brief initShortcut 初始化快捷键
     */
    void initShortcut();

    /**
     * @brief openFile 打开文件
     */
    void openFile();

signals:
    /**
     * @brief trashFile 删除文件信号
     * @param filePath  删除文件路径
     */
    void trashFile(const QString &filePath);

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

    /**
     * @brief showMenu 显示菜单
     */
    void showMenu();

    /**
     * @brief onOpenFolder 打开文件夹
     */
    void onOpenFolder();

    /**
    * @brief onCopy　右键菜单复制
    */
    void onCopy();

    /**
    * @brief onShortcutDel　delete按下
    */
    void onShortcutDel();

protected:
    /**
     * @brief paintEvent 控件绘制事件
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief mouseMoveEvent 用于取消鼠标拖拽，避免拖拽时窗口移动
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief mouseReleaseEvent 鼠标释放事件
     * @param event
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * @brief focusInEvent 焦点事件
     * @param event
     */
    void focusInEvent(QFocusEvent *event) override;

    /**
     * @brief focusOutEvent 失去焦点事件
     * @param event
     */
    void focusOutEvent(QFocusEvent *event) override;

    /**
     * @brief showPrintDialog 显示打印对话框
     */
    void showPrintDialog(const QStringList &paths, QWidget *parent);

private slots:
#if (DTK_VERSION_MAJOR > 5 \
    || (DTK_VERSION_MAJOR >=5 && DTK_VERSION_MINOR > 4) \
    || (DTK_VERSION_MAJOR >= 5 && DTK_VERSION_MINOR >= 4 && DTK_VERSION_PATCH >= 10))//5.4.7暂时没有合入
    /**
     * @brief paintRequestedAsyn 同步打印
     * @param _printer 打印机
     * @param pageRange 页数
     */
    void paintRequestedAsyn(DPrinter *_printer, const QVector<int> &pageRange);
#endif
    /**
     * @brief paintRequestSync 异步打印
     * @param _printer 打印机
     */
    void paintRequestSync(DPrinter *_printer);

private:
    bool                m_bVideo;//是否视频
    bool                m_bMousePress;//鼠标按下事件
    int                 m_index;//索引
    int64_t             m_nDuration = 0; //视频文件时长,int形式时间
    QList<QImage>       m_imgs;//需要打印的图片
    QString             m_path;//文件路径
    QPixmap             m_pixmap;//缩略图
    QString             m_pixmapstring;//缩略图路径
    QString             m_strDuratuion;//视频文件时长,形式为00：00：00
    QMenu               *m_menu;//右键菜单
    QAction             *m_actCopy;//复制
    QAction             *m_actDel;//删除
    QAction             *m_actOpenFolder;//打开文件夹
    QAction             *m_actPrint;//打印
    QDateTime           m_lastDelTime;//最后一次删除文件时间，避免过快删除导致显示空白
    AnimationWidget     *m_pAniWidget;//动画控件
    QPropertyAnimation  *m_pAnimation;

    bool                m_bFocus;
};

#endif // IMAGEITEM_H
