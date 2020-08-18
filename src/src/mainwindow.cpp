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

#include "mainwindow.h"
#include "capplication.h"
#include <QListWidgetItem>
#include <QTextLayout>
#include <QStyleFactory>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFormLayout>
#include <qsettingbackend.h>

#include <DLabel>
#include <DSettingsDialog>
#include <DSettingsOption>
#include <DSettings>
#include <DLineEdit>
#include <DFileDialog>
#include <DDialog>
#include <DDesktopServices>
#include <DMessageBox>
#include <DComboBox>
#include <dsettingswidgetfactory.h>

#include "v4l2_core.h"

using namespace dc;
extern bool g_bMultiSlt; //是否多选
extern QSet<int> g_setIndex;
extern int g_indexNow;
extern QString g_strFileName;
extern int g_videoCount;

QString CMainWindow::m_lastfilename = {""};
static void workaround_updateStyle(QWidget *parent, const QString &theme)
{
    parent->setStyle(QStyleFactory::create(theme));
    for (auto obj : parent->children()) {
        auto w = qobject_cast<QWidget *>(obj);
        if (w) {
            workaround_updateStyle(w, theme);
        }
    }
}

static QString ElideText(const QString &text, const QSize &size,
                         QTextOption::WrapMode wordWrap, const QFont &font,
                         Qt::TextElideMode mode, int lineHeight, int lastLineWidth)
{
    int height = 0;

    QTextLayout textLayout(text);
    QString str = nullptr;
    QFontMetrics fontMetrics(font);

    textLayout.setFont(font);
    const_cast<QTextOption *>(&textLayout.textOption())->setWrapMode(wordWrap);

    textLayout.beginLayout();

    QTextLine line = textLayout.createLine();

    while (line.isValid()) {
        height += lineHeight;

        if (height + lineHeight >= size.height()) {
            str += fontMetrics.elidedText(text.mid(line.textStart() + line.textLength() + 1),
                                          mode, lastLineWidth);
            break;
        }

        line.setLineWidth(size.width());

        const QString &tmp_str = text.mid(line.textStart(), line.textLength());

        if (tmp_str.indexOf('\n'))
            height += lineHeight;

        str += tmp_str;

        line = textLayout.createLine();

        if (line.isValid())
            str.append("\n");
    }

    textLayout.endLayout();

    if (textLayout.lineCount() == 1) {
        str = fontMetrics.elidedText(str, mode, lastLineWidth);
    }

    return str;
}

static QWidget *createFormatLabelOptionHandle(QObject *opt)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(opt);

    auto *lab = new DLabel();
    auto *main = new DWidget;
    auto *layout = new QHBoxLayout;

    main->setLayout(layout);
    main->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(lab);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->setAlignment(Qt::AlignVCenter);
    lab->setObjectName("OptionFormatLabel");
    lab->setFixedHeight(15);
    QString str = option->value().toString();
    lab->setText(option->value().toString());
    QFont font = lab->font();
    font.setPointSize(11);
    lab->setFont(font);
    lab->setAlignment(Qt::AlignVCenter);
    lab->show();
    //lab->setEnabled(false);
//    auto optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, main);

    auto optionWidget = new QWidget;
    optionWidget->setObjectName("OptionFrame");

    auto optionLayout = new QFormLayout(optionWidget);
    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);

    main->setMinimumWidth(240);
    optionLayout->addRow(new DLabel(QObject::tr(option->name().toStdString().c_str())), main);

    optionWidget->setContentsMargins(0, 0, 0, 0);
    workaround_updateStyle(optionWidget, "light");
    return optionWidget;
}

//static QWidget *createResolutionComboBoxOptionHandle(QObject *opt)
//{
//}

