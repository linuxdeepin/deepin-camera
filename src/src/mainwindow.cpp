/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
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

#include "mainwindow.h"

#include <QListWidgetItem>
#include <DLabel>
#include <QFileDialog>
#include <QStandardPaths>

CMainWindow::CMainWindow(DWidget *w): DMainWindow (w)
{
    QString m_strPath;
    m_strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Pictures/摄像头";
    m_fileWatcher.addPath(m_strPath);
    m_strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Videos/摄像头";
    m_fileWatcher.addPath(m_strPath);

    initUI();
    initTitleBar();
    initConnection();
    m_devnumMonitor = new DevNumMonitor();
    m_devnumMonitor->start();

    connect(m_devnumMonitor, SIGNAL(seltBtnStateEnable()), &m_toolBar, SLOT(set_btn_state_enable()));
    connect(m_devnumMonitor, SIGNAL(seltBtnStateDisable()), &m_toolBar, SLOT(set_btn_state_disable()));
    connect(&m_toolBar, SIGNAL(sltCamera()), &m_videoPre, SLOT(changeDev()));
//    horizontalLayout_5->addLayout(m_thumbnail.m_hBOx);

//    QFileSystemWatcher *m_fileWatcher = new QFileSystemWatcher;
//    QString m_strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Pictures/摄像头";


//    m_fileWatcher->addPath(m_strPath);
//    connect(m_fileWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onFileChanged(const QString&)));
//    connect(m_fileWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(onFileChanged(const QString&)));
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::initUI()
{
    DWidget *wget = new DWidget;
    qDebug() << "initUI";
    QVBoxLayout *hboxlayout = new QVBoxLayout;

    QPalette *pal = new QPalette(m_videoPre.palette());
    pal->setColor(QPalette::Background, Qt::black); //设置背景黑色
    m_videoPre.setAutoFillBackground(true);
    m_videoPre.setPalette(*pal);
    hboxlayout->addWidget(&m_videoPre);

//    hboxlayout->addWidget(&m_toolBar);
//    hboxlayout->addWidget(&m_thumbnail);
//    hboxlayout->setStretch(0, 16);
//    hboxlayout->setStretch(1, 1);
//    hboxlayout->setStretch(2, 3);

    wget->setLayout(hboxlayout);
    setCentralWidget(wget);

    setupTitlebar();
    this->resize(850, 600);
}

void CMainWindow::initTitleBar()
{
    DButtonBox *pDButtonBox = new DButtonBox();
    pDButtonBox->setFixedWidth(200);
    QList<DButtonBoxButton *> listButtonBox;
    DButtonBoxButton *pDButtonBoxBtn1 = new DButtonBoxButton(QStringLiteral("拍照"));
    DButtonBoxButton *pDButtonBoxBtn2 = new DButtonBoxButton(QStringLiteral("视频"));
    listButtonBox.append(pDButtonBoxBtn1);
    listButtonBox.append(pDButtonBoxBtn2);
    pDButtonBox->setButtonList(listButtonBox, false);
    titlebar()->addWidget(pDButtonBox);

    DIconButton *pDIconBtn = new DIconButton(nullptr/*DStyle::SP_IndicatorSearch*/);
    titlebar()->setIcon(QIcon::fromTheme("preferences-system"));// /usr/share/icons/bloom/apps/96
    titlebar()->addWidget(pDIconBtn, Qt::AlignLeft);
    connect(pDIconBtn, SIGNAL(clicked()), &m_videoPre, SLOT(changeDev()));
}

void CMainWindow::initConnection()
{
    //系统文件夹变化信号
    connect(&m_fileWatcher, SIGNAL(directoryChanged(const QString &)), &m_thumbnail, SLOT(onFileChanged(const QString &)));
    //系统文件变化信号
    connect(&m_fileWatcher, SIGNAL(fileChanged(const QString &)), &m_thumbnail, SLOT(onFileChanged(const QString &)));
//    actionToken     m_actToken;

//    avCodec         m_avCodec;
//    CameraDetect    m_camDetect;
//    effectproxy     m_effProxy;
//    ToolBar         m_toolBar;
    //拍照按钮信号
    connect(&m_toolBar, SIGNAL(sltPhoto()), &m_videoPre, SLOT(onBtnPhoto()));

    //三连拍按钮信号
    connect(&m_toolBar, SIGNAL(sltThreeShot()), &m_videoPre, SLOT(onBtnThreeShots()));

    //录像按钮信号
    connect(&m_toolBar, SIGNAL(sltVideo()), &m_videoPre, SLOT(onBtnVideo()));
    //特效按钮信号
    connect(&m_toolBar, SIGNAL(sltEffect()), &m_videoPre, SLOT(onBtnEffect()));


    //拍照信号
    connect(&m_toolBar, SIGNAL(takepic()), &m_actToken, SLOT(onTakePic()));

    //三连拍信号
    connect(&m_toolBar, SIGNAL(threeShots()), &m_actToken, SLOT(onThreeShots()));
    //录像信号
    connect(&m_toolBar, SIGNAL(takeVideo()), &m_actToken, SLOT(onTakeVideo()));
    //三连拍取消信号
    connect(&m_toolBar, SIGNAL(cancelThreeShots()), &m_actToken, SLOT(onCancelThreeShots()));
    //录像结束信号
    connect(&m_toolBar, SIGNAL(takeVideoOver()), &m_actToken, SLOT(onTakeVideoOver()));
    //拍照信号--显示倒计时
    connect(&m_toolBar, SIGNAL(takepic()), &m_videoPre, SLOT(onShowCountdown()));
    //三连拍信号--显示倒计时
    connect(&m_toolBar, SIGNAL(threeShots()), &m_videoPre, SLOT(onShowThreeCountdown()));
    //录像信号--显示计时
    connect(&m_toolBar, SIGNAL(takeVideo()), &m_videoPre, SLOT(onTShowTime()));
    //三连拍取消信号
    //connect(&m_toolBar, SIGNAL(cancelThreeShots()), &m_videoPre, SLOT(onCancelThreeShots()));
    //录像结束信号
    //connect(&m_toolBar, SIGNAL(takeVideoOver()), &m_videoPre, SLOT(onTakeVideoOver()));
    //选择特效信号
    connect(&m_toolBar, SIGNAL(chooseEffect()), &m_videoPre, SLOT(onChooseEffect()));
    //特效选择左边按钮
    connect(&m_toolBar, SIGNAL(moreEffectLeft()), &m_videoPre, SLOT(onMoreEffectLeft()));
    //特效选择右边按钮
    connect(&m_toolBar, SIGNAL(moreEffectRight()), &m_videoPre, SLOT(onMoreEffectRight()));
    //找不到设备信号
    connect(&m_videoPre, SIGNAL(disableButtons()), &m_toolBar, SLOT(set_btn_state_no_dev()));
    //正常按钮时能信号
    connect(&m_videoPre, SIGNAL(ableButtons()), &m_toolBar, SLOT(set_btn_state_wth_dev()));
    //结束占用按钮状态改变
    connect(&m_videoPre, SIGNAL(finishTakedCamera()), &m_toolBar, SLOT(onFinishTakedCamera()));
    //结束特效选择信号
    connect(&m_videoPre, SIGNAL(finishEffectChoose()), &m_toolBar, SLOT(onFinishEffectChoose()));

}

void CMainWindow::setupTitlebar()
{
    DMenu *menu = new DMenu();
    QAction *settingAction(new QAction(tr("Settings"), this));
    menu->addAction(settingAction);
    titlebar()->setMenu(menu);
    connect(settingAction, &QAction::triggered, this, &CMainWindow::settingsTriggered);


    m_setwidget = new Set_Widget(centralWidget());
    m_setwidget->setBackgroundRole(QPalette::Background);
    m_setwidget->setAutoFillBackground(true);
    QPalette *plette = new QPalette();

    plette->setBrush(QPalette::Background, QBrush(QColor(64, 64, 64, 180), Qt::SolidPattern));
    plette->setBrush(QPalette::WindowText, QBrush(QColor(255, 255, 255, 255), Qt::SolidPattern));
    m_setwidget->setPalette(*plette);

    //m_setwidget->update();
    //m_setwidget->setGeometry(0, 15 + m_setwidget->height(), this->width(), this->height() - m_setwidget->height());
}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    int width = this->width();
    int height = this->height();
    m_setwidget->resize(width, height);
}

void CMainWindow::menuItemInvoked(QAction *action)
{

}

void CMainWindow::settingsTriggered(bool bTrue)
{
    m_setwidget->show();
}

void CMainWindow::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Escape) {
        m_setwidget->hide();
    }
    QWidget::keyReleaseEvent(ev);
}




