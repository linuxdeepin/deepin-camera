#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "actiontoken.h"
#include "avcodec.h"
#include "cameradetect.h"
#include "effectproxy.h"
#include "thumbnailsbar.h"
#include "toolbar.h"
#include "videoeffect.h"
#include "videopreviewwidget.h"
#include "widgetproxy.h"

#include <QObject>
#include <DMainWindow>
#include <QPushButton>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
DWIDGET_USE_NAMESPACE
class QGridLayout;


//应用层界面通信站，与底层通信通过proxy代理类
class CMainWindow : public DMainWindow
{
    Q_OBJECT
public:
    CMainWindow(DWidget *w = nullptr);


    void newPreViewByState(PRIVIEW_STATE state);
    void newNinePreview();
    void showPreviewByState(PRIVIEW_STATE state);


private:
    ThumbnailsBar   m_thumbnail;
    actionToken     m_actToken;
    avCodec         m_avCodec;
    CameraDetect    m_camDetect;
    effectproxy     m_effProxy;
    ToolBar         m_toolBar;
    videoEffect     m_videoEffect;
    videopreviewwidget  m_videoPre;
    widgetProxy     m_wgtProxy;

    QFileSystemWatcher m_fileWatcher;

    QVector<videopreviewwidget *> m_VEffectPreview;
    void initUI();
    void initConnection();

private:

};

#endif // MAINWINDOW_H