static QWidget *createSelectableLineEditOptionHandle(QObject *opt)
{
    auto option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(opt);

    auto le = new DLineEdit();
    auto main = new DWidget;
    auto layout = new QHBoxLayout;

    static QString nameLast = nullptr;

    main->setLayout(layout);
    auto *icon = new DPushButton;
    icon->setAutoDefault(false);
    le->setFixedHeight(30);
    le->setObjectName("OptionSelectableLineEdit");
    le->setText(option->value().toString());
    auto fm = le->fontMetrics();
    auto pe = ElideText(le->text(), {285, fm.height()}, QTextOption::WrapAnywhere,
                        le->font(), Qt::ElideMiddle, fm.height(), 285);
    option->connect(le, &DLineEdit::focusChanged, [ = ](bool on) {
        if (on)
            le->setText(option->value().toString());

    });
    le->setText(pe);
    nameLast = pe;
//    icon->setIconVisible(true);
    icon->setIcon(QIcon(":/images/icons/light/select-normal.svg"));
    icon->setIconSize(QSize(25, 25));
    icon->setFixedHeight(30);
    layout->addWidget(le);
    layout->addWidget(icon);

//    auto optionWidget = DSettingsWidgetFactory::createTwoColumWidget(option, main);

    auto optionWidget = new QWidget;
    optionWidget->setObjectName("OptionFrame");

    auto optionLayout = new QFormLayout(optionWidget);
    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);

    main->setMinimumWidth(240);
    optionLayout->addRow(new DLabel(QObject::tr(option->name().toStdString().c_str())), main);

    workaround_updateStyle(optionWidget, "light");

    DDialog *prompt = new DDialog(optionWidget);
    prompt->setIcon(QIcon(":/images/icons/warning.svg"));
    //prompt->setTitle(QObject::tr("Permissions prompt"));
    prompt->setMessage(QObject::tr("You don't have permission to operate this folder"));
    prompt->setWindowFlags(prompt->windowFlags() | Qt::WindowStaysOnTopHint);
    prompt->addButton(QObject::tr("Close"), true, DDialog::ButtonRecommend);

    auto validate = [ = ](QString name, bool alert = true) -> bool {
        name = name.trimmed();
        if (name.isEmpty()) return false;

        if (name.size() && name[0] == '~')
        {
            name.replace(0, 1, QDir::homePath());
        }

        QFileInfo fi(name);
        QDir dir(name);
        if (fi.exists())
        {
            if (!fi.isDir()) {
                if (alert) le->showAlertMessage(QObject::tr("Invalid folder"));
                return false;
            }

            if (!fi.isReadable() || !fi.isWritable()) {
//                if (alert) le->showAlertMessage(QObject::tr("You don't have permission to operate this folder"));
                return false;
            }
        } else
        {
            if (dir.cdUp()) {
                QFileInfo ch(dir.path());
                if (!ch.isReadable() || !ch.isWritable())
                    return false;
            }
        }

        return true;
    };

    option->connect(icon, &DPushButton::clicked, [ = ]() {
        QString name = DFileDialog::getExistingDirectory(nullptr, QObject::tr("Open folder"),
                                                         CMainWindow::lastOpenedPath(),
                                                         DFileDialog::ShowDirsOnly | DFileDialog::DontResolveSymlinks);
        if (validate(name, false)) {
            option->setValue(name);
            nameLast = name;
        }
        QFileInfo fm(name);
        if ((!fm.isReadable() || !fm.isWritable()) && !name.isEmpty()) {
            prompt->show();
        }
    });



    option->connect(le, &DLineEdit::editingFinished, option, [ = ]() {

        QString name = le->text();
        QDir dir(name);

        auto pn = ElideText(name, {285, fm.height()}, QTextOption::WrapAnywhere,
                            le->font(), Qt::ElideMiddle, fm.height(), 285);
        auto nmls = ElideText(nameLast, {285, fm.height()}, QTextOption::WrapAnywhere,
                              le->font(), Qt::ElideMiddle, fm.height(), 285);

        if (!validate(le->text(), false)) {
            QFileInfo fn(dir.path());
            if ((!fn.isReadable() || !fn.isWritable()) && !name.isEmpty()) {
                prompt->show();
            }
        }
        if (!le->lineEdit()->hasFocus()) {
            if (validate(le->text(), false)) {
                option->setValue(le->text());
                le->setText(pn);
                nameLast = name;
            } else if (pn == pe) {
                le->setText(pe);
            } else {
                option->setValue(QVariant("~/Videos"));//设置为默认路径
                le->setText(QString("~/Videos"));
                option->setValue(QVariant("~/Videos"));
                le->setText(nmls);
            }
        }
    });

    option->connect(le, &DLineEdit::textEdited, option, [ = ](const QString & newStr) {
        validate(newStr);
    });

    option->connect(option, &DTK_CORE_NAMESPACE::DSettingsOption::valueChanged, le,
    [ = ](const QVariant & value) {
        auto pi = ElideText(value.toString(), {285, fm.height()}, QTextOption::WrapAnywhere,
                            le->font(), Qt::ElideMiddle, fm.height(), 285);
        le->setText(pi);
        le->update();
    });

    return  optionWidget;
}

QString CMainWindow::lastOpenedPath()
{
    QString lastPath = Settings::get().generalOption("last_open_path").toString();
    QDir lastDir(lastPath);
    if (lastPath.isEmpty() || !lastDir.exists()) {
        lastPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
        QDir newLastDir(lastPath);
        if (!newLastDir.exists()) {
            lastPath = QDir::currentPath();
        }
    }

    return lastPath;
}

CMainWindow::CMainWindow(DWidget *w): DMainWindow (w)
{
    m_devnumMonitor = new DevNumMonitor();
    m_devnumMonitor->start();
    m_nActTpye = ActTakePic;
    initUI();
    initTitleBar();
    initConnection();
}

CMainWindow::~CMainWindow()
{
    qDebug() << "stop_encoder_thread";
}

