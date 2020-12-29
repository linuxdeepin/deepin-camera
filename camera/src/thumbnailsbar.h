/*
* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
* Maintainer: hujianbo <hujianbo@uniontech.com>
*             shicetu <shicetu@uniontech.com>
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

#include "imageitem.h"

#include <DWidget>
#include <DButtonBox>
#include <DFloatingWidget>
#include <DIconButton>
#include <DLabel>
#include <DSpinner>

#include <QTimer>
#include <QDateTime>
#include <QObject>
#include <QDebug>
#include <QDirIterator>
#include <QReadWriteLock>
#include <QListWidget>
#include <QHBoxLayout>
#include <QFileSystemWatcher>
#include <QPushButton>

DWIDGET_USE_NAMESPACE

#define LAST_BUTTON_WIDTH 64//拍照/录制按钮宽度
#define LAST_BUTTON_HEIGHT 50//拍照/录制按钮高度
#define LAST_BUTTON_SPACE 14//按钮与窗体的间隔
#define ITEM_SPACE 2//控件间距
#define VIDEO_TIME_WIDTH 72//视频时长显示的宽度

/**
 * @brief ActType 定义枚举类型ActType，即拍照和录像
 */
enum ActType {ActTakePic, ActTakeVideo};

/**
 * @brief CamStatus 定义枚举类型CamStatus，即空闲，拍照，录像状态
 */
enum CamStatus {STATNULL, STATPicIng, STATVdIng}; //定义枚举类型CamStatus

QMap<int, ImageItem *> get_imageitem();

/**
 * @brief ThumbnailsBar 缩略图
 */
class ThumbnailsBar : public DFloatingWidget
{
    Q_OBJECT
public:

    explicit ThumbnailsBar(DWidget *parent = nullptr);
    /**
     * @brief initShortcut 初始化快捷键
     */
    void initShortcut();

    /**
     * @brief setBtntooltip 设置按钮部件
     */
    void setBtntooltip();

    /**
    * @brief ChangeActType 改变拍照或录像状态
    * @param nType 拍照或录像
    */
    void ChangeActType(enum ActType nType);

    /**
     * @brief addPath 添加路径
     */
    void addPath(QString strPath);

    /**
     * @brief addPaths 添加多个路径,无顺序要求,但文件夹需要存在
     */
    void addPaths(QString strPicPath,QString strVdPath);

    /**
     * @brief addFile 添加文件
     */
    void addFile(QString strFile);

    /**
     * @brief delFile 删除文件
     */
    void delFile(QString strFile);

    /**
     * @brief widthChanged 改变缩略图宽度
     */
    void widthChanged();

private:

    /**
    * @brief mousePressEvent 缩略图按下事件
    * @param ev 事件
    */
    void mousePressEvent(QMouseEvent *ev) Q_DECL_OVERRIDE;

    /**
    * @brief mouseMoveEvent 用于取消鼠标拖拽，避免拖拽时窗口移动
    * @param event 事件
    */
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    /**
    * @brief fitToolBar　调整工具栏
    */
    void fitToolBar();

    /**
    * @brief enableTitleBar　1、禁用标题栏视频；2、禁用标题栏拍照；3、恢复标题栏视频；4、恢复标题栏拍照
    */
    void enableTitleBar(int nType);

    /**
    * @brief takePic　拍照信号
    */
    void takePic(bool bTrue);

    /**
    * @brief takeVd　录像信号
    */
    void takeVd();

    /**
    * @brief enableSettings　开启设置功能
    */
    void enableSettings(bool bTrue);

public slots:
    /**
    * @brief onFoldersChanged　文件夹改变
    * @param strDirectory　直接路径
    */
    void onFoldersChanged(const QString &strDirectory);

    /**
    * @brief onBtnClick　拍照或录像按钮按下
    */
    void onBtnClick();

    /**
    * @brief onShortcutCopy　ctrl+C按下
    */
    void onShortcutCopy();

    /**
    * @brief onShortcutDel　delete按下
    */
    void onShortcutDel();

    /**
    * @brief onTrashFile　删除
    */
    void onTrashFile();

    /**
    * @brief onShowVdTime　录像时间
    */
    void onShowVdTime(QString str);

    /**
    * @brief onFileName　设置文件名
    * @param strfilename　文件名
    */
    void onFileName(QString strfilename);

    /**
    * @brief onCallMenu　调菜单
    */
    void onCallMenu();

    /**
    * @brief onOpenFolder　打开文件夹
    */
    void onOpenFolder();

    /**
    * @brief onFileName　打印
    */
    void OnPrint();

public:
    enum CamStatus m_nStatus;
    int            m_nMaxWidth;

    DLabel         *m_showVdTime;//缩略图显示视频时长

    QHBoxLayout    *m_mainLayout;
    QHBoxLayout    *m_hBox;

private:
    enum ActType   m_nActTpye;
    int            m_nDelTimes;//删除次数，用于屏蔽文件监控获取到的更新，提升ui性能

    DPushButton    *m_lastButton;//缩略图最后的按钮，可实现拍照/取消拍照/录制。

    QStringList    m_strlstFolders;//存放文件夹路径
    QDateTime      m_lastDelTime;//最后一次删除文件时间，避免过快删除导致显示空白
    QFileInfoList  m_fileInfoLst;//所有文件信息，界面每加载一个，对应删除一个
    QFileInfoList  m_lstPicFolder;//图片文件夹文件信息列表
    QFileInfoList  m_lstVdFolder;//视频文件夹文件信息列表
    QString        m_strFileName;//当前拍照或录制视频的文件名

};

#endif // THUMBNAILSBAR_H
