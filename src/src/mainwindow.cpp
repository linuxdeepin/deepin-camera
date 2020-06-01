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
    initConnection();

//    horizontalLayout_5->addLayout(m_thumbnail.m_hBOx);

//    QFileSystemWatcher *m_fileWatcher = new QFileSystemWatcher;
//    QString m_strPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/Pictures/摄像头";


//    m_fileWatcher->addPath(m_strPath);
//    connect(m_fileWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onFileChanged(const QString&)));
//    connect(m_fileWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(onFileChanged(const QString&)));
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

    hboxlayout->addWidget(&m_toolBar);
    hboxlayout->addWidget(&m_thumbnail);
    hboxlayout->setStretch(0, 16);
    hboxlayout->setStretch(1, 1);
    hboxlayout->setStretch(2, 3);

    wget->setLayout(hboxlayout);
    setCentralWidget(wget);
//    this->setMinimumWidth(600);
    this->resize(500,500);
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

//    videoEffect     m_videoEffect;
//    videopreviewwidget  m_videoPre;
//    widgetProxy     m_wgtProxy;
}