void CMainWindow::slotPopupSettingsDialog()
{
    auto dsd = new DSettingsDialog(this);
    dsd->widgetFactory()->registerWidget("selectableEdit", createSelectableLineEditOptionHandle);
    dsd->widgetFactory()->registerWidget("formatLabel", createFormatLabelOptionHandle);

    connect(dsd, SIGNAL(destroyed()), this, SLOT(onSettingsDlgClose()));

    auto resolutionmodeFamily = Settings::get().settings()->option("outsetting.resolutionsetting.resolution");

    if (get_v4l2_device_handler() != nullptr) {
        //格式索引
        int format_index = v4l2core_get_frame_format_index(
                               get_v4l2_device_handler(),
                               v4l2core_get_requested_frame_format(get_v4l2_device_handler()));

        //分辨率索引
        int resolu_index = v4l2core_get_format_resolution_index(
                               get_v4l2_device_handler(),
                               format_index,
                               v4l2core_get_frame_width(get_v4l2_device_handler()),
                               v4l2core_get_frame_height(get_v4l2_device_handler()));

        //获取当前摄像头的格式表
        v4l2_stream_formats_t *list_stream_formats = v4l2core_get_formats_list(get_v4l2_device_handler());

        //初始化分辨率字符串表
        QStringList resolutionDatabase;
        resolutionDatabase.clear();

        //当前分辨率下标
        int defres = 0;
        if (format_index >= 0 && resolu_index >= 0) {//format_index = -1 &&resolu_index = -1 表示设备被占用或者不存在
            for (int i = 0 ; i < list_stream_formats[format_index].numb_res; i++) {

                if ((list_stream_formats[format_index].list_stream_cap[i].width > 0
                        && list_stream_formats[format_index].list_stream_cap[i].height > 0) &&
                        (list_stream_formats[format_index].list_stream_cap[i].width < 7680
                         && list_stream_formats[format_index].list_stream_cap[i].height < 4320) &&
                        ((list_stream_formats[format_index].list_stream_cap[i].width % 8) == 0
                         && (list_stream_formats[format_index].list_stream_cap[i].height % 8) ==  0)) {
                    //加入分辨率的字符串
                    QString res_str = QString( "%1x%2").arg(list_stream_formats[format_index].list_stream_cap[i].width).arg(list_stream_formats[format_index].list_stream_cap[i].height);
                    resolutionDatabase.append(res_str);
                }
            }
            int tempostion = 0;
            int len = resolutionDatabase.size() - 1;
            for (int i = 0; i < resolutionDatabase.size() - 1; i++) {
                int flag = 1;
                for (int j = 0 ; j < len; j++) {
                    QStringList resolutiontemp1 = resolutionDatabase[j].split("x");
                    QStringList resolutiontemp2 = resolutionDatabase[j + 1].split("x");

                    if ((resolutiontemp1[0].toInt() <= resolutiontemp2[0].toInt())
                            && (resolutiontemp1[1].toInt() < resolutiontemp2[1].toInt())) {

                        QString resolutionstr = resolutionDatabase[j + 1];
                        resolutionDatabase[j + 1] = resolutionDatabase[j];
                        resolutionDatabase[j] = resolutionstr;
                        flag = 0;
                        tempostion = j;
                    }
                }
                len = tempostion;
                if (flag == 1) {
                    continue;
                }
            }

            for (int i = 0; i < resolutionDatabase.size(); i++) {
                QStringList resolutiontemp = resolutionDatabase[i].split("x");
                if ((v4l2core_get_frame_width(get_v4l2_device_handler()) == resolutiontemp[0].toInt()) &&
                        (v4l2core_get_frame_height(get_v4l2_device_handler()) == resolutiontemp[1].toInt())) {
                    defres = i; //set selected resolution index
                    break;
                }
            }

            resolutionmodeFamily->setData("items", resolutionDatabase);

            //设置当前分辨率的索引
            Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        } else {
            resolutionDatabase.clear();
            resolutionDatabase.append(QString(tr("None")));
            Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
            resolutionmodeFamily->setData("items", resolutionDatabase);
        }
    } else {
        //初始化分辨率字符串表
        QStringList resolutionDatabase = resolutionmodeFamily->data("items").toStringList();
        if (resolutionDatabase.size() > 0) {
            resolutionmodeFamily->data("items").clear();
        }
        resolutionDatabase.clear();
        resolutionDatabase.append(QString(tr("None")));
        Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
        resolutionmodeFamily->setData("items", resolutionDatabase);
    }

    dsd->setProperty("_d_QSSThemename", "dark");
    dsd->setProperty("_d_QSSFilename", "DSettingsDialog");
    dsd->updateSettings(Settings::get().settings());

    auto reset = dsd->findChild<QPushButton *>("SettingsContentReset");
    reset->setDefault(false);
    reset->setAutoDefault(false);

    dsd->exec();
    delete dsd;
    Settings::get().settings()->sync();
}

void CMainWindow::initBlockShutdown()
{
    if (!m_arg.isEmpty() || m_reply.value().isValid()) {
        qDebug() << "m_reply.value().isValid():" << m_reply.value().isValid();
        return;
    }

    m_pLoginManager = new QDBusInterface("org.freedesktop.login1",
                                         "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager",
                                         QDBusConnection::systemBus());

    m_arg << QString("shutdown")             // what
          << qApp->productName()           // who
          << QObject::tr("File not saved")          // why
          << QString("block");                        // mode

    //int fd = -1;
    m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    if (m_reply.isValid()) {
        /*fd = */(void)m_reply.value().fileDescriptor();
    }
    //如果for结束则表示没有发现未保存的tab项，则放开阻塞关机
    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
        qDebug() << "Nublock shutdown.";
    }
}

void CMainWindow::updateBlockSystem(bool bTrue)
{
    initBlockShutdown();

    if (bTrue) {
        m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);
    } else {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
        qDebug() << "Nublock shutdown.";
    }
}

void CMainWindow::onNoCam()
{
    onEnableTitleBar(3); //恢复按钮状态
    onEnableTitleBar(4); //恢复按钮状态
    onEnableSettings(true);
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->setBtntooltip();
    //m_thumbnail->onFoldersChanged(""); //恢复缩略图
    m_thumbnail->show();

}

void CMainWindow::initUI()
{
//    this->setWindowFlag(Qt::FramelessWindowHint);
    m_closeDlg = new CloseDialog(this, tr("Video recording is in progress. Close the window?"));
//    DWidget *wget = new DWidget;
//    QHBoxLayout *hboxlayout = new QHBoxLayout(this);
    this->resize(MinWindowWidth, MinWindowHeight);
    m_videoPre = new videowidget(this);

    QPalette paletteTime = m_videoPre->palette();
    paletteTime.setBrush(QPalette::Dark, QColor(/*"#202020"*/0, 0, 0, 51)); //深色
    m_videoPre->setPalette(paletteTime);

    CMainWindow::m_lastfilename = Settings::get().getOption("base.save.datapath").toString();
    if (CMainWindow::m_lastfilename.size() && CMainWindow::m_lastfilename[0] == '~') {
        QString str = QDir::homePath();
        CMainWindow::m_lastfilename.replace(0, 1, str);
    }
    if (!QDir(CMainWindow::m_lastfilename).exists())
        CMainWindow::m_lastfilename = QDir::homePath() + QString("/Videos");
    QString test1 = CMainWindow::m_lastfilename;

    m_videoPre->setSaveFolder(CMainWindow::m_lastfilename);
    if (QFileInfo(CMainWindow::m_lastfilename).exists()) {
        m_fileWatcher.addPath(CMainWindow::m_lastfilename);
    }
    this->setCentralWidget(m_videoPre);


    setupTitlebar();

    m_thumbnail = new ThumbnailsBar(this);
    m_thumbnail->move(0, height() - 10);
    m_thumbnail->setFixedHeight(LAST_BUTTON_HEIGHT + LAST_BUTTON_SPACE * 2);

    //添加右键打开文件夹功能
    QMenu *menu = new QMenu();
    QAction *actOpen = new QAction(this);
    actOpen->setText(tr("Open folder"));
    menu->addAction(actOpen);
    m_thumbnail->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_thumbnail, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
        Q_UNUSED(pos);
        menu->exec(QCursor::pos());
    });
    connect(actOpen, &QAction::triggered, this, [ = ] {
        QString save_path = Settings::get().generalOption("last_open_path").toString();
        if (save_path.isEmpty())
        {
            save_path = Settings::get().getOption("base.save.datapath").toString();
        }
        if (save_path.size() && save_path[0] == '~')
        {
            save_path.replace(0, 1, QDir::homePath());
        }

        if (!QFileInfo(save_path).exists())
        {
            QDir d;
            d.mkpath(save_path);
        }
        Dtk::Widget::DDesktopServices::showFolder(save_path);
    });

    m_thumbnail->show();
    m_thumbnail->setVisible(true);
    m_thumbnail->show();

    //m_thumbnail->setMaximumWidth(1200);
    m_thumbnail->m_nMaxItem = MinWindowWidth;


    QString test = CMainWindow::m_lastfilename;

    m_videoPre->setSaveFolder(CMainWindow::m_lastfilename);
    int nContinuous = Settings::get().getOption("photosetting.photosnumber.takephotos").toInt();
    int nDelayTime = Settings::get().getOption("photosetting.photosdelay.photodelays").toInt();
    switch (nContinuous) {
    case 1:
        nContinuous = 4;
        break;
    case 2:
        nContinuous = 10;
        break;
    default:
        nContinuous = 1;
        break;
    }
    switch (nDelayTime) {
    case 1:
        nDelayTime = 3;
        break;
    case 2:
        nDelayTime = 6;
        break;
    default:
        nDelayTime = 0;
        break;
    }
    m_videoPre->setInterval(nDelayTime);
    m_videoPre->setContinuous(nContinuous);
    this->resize(MinWindowWidth, MinWindowHeight);
    m_thumbnail->addPath(CMainWindow::m_lastfilename);
}

void CMainWindow::initTitleBar()
{
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    pDButtonBox = new DButtonBox();
    pDButtonBox->setFixedWidth(120);
    pDButtonBox->setFixedHeight(36);
    QList<DButtonBoxButton *> listButtonBox;
    QIcon iconPic(":/images/icons/light/button/photograph.svg");
    m_pTitlePicBtn = new DButtonBoxButton(nullptr/*iconPic*/);

    m_pTitlePicBtn->setIcon(iconPic);
    m_pTitlePicBtn->setIconSize(QSize(26, 26));

    DPalette pa = m_pTitlePicBtn->palette();
    QColor clo("#0081FF");
    pa.setColor(DPalette::Dark, clo);
    pa.setColor(DPalette::Light, clo);

    pa.setColor(DPalette::Button, clo);
    m_pTitlePicBtn->setPalette(pa);
    QIcon iconVd;

    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType)
        iconVd = QIcon(":/images/icons/light/record video.svg");
    else
        iconVd = QIcon(":/images/icons/dark/button/record video_dark.svg");

    m_pTitleVdBtn = new DButtonBoxButton(nullptr);
    m_pTitleVdBtn->setFocusPolicy(Qt::NoFocus);
    m_pTitleVdBtn->setIcon(iconVd);
    m_pTitleVdBtn->setIconSize(QSize(26, 26));

    listButtonBox.append(m_pTitlePicBtn);
    listButtonBox.append(m_pTitleVdBtn);
    pDButtonBox->setButtonList(listButtonBox, false);
    titlebar()->addWidget(pDButtonBox);


    pSelectBtn = new DIconButton(nullptr/*DStyle::SP_IndicatorSearch*/);
    pSelectBtn->setFixedSize(QSize(37, 37));
    pSelectBtn->setIconSize(QSize(37, 37));
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        pSelectBtn->setIcon(QIcon(":/images/icons/light/button/Switch camera.svg"));
    } else {
        pSelectBtn->setIcon(QIcon(":/images/icons/dark/button/Switch camera_dark.svg"));
    }

    titlebar()->setIcon(QIcon::fromTheme(":/images/logo/deepin-camera-32px.svg"));// /usr/share/icons/bloom/apps/96 //preferences-system
    titlebar()->addWidget(pSelectBtn, Qt::AlignLeft);
}

void CMainWindow::initConnection()
{
    connect(dApp, &CApplication::popupConfirmDialog, this, [ = ] {
        if (m_videoPre->getCapstatus())
        {
            int ret = m_closeDlg->exec();
            if (ret == 1) {
                m_videoPre->endBtnClicked();
                dApp->quit();
            }
        } else
        {
            dApp->quit();
        }
    });
    //connect(this, SIGNAL(windowstatechanged(Qt::WindowState windowState)), this, SLOT(onCapturepause(Qt::WindowState windowState)));
    //系统文件夹变化信号
//    connect(&m_fileWatcher, SIGNAL(directoryChanged(const QString &)), m_thumbnail, SLOT(onFoldersChanged(const QString &)));
    //系统文件变化信号
    connect(&m_fileWatcher, SIGNAL(fileChanged(const QString &)), m_thumbnail, SLOT(onFileChanged(const QString &)));
    //增删文件修改界面
    connect(m_thumbnail, SIGNAL(fitToolBar()), this, SLOT(onFitToolBar()));

    //修改标题栏按钮状态
    connect(m_thumbnail, SIGNAL(enableTitleBar(int)), this, SLOT(onEnableTitleBar(int)));
    //录像信号
    connect(m_thumbnail, SIGNAL(takeVd()), m_videoPre, SLOT(onTakeVideo()));
    //设置按钮信号
    connect(m_actionSettings, &QAction::triggered, this, &CMainWindow::slotPopupSettingsDialog);
    //禁用设置
    connect(m_thumbnail, SIGNAL(enableSettings(bool)), this, SLOT(onEnableSettings(bool)));
    //拍照信号--显示倒计时
    connect(m_thumbnail, SIGNAL(takePic(bool)), m_videoPre, SLOT(onTakePic(bool)));

    connect(&Settings::get(), SIGNAL(resolutionchanged(const QString &)), m_videoPre, SLOT(slotresolutionchanged(const QString &)));

    connect(m_videoPre, SIGNAL(takePicOnce()), this, SLOT(onTakePicOnce()));
    //拍照取消
    connect(m_videoPre, SIGNAL(takePicCancel()), this, SLOT(onTakePicCancel()));
    //拍照结束
    connect(m_videoPre, SIGNAL(takePicDone()), this, SLOT(onTakePicDone()));
    //录制结束
    connect(m_videoPre, SIGNAL(takeVdDone()), this, SLOT(onTakeVdDone()));
    //录制取消 （倒计时3秒内的）
    connect(m_videoPre, SIGNAL(takeVdCancel()), this, SLOT(onTakeVdCancel()));
    //录制关机阻塞
    connect(m_videoPre, SIGNAL(updateBlockSystem(bool)), this, SLOT(updateBlockSystem(bool)));
    //没有相机了，结束拍照、录制
    connect(m_videoPre, SIGNAL(noCam()), this, SLOT(onNoCam()));
    //相机被抢占了，结束拍照、录制
    connect(m_videoPre, SIGNAL(noCamAvailable()), this, SLOT(onNoCam()));
    //设备切换信号
    connect(pSelectBtn, SIGNAL(clicked()), m_videoPre, SLOT(changeDev()));

    connect(m_devnumMonitor, SIGNAL(seltBtnStateEnable()), this, SLOT(setSelBtnShow()));
    //多设备信号
    connect(m_devnumMonitor, SIGNAL(seltBtnStateDisable()), this, SLOT(setSelBtnHide()));

    connect(m_devnumMonitor, SIGNAL(existDevice()), m_videoPre, SLOT(restartDevices()));

    connect(m_videoPre, SIGNAL(sigDeviceChange()), &Settings::get(), SLOT(setNewResolutionList()));

    //标题栏图片按钮
    connect(m_pTitlePicBtn, SIGNAL(clicked()), this, SLOT(onTitlePicBtn()));
    //标题栏视频按钮
    connect(m_pTitleVdBtn, SIGNAL(clicked()), this, SLOT(onTitleVdBtn()));
    //connect(m_closeDlg, SIGNAL(buttonClicked(int index, const QString & text)), this, SLOT(onCloseDlgBtnClicked(int index, const QString & text)));

    //主题变换
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CMainWindow::onThemeChange);
}
void CMainWindow::setSelBtnHide()
{
    pSelectBtn->hide();
}

void CMainWindow::setSelBtnShow()
{
    pSelectBtn->show();
}

void CMainWindow::setupTitlebar()
{
    menu = new DMenu();
    m_actionSettings = new QAction(tr("Settings"), this);
    menu->addAction(m_actionSettings);
    titlebar()->setMenu(menu);
}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    int width = this->width();
    //int height = this->height();
    m_thumbnail->m_nMaxItem = width;
//    if (nOldWidth > width) {//画面缩小了，要重新调整；
//        m_thumbnail->onFoldersChanged("");
//    } else {//放大时也调整，不使用延迟屏幕闪烁比较严重，解决此问题需要重新实现缩略图部分功能
//        m_thumbnail->hide();
//        QTimer::singleShot(200, this, [=]
//        {
//            m_thumbnail->onFoldersChanged("");
//            m_thumbnail->show();
//        });
//    }
    //m_thumbnail->onFoldersChanged("");
    if (m_thumbnail) {
        onFitToolBar();
    }
    //m_videoPre->resize(this->size());
    m_videoPre->update();
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_videoPre->getCapstatus()) {
        int ret = m_closeDlg->exec();
        switch (ret) {
        case 0:
            event->ignore();
            break;
        case 1:
            m_videoPre->endBtnClicked();
            event->accept();
            break;
        default:
            event->ignore();
            break;
        }
    }
}

void CMainWindow::changeEvent(QEvent *event)
{
    Q_UNUSED(event);
//    qDebug() << this->windowState() << endl;
    if (this->windowState() == Qt::WindowMinimized) {
        set_capture_pause(1);
    } else if (this->windowState() == (Qt::WindowMinimized | Qt::WindowMaximized)) {
        set_capture_pause(1);
    } else if (this->isVisible() == true) {
        set_capture_pause(0);
    }
}

void CMainWindow::onFitToolBar()
{
    if (m_thumbnail) {
        int n = m_thumbnail->getItemCount();
        int nWidth = 0;
        if (n <= 0) {
            nWidth = LAST_BUTTON_SPACE * 2 + LAST_BUTTON_WIDTH;
            m_thumbnail->m_showVdTime->hide();
        } else {
            if (g_videoCount <= 0) {
                m_thumbnail->m_showVdTime->hide();
                nWidth = n * THUMBNAIL_WIDTH + ITEM_SPACE * (n - 1) + LAST_BUTTON_SPACE * 4 + LAST_BUTTON_WIDTH + 8;//4是选中边框宽度
            } else {
                m_thumbnail->m_showVdTime->show();
                nWidth = n * THUMBNAIL_WIDTH + ITEM_SPACE * (n - 1) + LAST_BUTTON_SPACE * 5 + LAST_BUTTON_WIDTH + 8 + 4 + VIDEO_TIME_WIDTH;//4是选中边框宽度
            }
        }

        m_thumbnail->resize(qMin(this->width(), nWidth), THUMBNAIL_HEIGHT + 30);

        m_thumbnail->move((this->width() - m_thumbnail->width()) / 2,
                          this->height() - m_thumbnail->height() - 5);

        //qDebug() << "nCount " << n << " nWidth " << nWidth << " winWidth " << this->width();

        //m_thumbnail->update();
    }
}

void CMainWindow::onEnableTitleBar(int nType)
{
    //1、禁用标题栏视频；2、禁用标题栏拍照；3、恢复标题栏视频；4、恢复标题栏拍照
    switch (nType) {
    case 1:
        m_pTitleVdBtn->setEnabled(false);
        pSelectBtn->setEnabled(false);
        break;
    case 2:
        m_pTitlePicBtn->setEnabled(false);
        pSelectBtn->setEnabled(false);
        break;
    case 3:
        m_pTitleVdBtn->setEnabled(true);
        pSelectBtn->setEnabled(true);
        break;
    case 4:
        m_pTitlePicBtn->setEnabled(true);
        pSelectBtn->setEnabled(true);
        break;
    default:
        break;
    }
}

void CMainWindow::menuItemInvoked(QAction *action)
{
    Q_UNUSED(action);
}

void CMainWindow::onTitlePicBtn()
{
    if (m_nActTpye == ActTakePic) {
        return;
    }
    int type = DGuiApplicationHelper::instance()->themeType();
    m_nActTpye = ActTakePic;
    //切换标题栏拍照按钮颜色
    DPalette pa = m_pTitlePicBtn->palette();
    QColor clo("#0081FF");
    pa.setColor(DPalette::Dark, clo);
    pa.setColor(DPalette::Light, clo);
    pa.setColor(DPalette::Button, clo);
    m_pTitlePicBtn->setPalette(pa);

    QIcon iconPic(":/images/icons/light/button/photograph.svg");
    m_pTitlePicBtn->setIcon(iconPic);

    //切换标题栏视频按钮颜色
    DPalette paVd = m_pTitleVdBtn->palette();
    QColor cloVd("#000000");
    cloVd.setAlpha(20);
    paVd.setColor(DPalette::Dark, cloVd);
    paVd.setColor(DPalette::Light, cloVd);
    paVd.setColor(DPalette::Button, cloVd);
    m_pTitleVdBtn->setPalette(paVd);
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        QIcon iconVd(":/images/icons/light/record video.svg");
        m_pTitleVdBtn->setIcon(iconVd);
    } else {
        QIcon iconVd(":/images/icons/dark/button/record video_dark.svg");
        m_pTitleVdBtn->setIcon(iconVd);
    }
    m_thumbnail->ChangeActType(m_nActTpye);
}

void CMainWindow::onTitleVdBtn()
{
    if (m_nActTpye == ActTakeVideo) {
        return;
    }
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    m_nActTpye = ActTakeVideo;
    //切换标题栏视频按钮颜色
    DPalette paPic = m_pTitleVdBtn->palette();
    QColor cloPic("#0081FF");
    paPic.setColor(DPalette::Dark, cloPic);
    paPic.setColor(DPalette::Light, cloPic);
    paPic.setColor(DPalette::Button, cloPic);
    m_pTitleVdBtn->setPalette(paPic);

    QIcon iconVd(":/images/icons/light/button/transcribe.svg");
    m_pTitleVdBtn->setIcon(iconVd);

    //切换标题栏拍照按钮颜色
    DPalette paVd = m_pTitlePicBtn->palette();
    QColor cloVd("#000000");
    cloVd.setAlpha(20);
    paVd.setColor(DPalette::Dark, cloVd);
    paVd.setColor(DPalette::Light, cloVd);
    paVd.setColor(DPalette::Button, cloVd);
    m_pTitlePicBtn->setPalette(paVd);
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        QIcon iconVd(":/images/icons/light/photograph.svg");
        m_pTitlePicBtn->setIcon(iconVd);
    } else {
        QIcon iconPic(":/images/icons/dark/button/photograph_dark.svg");
        m_pTitlePicBtn->setIcon(iconPic);
    }
    m_thumbnail->ChangeActType(m_nActTpye);
}

void CMainWindow::onSettingsDlgClose()
{
    /**********************************************/
    if (QDir(Settings::get().getOption("base.save.datapath").toString()).exists() == false) {
        CMainWindow::m_lastfilename = QString("~") + QString("/Videos");
        Settings::get().setPathOption("datapath", QVariant(CMainWindow::m_lastfilename));
    }

    if (CMainWindow::m_lastfilename.size() && CMainWindow::m_lastfilename[0] == '~') {
        CMainWindow::m_lastfilename.replace(0, 1, QDir::homePath());
    }

    CMainWindow::m_lastfilename = Settings::get().getOption("base.save.datapath").toString();
    if (QDir(CMainWindow::m_lastfilename).exists() == false) {
        CMainWindow::m_lastfilename = QDir::homePath() + QString("/Videos");
    }

//    QString test = CMainWindow::m_lastfilename;
    m_videoPre->setSaveFolder(CMainWindow::m_lastfilename);
    m_fileWatcher.addPath(CMainWindow::m_lastfilename);
    m_thumbnail->addPath(CMainWindow::m_lastfilename);

    int nContinuous = Settings::get().getOption("photosetting.photosnumber.takephotos").toInt();
    int nDelayTime = Settings::get().getOption("photosetting.photosdelay.photodelays").toInt();


    /**********************************************/
    switch (nContinuous) {
    case 1:
        nContinuous = 4;
        break;
    case 2:
        nContinuous = 10;
        break;
    default:
        nContinuous = 1;
        break;
    }
    switch (nDelayTime) {
    case 1:
        nDelayTime = 3;
        break;
    case 2:
        nDelayTime = 6;
        break;
    default:
        nDelayTime = 0;
        break;
    }
    m_videoPre->setInterval(nDelayTime);
    m_videoPre->setContinuous(nContinuous);
}

void CMainWindow::onEnableSettings(bool bTrue)
{
    m_actionSettings->setEnabled(bTrue);
}

void CMainWindow::onTakePicDone()
{
    qDebug() << "onTakePicDone";
    onEnableTitleBar(3); //恢复按钮状态
    onEnableSettings(true);
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->setBtntooltip();
    QString strPath = m_videoPre->m_imgPrcThread->m_strPath;
    m_thumbnail->addFile(strPath);
}

void CMainWindow::onTakePicOnce()
{
    qDebug() << "onTakePicOnce";
    m_thumbnail->addFile(m_videoPre->m_imgPrcThread->m_strPath);
}

void CMainWindow::onTakePicCancel()
{
    onEnableTitleBar(3); //恢复按钮状态
    onEnableSettings(true);
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->setBtntooltip();
}

void CMainWindow::onTakeVdDone()
{
    onEnableTitleBar(4); //恢复按钮状态
    m_thumbnail->m_nStatus = STATNULL;
    //m_thumbnail->onFoldersChanged(""); //恢复缩略图
    m_thumbnail->show();
    onEnableSettings(true);
    QTimer::singleShot(200, this, [ = ] {
        QString strFileName = m_videoPre->getFolder() + "/" + g_strFileName;
        QFile file(strFileName);
        if (!file.exists())
        {
            usleep(200000);
        }
        m_thumbnail->addFile(strFileName);
    });
}

void CMainWindow::onTakeVdCancel()   //保存视频完成，通过已有的文件检测实现缩略图恢复，这里不需要额外处理
{
    onEnableTitleBar(4); //恢复按钮状态
    m_thumbnail->m_nStatus = STATNULL;
    //m_thumbnail->onFoldersChanged(""); //恢复缩略图
    m_thumbnail->show();
    onEnableSettings(true);
}

void CMainWindow::onThemeChange(DGuiApplicationHelper::ColorType type)
{
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        pSelectBtn->setIconSize(QSize(37, 37));
        pSelectBtn->setIcon(QIcon(":/images/icons/light/button/Switch camera.svg"));
        if (m_nActTpye == ActTakePic) {
            m_pTitleVdBtn->setIcon(QIcon(":/images/icons/light/record video.svg"));
        } else {
            m_pTitlePicBtn->setIcon(QIcon(":/images/icons/light/photograph.svg"));
        }
    }
    if (type == DGuiApplicationHelper::DarkType) {
        pSelectBtn->setIcon(QIcon(":/images/icons/dark/button/Switch camera_dark.svg"));
        if (m_nActTpye == ActTakePic) {
            //pSelectBtn->setIconSize(QSize(20, 20));
            m_pTitleVdBtn->setIcon(QIcon(":/images/icons/dark/button/record video_dark.svg"));
        } else {
            m_pTitlePicBtn->setIcon(QIcon(":/images/icons/dark/button/photograph_dark.svg"));
        }
    }
}

void CMainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) {
        qDebug() << "shift pressed";
        g_bMultiSlt = true;
        g_setIndex.insert(g_indexNow);
    }
}

void CMainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) {
        qDebug() << "shift released";
        g_bMultiSlt = false;
        //g_setIndex.clear();
        //g_setIndex.insert(g_indexNow);
    }
}
