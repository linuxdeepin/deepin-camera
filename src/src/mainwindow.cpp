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
#include "v4l2_core.h"
#include "datamanager.h"
#include "shortcut.h"
#include "ac-deepin-camera-define.h"

#include <DLabel>
#include <DApplication>
#include <DSettingsDialog>
#include <DSettingsOption>
#include <DSettings>
#include <DLineEdit>
#include <DFileDialog>
#include <DDialog>
#include <DDesktopServices>
#include <DMessageBox>
#include <DComboBox>
#include <DWindowMinButton>
#include <DWindowCloseButton>
#include <DWindowMaxButton>
#include <DWindowOptionButton>
#include <DWindowQuitFullButton>

#include <QListWidgetItem>
#include <QTextLayout>
#include <QStyleFactory>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFormLayout>
#include <QShortcut>
#include <QLibraryInfo>

#include <qsettingbackend.h>
#include <dsettingswidgetfactory.h>

extern "C" {
#include "encoder.h"
#include "load_libs.h"
}

using namespace dc;

const int CMainWindow::minWindowWidth = 800;
const int CMainWindow::minWindowHeight = 583;

QString CMainWindow::lastVdFileName = {""};
QString CMainWindow::lastPicFileName = {""};

static void workaround_updateStyle(QWidget *parent, const QString &theme)
{
    parent->setStyle(QStyleFactory::create(theme));
    for (auto obj : parent->children()) {
        QWidget *tmp_widget = qobject_cast<QWidget *>(obj);
        if (tmp_widget)
            workaround_updateStyle(tmp_widget, theme);
    }
}

static QString ElideText(const QString &text, const QSize &size,
                         QTextOption::WrapMode wordWrap, const QFont &font,
                         Qt::TextElideMode mode, int lineHeight, int lastLineWidth)
{
    int tmpheight = 0;
    QTextLayout textLayout(text);
    QString textlinestr = nullptr;
    QFontMetrics fontMetrics(font);
    textLayout.setFont(font);
    const_cast<QTextOption *>(&textLayout.textOption())->setWrapMode(wordWrap);
    textLayout.beginLayout();
    QTextLine tmptextline = textLayout.createLine();

    while (tmptextline.isValid()) {
        tmpheight += lineHeight;

        if (tmpheight + lineHeight >= size.height()) {
            textlinestr += fontMetrics.elidedText(text.mid(tmptextline.textStart() + tmptextline.textLength() + 1),
                                                  mode, lastLineWidth);
            break;
        }
        tmptextline.setLineWidth(size.width());
        const QString &tmp_str = text.mid(tmptextline.textStart(), tmptextline.textLength());

        if (tmp_str.indexOf('\n'))
            tmpheight += lineHeight;

        textlinestr += tmp_str;
        tmptextline = textLayout.createLine();

        if (tmptextline.isValid())
            textlinestr.append("\n");

    }

    textLayout.endLayout();

    if (textLayout.lineCount() == 1)
        textlinestr = fontMetrics.elidedText(textlinestr, mode, lastLineWidth);

    return textlinestr;
}

static QWidget *createFormatLabelOptionHandle(QObject *opt)
{
    DTK_CORE_NAMESPACE::DSettingsOption *option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(opt);
    DLabel *lab = new DLabel();
    DWidget *main = new DWidget();
    QHBoxLayout *layout = new QHBoxLayout;
    QWidget *optionWidget = new QWidget;
    QFormLayout *optionLayout = new QFormLayout(optionWidget);

    main->setLayout(layout);
    main->setContentsMargins(0, 0, 0, 0);
    main->setMinimumWidth(240);
    layout->addWidget(lab);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignVCenter);
    lab->setObjectName(OPTION_FORMAT_LABER);
    lab->setAccessibleName(OPTION_FORMAT_LABER);
    lab->setFixedHeight(15);
    lab->setText(option->value().toString());
    QFont font = lab->font();
    font.setPointSize(11);
    lab->setFont(font);
    lab->setAlignment(Qt::AlignVCenter);
    lab->show();
    optionWidget->setObjectName(OPTION_FRAME);
    optionWidget->setAccessibleName(OPTION_FRAME);
    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);
    optionLayout->addRow(new DLabel(QObject::tr(option->name().toStdString().c_str())), main);
    optionWidget->setContentsMargins(0, 0, 0, 0);
    workaround_updateStyle(optionWidget, "light");

    return optionWidget;
}

static QWidget *createPicSelectableLineEditOptionHandle(QObject *opt)
{
    DTK_CORE_NAMESPACE::DSettingsOption *option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(opt);
    option->setObjectName(PIC_OPTION);
    DLineEdit *picPathLineEdit = new DLineEdit;//文本框
    DWidget *main = new DWidget;
    QHBoxLayout *horboxlayout = new QHBoxLayout;
    DPushButton *icon = new DPushButton(main);
    QWidget *optionWidget = new QWidget;
    QFormLayout *optionLayout = new QFormLayout(optionWidget);
    DDialog *optinvaliddialog = new DDialog(optionWidget);
    //主目录相对路径
    QString relativeHomePath = QString("~") + QDir::separator();
    //保存上一次正确的文件夹路径，否则为默认文件夹
    static QString lastPicPath = nullptr;

    main->setLayout(horboxlayout);
    icon->setAutoDefault(false);
    icon->setObjectName(BUTTON_OPTION_PIC_LINE_EDIT);
    icon->setAccessibleName(BUTTON_OPTION_PIC_LINE_EDIT);
    picPathLineEdit->setFixedHeight(30);
    picPathLineEdit->setObjectName(OPTION_PIC_SELECTABLE_LINE_EDIT);
    picPathLineEdit->setAccessibleName(OPTION_PIC_SELECTABLE_LINE_EDIT);
    //获取当前设置的照片保存路径
    QString curPicSettingPath = option->value().toString();

    if (curPicSettingPath.contains(relativeHomePath))
        curPicSettingPath.replace(0, 1, QDir::homePath());

    QDir curPicSettingPathdir(curPicSettingPath);

    //路径不存在
    if (!curPicSettingPathdir.exists())
        picPathLineEdit->setText(option->defaultValue().toString());//更改文本框为默认路径~/Pictures/Camera
    else
        picPathLineEdit->setText(option->value().toString());//更改文本框为设置的路径

    QFontMetrics tem_fontmetrics = picPathLineEdit->fontMetrics();
    QString picStrElideText = ElideText(picPathLineEdit->text(), {285, tem_fontmetrics.height()}, QTextOption::WrapAnywhere,
                                        picPathLineEdit->font(), Qt::ElideMiddle, tem_fontmetrics.height(), 285);

    option->connect(picPathLineEdit, &DLineEdit::focusChanged, [ = ](bool on) {
        if (on) {
            QString settingPicPathValue = option->value().toString();

            if (settingPicPathValue.front() == '~')
                settingPicPathValue.replace(0, 1, QDir::homePath());

            QDir dir(settingPicPathValue);

            if (!dir.exists())
                picPathLineEdit->setText(option->defaultValue().toString());//更改文本框为默认路径~/Pictures/Camera
            else
                picPathLineEdit->setText(option->value().toString());//更改文本框为设置的路径
        }
    });

    picPathLineEdit->setText(picStrElideText);

    if (picStrElideText[0] == '~' && !picStrElideText.compare(option->defaultValue().toString())) {
        picStrElideText.replace(0, 1, QDir::homePath());
        lastPicPath = picStrElideText;
    } else if (picStrElideText[0] == '~' && picStrElideText.compare(option->defaultValue().toString())) {
        picStrElideText = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                          + QDir::separator() + QObject::tr("Camera");

        lastPicPath = picStrElideText;
    } else
        lastPicPath = picStrElideText;

    icon->setIcon(QIcon(":/images/icons/light/select-normal.svg"));
    icon->setIconSize(QSize(25, 25));
    icon->setFixedHeight(30);
    horboxlayout->addWidget(picPathLineEdit);
    horboxlayout->addWidget(icon);
    optionWidget->setObjectName(OPTION_FRAME);
    optionWidget->setAccessibleName(OPTION_FRAME);
    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);
    main->setMinimumWidth(240);
    optionLayout->addRow(new DLabel(QObject::tr(option->name().toStdString().c_str())), main);
    workaround_updateStyle(optionWidget, "light");
    optinvaliddialog->setObjectName(OPTION_INVALID_DIALOG);
    optinvaliddialog->setAccessibleName(OPTION_INVALID_DIALOG);
    optinvaliddialog->setIcon(QIcon(":/images/icons/warning.svg"));
    optinvaliddialog->setMessage(QObject::tr("You don't have permission to operate this folder"));
    optinvaliddialog->setWindowFlags(optinvaliddialog->windowFlags() | Qt::WindowStaysOnTopHint);
    optinvaliddialog->addButton(QObject::tr("Close"), true, DDialog::ButtonRecommend);

    /**
     *@brief 照片文本框路径是否正确
     *@param name:图片保存路径, alert:提示
     */
    auto validate = [ = ](QString name, bool alert = true) -> bool {
        Q_UNUSED(alert);
        name = name.trimmed();

        if (name.isEmpty())
            return false;

        if (name.size() > 2 && name.contains(relativeHomePath))
            name.replace(0, 1, QDir::homePath());

        QFileInfo fileinfo(name);
        QDir dir(name);

        if (fileinfo.exists())
        {
            QString picStr("/home/");
            if (!fileinfo.isDir())
                return false;

            if (picStr.contains(name))
                return false;

            if (!fileinfo.isReadable() || !fileinfo.isWritable())
                return false;

        } else
        {
            if (relativeHomePath.contains(name))
                return false;

            if (dir.cdUp()) {
                QFileInfo ch(dir.path());

                if (!ch.isReadable() || !ch.isWritable())
                    return false;
            }
            return false;
        }
        return true;
    };

    /**
     *@brief 打开照片文件夹按钮槽函数
     */
    option->connect(icon, &DPushButton::clicked, [ = ]() {

        QString selectPicSavePath;
        QString tmplastpicpath = lastPicPath;

        if (tmplastpicpath.contains(relativeHomePath))
            tmplastpicpath.replace(0, 1, QDir::homePath());

        QDir dir(tmplastpicpath);

        if (!dir.exists()) {
            //设置文本框为新的路径
            tmplastpicpath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                             + QDir::separator() + QObject::tr("Camera");
            QDir defaultdir(tmplastpicpath);

            if (!defaultdir.exists())
                dir.mkdir(tmplastpicpath);

            picPathLineEdit->setText(option->defaultValue().toString());
        }
#ifdef UNITTEST
        selectPicSavePath = "~/Pictures/";
#else
        //打开文件夹
        selectPicSavePath = DFileDialog::getExistingDirectory(nullptr, QObject::tr("Open folder"),
                                                              tmplastpicpath,
                                                              DFileDialog::ShowDirsOnly | DFileDialog::DontResolveSymlinks);
#endif

        if (validate(selectPicSavePath, false)) {
            option->setValue(selectPicSavePath);
            picPathLineEdit->setText(selectPicSavePath);
            tmplastpicpath = selectPicSavePath;
        }

        QFileInfo fm(selectPicSavePath);

        if ((!fm.isReadable() || !fm.isWritable()) && !selectPicSavePath.isEmpty())
            optinvaliddialog->show();
    });

    /**
     *@brief 照片文本框编辑完成槽函数
     */
    option->connect(picPathLineEdit, &DLineEdit::editingFinished, option, [ = ]() {

        QString curPicEditStr = picPathLineEdit->text();
        QDir dir(curPicEditStr);
        //获取当前文本框路径
        QString curPicLineEditPath = ElideText(curPicEditStr, {285, tem_fontmetrics.height()}, QTextOption::WrapAnywhere,
                                               picPathLineEdit->font(), Qt::ElideMiddle, tem_fontmetrics.height(), 285);

        if (!validate(picPathLineEdit->text(), false)) {
            QFileInfo fn(dir.path());

            if ((!fn.isReadable() || !fn.isWritable()) && !curPicEditStr.isEmpty())
                curPicLineEditPath = option->defaultValue().toString();
        }

        if (!picPathLineEdit->lineEdit()->hasFocus()) {

            //文本框路径有效
            if (validate(picPathLineEdit->text(), false)) {
                option->setValue(curPicLineEditPath);
                picPathLineEdit->setText(curPicLineEditPath);
                lastPicPath = curPicLineEditPath;
            }
            //路径编辑，但未修改
            else if (curPicLineEditPath == picStrElideText) {
                picPathLineEdit->setText(picStrElideText);
            }
            //文本框路径无效
            else {
                QString strDefaultPicPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                                            + QDir::separator() + QObject::tr("Camera");
                //设置为默认路径
                option->setValue(option->defaultValue().toString());
                picPathLineEdit->setText(option->defaultValue().toString());
            }
        }
    });

    option->connect(picPathLineEdit, &DLineEdit::textEdited, option, [ = ](const QString & newStr) {
        validate(newStr);
    });

    /**
     *@brief 照片后台设置参数更改槽函数
     */
    option->connect(option, &DTK_CORE_NAMESPACE::DSettingsOption::valueChanged, picPathLineEdit,
    [ = ](const QVariant & value) {
        auto pi = ElideText(value.toString(), {285, tem_fontmetrics.height()}, QTextOption::WrapAnywhere,
                            picPathLineEdit->font(), Qt::ElideMiddle, tem_fontmetrics.height(), 285);
        picPathLineEdit->setText(pi);
        lastPicPath = pi;
        option->setValue(pi);
        qDebug() << "save pic last path:" << pi << endl;
        picPathLineEdit->update();
    });

    return  optionWidget;
}



static QWidget *createVdSelectableLineEditOptionHandle(QObject *opt)
{
    DTK_CORE_NAMESPACE::DSettingsOption *option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(opt);
    option->setObjectName(VIDEO_OPTION);

    DLineEdit *videoPathLineEdit = new DLineEdit;
    DWidget *main = new DWidget;
    QHBoxLayout *horboxlayout = new QHBoxLayout;
    DPushButton *icon = new DPushButton(main);
    QWidget *optionWidget = new QWidget;
    QFormLayout *optionLayout = new QFormLayout(optionWidget);
    DDialog *optinvaliddialog = new DDialog(optionWidget);
    //主目录相对路径
    QString relativeHomePath = QString("~") + QDir::separator();
    static QString lastVideoPath = nullptr;

    main->setLayout(horboxlayout);
    icon->setAutoDefault(false);
    icon->setObjectName(BUTTON_OPTION_VIDEO_LINE_EDIT);
    icon->setAccessibleName(BUTTON_OPTION_VIDEO_LINE_EDIT);
    videoPathLineEdit->setFixedHeight(30);
    videoPathLineEdit->setObjectName(OPTION_VIDEO_SELECTABLE_LINE_EDIT);
    videoPathLineEdit->setAccessibleName(OPTION_VIDEO_SELECTABLE_LINE_EDIT);
    QString curVideoSettingPath = option->value().toString();
    if (curVideoSettingPath.contains(relativeHomePath))
        curVideoSettingPath.replace(0, 1, QDir::homePath());

    QDir curVideoSettingPathdir(curVideoSettingPath);

    //路径不存在
    if (!curVideoSettingPathdir.exists())
        videoPathLineEdit->setText(option->defaultValue().toString());//更改文本框为默认路径~/Videos/Camera
    else
        videoPathLineEdit->setText(option->value().toString());//更改文本框为设置的路径

    QFontMetrics tem_fontmetrics = videoPathLineEdit->fontMetrics();
    QString VideoStrElideText = ElideText(videoPathLineEdit->text(), {285, tem_fontmetrics.height()}, QTextOption::WrapAnywhere,
                                          videoPathLineEdit->font(), Qt::ElideMiddle, tem_fontmetrics.height(), 285);

    option->connect(videoPathLineEdit, &DLineEdit::focusChanged, [ = ](bool on) {
        if (on) {
            QString settingVideoPathValue = option->value().toString();

            if (settingVideoPathValue.front() == '~')
                settingVideoPathValue.replace(0, 1, QDir::homePath());

            QDir dir(settingVideoPathValue);

            if (!dir.exists())
                videoPathLineEdit->setText(option->defaultValue().toString());
            else
                videoPathLineEdit->setText(option->value().toString());
        }
    });

    videoPathLineEdit->setText(VideoStrElideText);

    if (VideoStrElideText[0] == '~' && !VideoStrElideText.compare(option->defaultValue().toString())) {
        VideoStrElideText.replace(0, 1, QDir::homePath());
        lastVideoPath = VideoStrElideText;
    } else if (VideoStrElideText[0] == '~' && VideoStrElideText.compare(option->defaultValue().toString())) {
        VideoStrElideText = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                            + QDir::separator() + QObject::tr("Camera");

        lastVideoPath = VideoStrElideText;
    } else
        lastVideoPath = VideoStrElideText;

    icon->setIcon(QIcon(":/images/icons/light/select-normal.svg"));
    icon->setIconSize(QSize(25, 25));
    icon->setFixedHeight(30);
    horboxlayout->addWidget(videoPathLineEdit);
    horboxlayout->addWidget(icon);
    optionWidget->setObjectName(OPTION_FRAME);
    optionWidget->setAccessibleName(OPTION_FRAME);
    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);
    main->setMinimumWidth(240);
    optionLayout->addRow(new DLabel(QObject::tr(option->name().toStdString().c_str())), main);
    workaround_updateStyle(optionWidget, "light");
    optinvaliddialog->setObjectName(OPTION_INVALID_DIALOG);
    optinvaliddialog->setAccessibleName(OPTION_INVALID_DIALOG);
    optinvaliddialog->setIcon(QIcon(":/images/icons/warning.svg"));
    optinvaliddialog->setMessage(QObject::tr("You don't have permission to operate this folder"));
    optinvaliddialog->setWindowFlags(optinvaliddialog->windowFlags() | Qt::WindowStaysOnTopHint);
    optinvaliddialog->addButton(QObject::tr("Close"), true, DDialog::ButtonRecommend);

    /**
     *@brief 视频文本框路径是否正确
     *@param name:视频保存路径, alert:提示
     */
    auto validate = [ = ](QString name, bool alert = true) -> bool {
        Q_UNUSED(alert);
        name = name.trimmed();

        if (name.isEmpty())
            return false;

        if (name.size() > 2 && name.contains(relativeHomePath))
            name.replace(0, 1, QDir::homePath());

        QFileInfo fileinfo(name);
        QDir dir(name);

        if (fileinfo.exists())
        {
            QString homeStr("/home/");
            if (!fileinfo.isDir())
                return false;

            if (homeStr.contains(name))
                return false;

            if (!fileinfo.isReadable() || !fileinfo.isWritable())
                return false;

        } else
        {
            if (relativeHomePath.contains(name))
                return false;

            if (dir.cdUp()) {
                QFileInfo ch(dir.path());

                if (!ch.isReadable() || !ch.isWritable())
                    return false;
            }
            return false;
        }
        return true;
    };

    /**
     *@brief 打开视频文件夹按钮槽函数
     */
    option->connect(icon, &DPushButton::clicked, [ = ]() {


        QString selectVideoSavePath;
        QDir dir(lastVideoPath);
        if (!dir.exists()) {
            //设置文本框为新的路径
            lastVideoPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                            + QDir::separator() + QObject::tr("Camera");
            QDir defaultdir(lastVideoPath);

            if (!defaultdir.exists()) {
                dir.mkdir(lastVideoPath);
            }
            videoPathLineEdit->setText(option->defaultValue().toString());
        }
#ifdef UNITTEST
        selectVideoSavePath = "~/Videos/";
#else
        //打开文件夹
        selectVideoSavePath = DFileDialog::getExistingDirectory(nullptr, QObject::tr("Open folder"),
                                                                lastVideoPath,
                                                                DFileDialog::ShowDirsOnly | DFileDialog::DontResolveSymlinks);
#endif

        if (validate(selectVideoSavePath, false)) {
            option->setValue(selectVideoSavePath);
            videoPathLineEdit->setText(selectVideoSavePath);
            lastVideoPath = selectVideoSavePath;
        }

        QFileInfo fm(selectVideoSavePath);

        if ((!fm.isReadable() || !fm.isWritable()) && !selectVideoSavePath.isEmpty())
            optinvaliddialog->show();

    });

    /**
     *@brief 视频文本框编辑完成槽函数
     */
    option->connect(videoPathLineEdit, &DLineEdit::editingFinished, option, [ = ]() {

        QString curVideoEditStr = videoPathLineEdit->text();
        QDir dir(curVideoEditStr);

        QString curVideoLineEditPath = ElideText(curVideoEditStr, {285, tem_fontmetrics.height()}, QTextOption::WrapAnywhere,
                                                 videoPathLineEdit->font(), Qt::ElideMiddle, tem_fontmetrics.height(), 285);

        if (!validate(videoPathLineEdit->text(), false)) {
            QFileInfo fn(dir.path());

            if ((!fn.isReadable() || !fn.isWritable()) && !curVideoEditStr.isEmpty())
                curVideoLineEditPath = option->defaultValue().toString();
        }

        if (!videoPathLineEdit->lineEdit()->hasFocus()) {

            //文本框路径有效
            if (validate(videoPathLineEdit->text(), false)) {
                option->setValue(videoPathLineEdit->text());
                videoPathLineEdit->setText(curVideoLineEditPath);
                lastVideoPath = curVideoEditStr;
            }
            //路径编辑，但未修改
            else if (curVideoLineEditPath == VideoStrElideText) {
                videoPathLineEdit->setText(VideoStrElideText);
            }
            //文本框路径无效
            else {
                QString strDefaultVdPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                                           + QDir::separator() + QObject::tr("Camera");
                //设置为默认路径
                option->setValue(option->defaultValue().toString());
                videoPathLineEdit->setText(option->defaultValue().toString());
            }
        }
    });

    option->connect(videoPathLineEdit, &DLineEdit::textEdited, option, [ = ](const QString & newStr) {
        validate(newStr);
    });

    /**
     *@brief 视频后台设置参数更改槽函数
     */
    option->connect(option, &DTK_CORE_NAMESPACE::DSettingsOption::valueChanged, videoPathLineEdit,
    [ = ](const QVariant & value) {
        auto pi = ElideText(value.toString(), {285, tem_fontmetrics.height()},
                            QTextOption::WrapAnywhere, videoPathLineEdit->font(), Qt::ElideMiddle,
                            tem_fontmetrics.height(), 285);

        videoPathLineEdit->setText(pi);
        lastVideoPath = pi;
        option->setValue(pi);

        videoPathLineEdit->update();
    });

    return  optionWidget;
}

QString CMainWindow::lastOpenedPath(QStandardPaths::StandardLocation standard)
{
    QString lastPath;
    if (standard == QStandardPaths::MoviesLocation) {
        lastPath = Settings::get().getOption("base.save.vddatapath").toString();
        if (lastPath.compare(Settings::get().settings()->option("base.save.vddatapath")->defaultValue().toString()) == 0)
            lastPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                       + QDir::separator() + QObject::tr("Camera");

    } else {
        lastPath = Settings::get().getOption("base.save.picdatapath").toString();
        if (lastPath.compare(Settings::get().settings()->option("base.save.picdatapath")->defaultValue().toString()) == 0)
            lastPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                       + QDir::separator() + QObject::tr("Camera");
    }

    QDir lastDir(lastPath);
    if (lastPath.isEmpty() || !lastDir.exists()) {
        if (standard == QStandardPaths::MoviesLocation) {
            lastPath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                       + QDir::separator() + QObject::tr("Camera");
        } else {
            lastPath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                       + QDir::separator() + QObject::tr("Camera");
        }

        QDir dirNew(lastPath);
        if (!dirNew.exists()) {
            if (!dirNew.mkdir(lastPath)) {
                qWarning() << "make dir error:" << lastPath;
                lastPath = QDir::currentPath();//这是错误的路径，但肯定存在
                qDebug() << "now lastVdPath is:" << lastPath;
            }
        }
    }

    if (lastPath.size() && lastPath[0] == '~') {
        QString str = QDir::homePath();
        lastPath.replace(0, 1, str);
    }
    return lastPath;
}

void CMainWindow::setWayland(bool bTrue)
{
    m_bWayland = bTrue;
}

CMainWindow::CMainWindow(QWidget *parent): DMainWindow(parent)
{
    m_SetDialog = nullptr;
    m_thumbnail = nullptr;
    m_videoPre = nullptr;
    m_pLoginManager = nullptr;
    m_pLoginMgrSleep = nullptr;
    m_pDBus = nullptr;
    m_bWayland = false;
    m_nActTpye = ActTakePic;
    m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();
    this->setObjectName(MAIN_WINDOW);
    this->setAccessibleName(MAIN_WINDOW);
    setupTitlebar();
}

CMainWindow::~CMainWindow()
{
    if (m_rightbtnmenu) {
        delete m_rightbtnmenu;
        m_rightbtnmenu = nullptr;
    }

    if (m_devnumMonitor) {
        while (m_devnumMonitor->isRunning()) {
            m_devnumMonitor->exit();
            m_devnumMonitor->deleteLater();
        }
        m_devnumMonitor = nullptr;
    }

    if (m_videoPre) {
        m_videoPre->deleteLater();
        m_videoPre = nullptr;
    }

    if (m_thumbnail) {
        m_thumbnail->deleteLater();
        m_thumbnail = nullptr;
    }

    qDebug() << "stop_encoder_thread";
}

void CMainWindow::slotPopupSettingsDialog()
{
    settingDialog();
    m_SetDialog->exec();
    settingDialogDel();
}

QString CMainWindow::libPath(const QString &strlib)
{
    QDir  dir;
    QString path  = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
    dir.setPath(path);
    QStringList list = dir.entryList(QStringList() << (strlib + "*"), QDir::NoDotAndDotDot | QDir::Files); //filter name with strlib

    if (list.contains(strlib))
        return strlib;

    list.sort();
    Q_ASSERT(list.size() > 0);
    return list.last();
}


void CMainWindow::initDynamicLibPath()
{
    LoadLibNames tmp;
    QByteArray avcodec = libPath("libavcodec.so").toLatin1();
    tmp.chAvcodec = avcodec.data();
    QByteArray avformat = libPath("libavformat.so").toLatin1();
    tmp.chAvformat = avformat.data();
    QByteArray avutil = libPath("libavutil.so").toLatin1();
    tmp.chAvutil = avutil.data();
    QByteArray udev = libPath("libudev.so").toLatin1();
    tmp.chUdev = udev.data();
    QByteArray usb = libPath("libusb-1.0.so").toLatin1();
    tmp.chUsb = usb.data();
    QByteArray portaudio = libPath("libportaudio.so").toLatin1();
    tmp.chPortaudio = portaudio.data();
    QByteArray v4l2 = libPath("libv4l2.so").toLatin1();
    tmp.chV4l2 = v4l2.data();
    QByteArray ffmpegthumbnailer = libPath("libffmpegthumbnailer.so").toLatin1();
    tmp.chFfmpegthumbnailer = ffmpegthumbnailer.data();
    QByteArray swscale = libPath("libswscale.so").toLatin1();
    tmp.chSwscale = swscale.data();
    QByteArray swresample = libPath("libswresample.so").toLatin1();
    tmp.chSwresample = swresample.data();
    setLibNames(tmp);
}

void CMainWindow::initBlockShutdown()
{
    if (!m_arg.isEmpty() || m_reply.value().isValid()) {
        qInfo() << "m_reply.value().isValid():" << m_reply.value().isValid();
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

    m_reply = m_pLoginManager->callWithArgumentList(QDBus::Block, "Inhibit", m_arg);

    if (m_reply.isValid())
        (void)m_reply.value().fileDescriptor();

    //如果for结束则表示没有发现未保存的tab项，则放开阻塞关机
    if (m_reply.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_reply;
        m_reply = QDBusReply<QDBusUnixFileDescriptor>();
        qDebug() << "init Nublock shutdown.";
    }
}

void CMainWindow::initBlockSleep()
{
    if (!m_argSleep.isEmpty() || m_replySleep.value().isValid()) {
        qInfo() << "m_reply.value().isValid():" << m_replySleep.value().isValid();
        return;
    }

    m_pLoginMgrSleep = new QDBusInterface("org.freedesktop.login1",
                                          "/org/freedesktop/login1",
                                          "org.freedesktop.login1.Manager",
                                          QDBusConnection::systemBus());

    m_argSleep << QString("sleep")             // what
               << qApp->productName()           // who
               << QObject::tr("File not saved")          // why
               << QString("block");                        // mode

    m_replySleep = m_pLoginMgrSleep->callWithArgumentList(QDBus::Block, "Inhibit", m_argSleep);

    if (m_replySleep.isValid())
        (void)m_replySleep.value().fileDescriptor();

    //如果for结束则表示没有发现未保存的tab项，则放开阻塞睡眠
    if (m_replySleep.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_replySleep;
        m_replySleep = QDBusReply<QDBusUnixFileDescriptor>();
        qDebug() << "init Nublock sleep.";
    }

}

void CMainWindow::initTabOrder()
{
    /*
     *主窗口tab循序切换
    */
    DWindowMinButton *windowMinBtn = titlebar()->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
    DWindowOptionButton *windowoptionButton = titlebar()->findChild<DWindowOptionButton *>("DTitlebarDWindowOptionButton");
    DWindowMaxButton *windowMaxBtn = titlebar()->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
    DWindowCloseButton *windowCloseBtn = titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    ThumbWidget *thumbLeftWidget = this->findChild<ThumbWidget *>("thumbLeftWidget");
    DPushButton *picVideoBtn = m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO);
    DPushButton *takeVideoEndBtn = m_thumbnail->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);

    setTabOrder(m_pSelectBtn, m_pTitlePicBtn);
    setTabOrder(m_pTitlePicBtn, m_pTitleVdBtn);
    setTabOrder(m_pTitleVdBtn, windowoptionButton);
    setTabOrder(windowoptionButton, windowMinBtn);
    setTabOrder(windowMinBtn, windowMaxBtn);
    setTabOrder(windowMaxBtn, windowCloseBtn);
    setTabOrder(windowCloseBtn, thumbLeftWidget);
    setTabOrder(thumbLeftWidget, picVideoBtn);
    setTabOrder(picVideoBtn, takeVideoEndBtn);


    titlebar()->setFocusPolicy(Qt::NoFocus);
}

void CMainWindow::initEventFilter()
{
    /**
     * @brief windowMinBtn 最小化按钮
     */
    DWindowMinButton *windowMinBtn = titlebar()->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
    if (windowMinBtn)
        windowMinBtn->installEventFilter(this);

    /**
     * @brief windowoptionButton 菜单栏按钮
     */
    DWindowOptionButton *windowoptionButton = titlebar()->findChild<DWindowOptionButton *>("DTitlebarDWindowOptionButton");
    if (windowoptionButton)
        windowoptionButton->installEventFilter(this);

    /**
     * @brief windowMaxBtn  最大化按钮
     */
    DWindowMaxButton *windowMaxBtn = titlebar()->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
    if (windowMaxBtn)
        windowMaxBtn->installEventFilter(this);

    /**
     * @brief windowCloseBtn 关闭按钮
     */
    DWindowCloseButton *windowCloseBtn = titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    if (windowCloseBtn)
        windowCloseBtn->installEventFilter(this);

    /**
     * @brief thumbLeftWidget 缩略图左边窗口
     */
    ThumbWidget *thumbLeftWidget = this->findChild<ThumbWidget *>("thumbLeftWidget");
    if (thumbLeftWidget)
        thumbLeftWidget->installEventFilter(this);

    /**
     * @brief picVideoBtn 拍照/录制按钮
     */
    DPushButton *picVideoBtn = m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO);
    if (picVideoBtn)
        picVideoBtn->installEventFilter(this);

    /**
     * @brief takeVideoEndBtn 结束按钮
     */
    DPushButton *takeVideoEndBtn = m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
    if (takeVideoEndBtn)
        takeVideoEndBtn->installEventFilter(this);

    if (m_videoPre)
        m_videoPre->installEventFilter(this);

    if (m_pSelectBtn)
        m_pSelectBtn->installEventFilter(this);

    if (m_pTitlePicBtn)
        m_pTitlePicBtn->installEventFilter(this);

    if (m_pTitleVdBtn)
        m_pTitleVdBtn->installEventFilter(this);

    if (titlebar())
        titlebar()->installEventFilter(this);

}

void CMainWindow::initShortcut()
{
    QShortcut *scViewShortcut = new QShortcut(QKeySequence("Ctrl+Shift+/"), this);
    QShortcut *scSpaceShortcut = new QShortcut(Qt::Key_Space, this);
    QShortcut *scEnterShortcut = new QShortcut(Qt::Key_Return, this);

    scViewShortcut->setObjectName(SHORTCUT_VIEW);
    scSpaceShortcut->setObjectName(SHORTCUT_SPACE);

    connect(scViewShortcut, &QShortcut::activated, this, [ = ] {
        qDebug() << "receive Ctrl+Shift+/";
        QRect rect = window()->geometry();
        QPoint pos(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
        Shortcut sc;
        QStringList shortcutString;
        QString param1 = "-j=" + sc.toStr();
        QString param2 = "-p=" + QString::number(pos.x()) + "," + QString::number(pos.y());

        shortcutString << "-b" << param1 << param2;
#ifndef UNITTEST
        QProcess::startDetached("deepin-shortcut-viewer", shortcutString);
#endif
    });

    connect(scSpaceShortcut, &QShortcut::activated, this, [ = ] {
        DPushButton *takevideo =  m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);

        if (QDateTime::currentMSecsSinceEpoch() < m_SpaceKeyInterval)
            //获取录制按钮第一次点击时间(ms)
            m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();

        //第二次点击时间与第一次时间差
        if (QDateTime::currentMSecsSinceEpoch() - m_SpaceKeyInterval > 300)
        {
            if (takevideo->isVisible()) {
                //重置当前时间
                m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();
                takevideo->click();
            } else {
                //重置当前时间
                m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();
                m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();

            }
        }
    });

    //根据tab焦点进行Enter键操作
    connect(scEnterShortcut, &QShortcut::activated, this, [ = ] {
        DWindowMinButton *windowMinBtn = titlebar()->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
        DWindowOptionButton *windowoptionButton = titlebar()->findChild<DWindowOptionButton *>("DTitlebarDWindowOptionButton");
        DWindowMaxButton *windowMaxBtn = titlebar()->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
        DWindowCloseButton *windowCloseBtn = titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
        QWidget *focuswidget = focusWidget();


        /**
         *@brief 判断当前的焦点窗口在哪个控件上，通过enter键触发对应的点击操作
         */

        if (m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END) == focuswidget
                || m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO) == focuswidget)
        {
            DPushButton *takevideo =  m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);

            //获取录制按钮第一次点击时间(ms)
            if (QDateTime::currentMSecsSinceEpoch() < m_SpaceKeyInterval)
                m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();

            //第二次点击时间与第一次时间差
            if (QDateTime::currentMSecsSinceEpoch() - m_SpaceKeyInterval > 300) {

                if (takevideo->isVisible()) {
                    //重置当前时间
                    m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();
                    takevideo->click();
                } else {
                    //重置当前时间
                    m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();
                    m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
                }
            }
        } else if (m_pTitlePicBtn == focuswidget)
            m_pTitlePicBtn->click();
        else if (m_pTitleVdBtn == focuswidget)
            m_pTitleVdBtn->click();
        else if (windowoptionButton == focuswidget)
            windowoptionButton->click();
        else if (windowMinBtn == focuswidget)
            windowMinBtn->click();
        else if (windowMaxBtn == focuswidget)
            windowMaxBtn->click();
        else if (windowCloseBtn == focuswidget)
            windowCloseBtn->click();
        else if (m_pSelectBtn == focuswidget)
            m_pSelectBtn->click();
        else
            return;
    });

}

void CMainWindow::settingDialog()
{
    m_SetDialog = new DSettingsDialog(this);
    m_SetDialog->widgetFactory()->registerWidget("selectableEditpic", createPicSelectableLineEditOptionHandle);
    m_SetDialog->widgetFactory()->registerWidget("selectableEditvd", createVdSelectableLineEditOptionHandle);
    m_SetDialog->widgetFactory()->registerWidget("formatLabel", createFormatLabelOptionHandle);
    m_SetDialog->setObjectName(SETTING_DIALOG);
    m_SetDialog->setAccessibleName(SETTING_DIALOG);

    connect(m_SetDialog, SIGNAL(destroyed()), this, SLOT(onSettingsDlgClose()));

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
        if (format_index >= 0 && resolu_index >= 0) {

            //format_index = -1 &&resolu_index = -1 表示设备被占用或者不存在
            for (int i = 0 ; i < list_stream_formats[format_index].numb_res; i++) {

                if ((list_stream_formats[format_index].list_stream_cap[i].width > 0
                        && list_stream_formats[format_index].list_stream_cap[i].height > 0) &&
                        (list_stream_formats[format_index].list_stream_cap[i].width < 7680
                         && list_stream_formats[format_index].list_stream_cap[i].height < 4320) &&
                        ((list_stream_formats[format_index].list_stream_cap[i].width % 8) == 0
                         && (list_stream_formats[format_index].list_stream_cap[i].height % 8) ==  0)) {
                    //加入分辨率的字符串
                    QString res_str = QString("%1x%2").arg(list_stream_formats[format_index].list_stream_cap[i].width).arg(list_stream_formats[format_index].list_stream_cap[i].height);

                    //去重
                    if (resolutionDatabase.contains(res_str) == false)
                        resolutionDatabase.append(res_str);
                }
            }
            int defres = 0;
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

                if (flag == 1)
                    continue;

            }

            for (int i = 0; i < resolutionDatabase.size(); i++) {
                QStringList resolutiontemp = resolutionDatabase[i].split("x");

                if ((v4l2core_get_frame_width(get_v4l2_device_handler()) == resolutiontemp[0].toInt()) &&
                        (v4l2core_get_frame_height(get_v4l2_device_handler()) == resolutiontemp[1].toInt())) {
                    defres = i; //设置分辨率下拉菜单当前索引
                    break;
                }

            }

            resolutionmodeFamily->setData("items", resolutionDatabase);
            //设置当前分辨率的索引
            resolutionDatabase.append(QString(tr("None")));
            Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), defres);
        } else {
            resolutionDatabase.clear();
            resolutionmodeFamily->setData("items", resolutionDatabase);
        }
    } else {
        //初始化分辨率字符串表
        QStringList resolutionDatabase = resolutionmodeFamily->data("items").toStringList();

        if (resolutionDatabase.size() > 0)
            resolutionmodeFamily->data("items").clear();

        resolutionDatabase.clear();
        resolutionDatabase.append(QString(tr("None")));
        Settings::get().settings()->setOption(QString("outsetting.resolutionsetting.resolution"), 0);
        resolutionmodeFamily->setData("items", resolutionDatabase);
    }
    m_SetDialog->setProperty("_d_QSSThemename", "dark");
    m_SetDialog->setProperty("_d_QSSFilename", "DSettingsDialog");
    m_SetDialog->updateSettings(Settings::get().settings());

    QPushButton *resetbtn = m_SetDialog->findChild<QPushButton *>("SettingsContentReset");
    resetbtn->setDefault(false);
    resetbtn->setAutoDefault(false);
}

void CMainWindow::settingDialogDel()
{
    if (m_SetDialog) {
        delete m_SetDialog;
        m_SetDialog = nullptr;
    }
}

void CMainWindow::loadAfterShow()
{
    initDynamicLibPath();
    //该方法导致键盘可用性降低，调试时无法使用、触摸屏无法唤起多次右键菜单，改用备用方案
    //this->grabKeyboard();//与方法：“QGuiApplication::keyboardModifiers() == Qt::ShiftModifier”具有同等效果
    initUI();
    initShortcut();
    gviewencoder_init();
    v4l2core_init();

    m_devnumMonitor = new DevNumMonitor();
    m_devnumMonitor->setParent(this);
    m_devnumMonitor->setObjectName("DevMonitorThread");
    m_devnumMonitor->start();
    initTitleBar();
    initConnection();
    //后续修改为标准Qt用法
    QString strCache = QString(getenv("HOME")) + QDir::separator() + QString(".cache")
                       + QDir::separator() + QString("deepin") + QDir::separator()
                       + QString("deepin-camera") + QDir::separator();
    QDir dir;
    dir.mkpath(strCache);
    initThumbnails();
    initThumbnailsConn();
    initTabOrder();
    initEventFilter();

    connect(m_devnumMonitor, SIGNAL(seltBtnStateEnable()), this, SLOT(setSelBtnShow()));//显示切换按钮
    connect(m_devnumMonitor, SIGNAL(seltBtnStateDisable()), this, SLOT(setSelBtnHide()));//多设备信号
    connect(m_devnumMonitor, SIGNAL(existDevice()), m_videoPre, SLOT(onRestartDevices()));//重启设备

    m_thumbnail->addPaths(lastVdFileName, lastPicFileName);
    m_videoPre->delayInit();
}

/**
 * @brief CMainWindow::recoverTabWidget
 * @param index
 *
 * 根据eventfilter函数设置的DataManager的m_tabindex序号对控件的tab框选效果做出恢复
 */
void CMainWindow::recoverTabWidget(uint index)
{
    DWindowMinButton *windowMinBtn = titlebar()->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
    DWindowOptionButton *windowoptionButton = titlebar()->findChild<DWindowOptionButton *>("DTitlebarDWindowOptionButton");
    DWindowMaxButton *windowMaxBtn = titlebar()->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
    DWindowCloseButton *windowCloseBtn = titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    switch (index) {
    case 0:
        if (findChild<videowidget *>())
            findChild<videowidget *>()->setFocus();
        break;
    case 1:
        if (m_pSelectBtn)
            m_pSelectBtn->setFocus();
        break;
    case 2:
        if (m_pTitlePicBtn)
            m_pTitlePicBtn->setFocus();
        break;
    case 3:
        if (m_pTitleVdBtn)
            m_pTitleVdBtn->setFocus();
        break;
    case 4:
        if (windowoptionButton)
            windowoptionButton->setFocus();
        break;
    case 5:
        if (windowMinBtn)
            windowMinBtn->setFocus();
        break;
    case 6:
        if (windowMaxBtn)
            windowMaxBtn->setFocus();
        break;
    case 7:
        if (windowCloseBtn)
            windowCloseBtn->setFocus();
        break;
    case 8:
        if (m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO))
            m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->setFocus();
        break;
    case 9:
        if (m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END))
            m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END)->setFocus();
        break;
    case 10:
        if (m_thumbnail->findChild<ThumbWidget *>("thumbLeftWidget"))
            m_thumbnail->findChild<ThumbWidget *>("thumbLeftWidget")->setFocus();
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
        qDebug() << "Nublock shutdown.";
    }

    if (m_bWayland) {
        initBlockSleep();

        if (bTrue) {
            m_replySleep = m_pLoginMgrSleep->callWithArgumentList(QDBus::Block, "Inhibit", m_argSleep);
        } else {
            QDBusReply<QDBusUnixFileDescriptor> tmp = m_replySleep;
            m_replySleep = QDBusReply<QDBusUnixFileDescriptor>();
            qDebug() << "Nublock sleep.";
        }
    }
}

void CMainWindow::onNoCam()
{
    onEnableTitleBar(3); //恢复按钮状态
    onEnableTitleBar(4); //恢复按钮状态
    onEnableSettings(true);

    if (m_thumbnail) {
        m_thumbnail->m_nStatus = STATNULL;
        m_thumbnail->show();
    }

}

void CMainWindow::stopCancelContinuousRecording(bool bTrue)
{
    if (bTrue) {
        if (m_thumbnail->m_nStatus == STATPicIng)
            m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
        else if (m_thumbnail->m_nStatus == STATVdIng) {
            QPushButton *btn = m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
            if (btn->isVisible())
                btn->click();
            else
                m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
        }
    }
}

void CMainWindow::onDirectoryChanged(const QString &)
{
    QDir dirChangePic(lastPicFileName);
    QDir dirChangeVd(lastVdFileName);
    bool bPic = dirChangePic.exists();
    bool bVd = dirChangeVd.exists();
    if (bPic && bVd) {
        return;
    } else {
        if (!bVd) {
            //录制状态下文件夹不存在需要停止录制
            DPushButton *tabkevdent =  m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
            if (tabkevdent->isVisible()) {
                tabkevdent->click();
            }
            lastVdFileName = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + QObject::tr("Camera");
            QDir dirDefaultVd(lastVdFileName);
            if (!dirDefaultVd.exists()) {
                bool bMakeDir = dirDefaultVd.mkdir(lastVdFileName);
                if (!bMakeDir) {
                    qWarning() << "make dir error:" << lastVdFileName;
                }
            }
            m_videoPre->setSaveVdFolder(lastVdFileName);
            Settings::get().settings()->setOption("base.save.vddatapath", lastVdFileName);
        }

        if (!bPic) {
            //连拍状态下文件夹不存在需要关掉连拍
            if (m_thumbnail->m_nStatus == STATPicIng)
                m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
            lastPicFileName = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator() + QObject::tr("Camera");
            QDir dirDefaultPic(lastPicFileName);
            if (!dirDefaultPic.exists()) {
                bool bMakeDir = dirDefaultPic.mkdir(lastPicFileName);
                if (!bMakeDir) {
                    qWarning() << "make dir error:" << lastPicFileName;
                }
            }
            m_videoPre->setSavePicFolder(lastPicFileName);
            Settings::get().settings()->setOption("base.save.picdatapath", lastPicFileName);
        }
        //更新文件夹监控
        QDir dirVd(lastVdFileName);
        dirVd.cdUp();
        QDir dirPic(lastPicFileName);
        dirPic.cdUp();
        QString strVd = dirVd.path();
        m_fileWatcher.removePaths(m_fileWatcher.directories());
        m_fileWatcherUp.removePaths(m_fileWatcherUp.directories());
        m_fileWatcher.addPath(lastVdFileName);
        m_fileWatcher.addPath(lastPicFileName);
        QString strPic = dirPic.path();
        m_fileWatcherUp.addPath(strVd);
        if (strVd.compare(strPic) != 0) {
            m_fileWatcherUp.addPath(strPic);
        }
        //用于刷新ui,当某个路径切换为默认路径，并且默认路径有文件时，可能会觉得奇怪（比如感觉没删掉），但实际是没问题的
        m_thumbnail->addPaths(lastPicFileName, lastVdFileName);
    }
}

void CMainWindow::onTimeoutLock(QString serviceName, QVariantMap key2value, QStringList)
{
    qDebug() << serviceName << key2value << endl;
    //仅wayland需要锁屏结束录制并停止使用摄像头，从锁屏恢复重新开启摄像头
    if (m_bWayland) {

        if (key2value.value("Locked").value<bool>()) {
            qDebug() << "locked";

            if (m_videoPre->getCapStatus())
                m_videoPre->onEndBtnClicked();

            //连拍状态下锁屏需要关掉连拍
            if (m_thumbnail->m_nStatus == STATPicIng)
                m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();

            m_videoPre->m_imgPrcThread->stop();
            qDebug() << "lock end";
        } else {
            qDebug() << "restart use camera cause ScreenBlack or PoweerLock";
            //打开摄像头
            m_videoPre->onChangeDev();
            qDebug() << "v4l2core_start_stream OK";
        }

    } else//锁屏结束连拍
        if (m_thumbnail->m_nStatus == STATPicIng && key2value.value("Locked").value<bool>())
            m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
    /*锁屏取消、结束录制*/
        else if (m_thumbnail->m_nStatus == STATVdIng && key2value.value("Locked").value<bool>()) {
            QPushButton *btn = m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
            if (btn->isVisible())
                btn->click();
            else
                m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
        }
}

void CMainWindow::initUI()
{
    m_videoPre = new videowidget(this);
    QPalette paletteTime = m_videoPre->palette();
    m_videoPre->setObjectName(VIDEO_PREVIEW_WIDGET);
    m_videoPre->setAccessibleName(VIDEO_PREVIEW_WIDGET);
    setCentralWidget(m_videoPre);
    paletteTime.setBrush(QPalette::Dark, QColor(/*"#202020"*/0, 0, 0, 51)); //深色
    m_videoPre->setPalette(paletteTime);

    lastVdFileName = lastOpenedPath(QStandardPaths::MoviesLocation);//如果路径不存在会自动使用并创建默认路径
    lastPicFileName = lastOpenedPath(QStandardPaths::PicturesLocation);

    m_fileWatcher.addPath(lastVdFileName);
    m_fileWatcher.addPath(lastPicFileName);

    QDir dirVd(lastVdFileName);
    dirVd.cdUp();
    m_fileWatcherUp.addPath(dirVd.path());

    QDir dirPic(lastPicFileName);
    dirPic.cdUp();
    if (!m_fileWatcherUp.directories().contains(dirPic.path())) {
        m_fileWatcherUp.addPath(dirPic.path());
    }

    m_videoPre->setSaveVdFolder(lastVdFileName);
    m_videoPre->setSavePicFolder(lastPicFileName);

    int nContinuous = Settings::get().getOption("photosetting.photosnumber.takephotos").toInt();
    int nDelayTime = Settings::get().getOption("photosetting.photosdelay.photodelays").toInt();
    bool soundphoto = Settings::get().getOption("photosetting.audiosetting.soundswitchbtn").toBool();

    switch (nContinuous) {//连拍次数
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

    switch (nDelayTime) {//延迟时间
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

    if (soundphoto)
        set_takeing_photo_sound(1);
    else
        set_takeing_photo_sound(0);

    m_videoPre->setInterval(nDelayTime);
    m_videoPre->setContinuous(nContinuous);
    resize(minWindowWidth, minWindowHeight);
}

void CMainWindow::initTitleBar()
{
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    pDButtonBox = new DButtonBox(this);
    QList<DButtonBoxButton *> listButtonBox;
    m_pTitlePicBtn = new DButtonBoxButton(QString(""), this);
    m_pTitleVdBtn = new DButtonBoxButton(QString(""), this);
    m_pSelectBtn = new DIconButton(this/*DStyle::SP_IndicatorSearch*/);

    pDButtonBox->setObjectName(BUTTOM_TITLE_BOX);
    pDButtonBox->setAccessibleName(BUTTOM_TITLE_BOX);
    pDButtonBox->setFixedWidth(120);
    pDButtonBox->setFixedHeight(36);
    //初始化标题栏拍照按钮
    QIcon iconPic(":/images/icons/light/button/photograph.svg");
    m_pTitlePicBtn->setObjectName(BUTTOM_TITLE_PICTURE);
    m_pTitlePicBtn->setAccessibleName(BUTTOM_TITLE_PICTURE);
    m_pTitlePicBtn->setIcon(iconPic);
    m_pTitlePicBtn->setIconSize(QSize(26, 26));
    DPalette pa = m_pTitlePicBtn->palette();
    QColor clo("#0081FF");
    pa.setColor(DPalette::Dark, clo);
    pa.setColor(DPalette::Light, clo);
    pa.setColor(DPalette::Button, clo);
    m_pTitlePicBtn->setPalette(pa);
    QIcon iconVd;

    //初始化主题判断
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType)
        iconVd = QIcon(":/images/icons/light/record video.svg");
    else
        iconVd = QIcon(":/images/icons/dark/button/record video_dark.svg");

    //初始化标题栏录像按钮
    m_pTitleVdBtn->setObjectName(BUTTOM_TITLE_VEDIO);
    m_pTitleVdBtn->setAccessibleName(BUTTOM_TITLE_VEDIO);
    m_pTitleVdBtn->setIcon(iconVd);
    m_pTitleVdBtn->setIconSize(QSize(26, 26));
    listButtonBox.append(m_pTitlePicBtn);
    listButtonBox.append(m_pTitleVdBtn);
    pDButtonBox->setButtonList(listButtonBox, false);
    titlebar()->addWidget(pDButtonBox);

    //初始化切换按钮
    m_pSelectBtn->setObjectName(BUTTOM_TITLE_SELECT);
    m_pSelectBtn->setAccessibleName(BUTTOM_TITLE_SELECT);
    m_pSelectBtn->setFixedSize(QSize(37, 37));
    m_pSelectBtn->setIconSize(QSize(37, 37));
    m_pSelectBtn->hide();
    m_pSelectBtn->setFocusPolicy(Qt::TabFocus);
    m_pSelectBtn->installEventFilter(this);

    //初始化主题判断
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        if (CamApp->isPanelEnvironment())
            m_pSelectBtn->setIcon(QIcon(":/table/Icons/light/Switch camera_dark.svg"));
        else
            m_pSelectBtn->setIcon(QIcon(":/images/icons/light/button/Switch camera.svg"));
    } else {
        if (CamApp->isPanelEnvironment())
            m_pSelectBtn->setIcon(QIcon(":/table/Icons/dark/Switch camera.svg"));
        else
            m_pSelectBtn->setIcon(QIcon(":/images/icons/dark/button/Switch camera_dark.svg"));
    }

    titlebar()->setIcon(QIcon::fromTheme("deepin-camera"));
    titlebar()->addWidget(m_pSelectBtn, Qt::AlignLeft);
}

void CMainWindow::initConnection()
{
    connect(CamApp, &CApplication::popupConfirmDialog, this, [ = ] {
        if (m_videoPre->getCapStatus())
        {
            CloseDialog closeDlg(this, tr("Video recording is in progress. Close the window?"));
#ifdef UNITTEST
            closeDlg.show();
#else
            int ret = closeDlg.exec();
#endif
#ifdef UNITTEST
            closeDlg.close();
#else
            if (ret == 1)
                qApp->quit();
#endif
        } else
            qApp->quit();

    });

    //设置按钮信号
    connect(m_actionSettings, &QAction::triggered, this, &CMainWindow::slotPopupSettingsDialog);
    //切换分辨率
    connect(&Settings::get(), SIGNAL(resolutionchanged(const QString &)), m_videoPre, SLOT(slotresolutionchanged(const QString &)));
    //拍照
    connect(m_videoPre, SIGNAL(takePicOnce()), this, SLOT(onTakePicOnce()));
    //拍照取消
    connect(m_videoPre, SIGNAL(takePicCancel()), this, SLOT(onTakePicCancel()));
    //拍照结束
    connect(m_videoPre, SIGNAL(takePicDone()), this, SLOT(onTakePicDone()));
    //录制结束
    connect(m_videoPre, SIGNAL(takeVdDone()), this, SLOT(onTakeVdDone()));
    //录制取消 （倒计时3秒内的）
    connect(m_videoPre, SIGNAL(takeVdCancel()), this, SLOT(onTakeVdCancel()));
    //录制关机/休眠阻塞
    connect(m_videoPre, SIGNAL(updateBlockSystem(bool)), this, SLOT(updateBlockSystem(bool)));
    //没有相机了，结束拍照、录制
    connect(m_videoPre, SIGNAL(noCam()), this, SLOT(onNoCam()));
    //相机被抢占了，结束拍照、录制
    connect(m_videoPre, SIGNAL(noCamAvailable()), this, SLOT(onNoCam()));
    //设备切换信号
    connect(m_pSelectBtn, SIGNAL(clicked()), m_videoPre, SLOT(onChangeDev()));
    //设置新的分辨率
    connect(m_videoPre, SIGNAL(sigDeviceChange()), &Settings::get(), SLOT(setNewResolutionList()));
    //标题栏图片按钮
    connect(m_pTitlePicBtn, SIGNAL(clicked()), this, SLOT(onTitlePicBtn()));
    //标题栏视频按钮
    connect(m_pTitleVdBtn, SIGNAL(clicked()), this, SLOT(onTitleVdBtn()));
    //主题变换
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &CMainWindow::onThemeChange);

    //连拍在锁屏、熄屏情况下都要结束，与平台无关
    QDBusConnection::sessionBus().connect("com.deepin.SessionManager", "/com/deepin/SessionManager",
                                          "org.freedesktop.DBus.Properties", "PropertiesChanged", this,
                                          SLOT(onTimeoutLock(QString, QVariantMap, QStringList)));


    //控制中心更改了本地时间，及时更新当前时间
    //避免时间往前切换过后，需要点击两次才能结束录制
    QDBusConnection::sessionBus().connect("com.deepin.daemon.Timedate", "/com/deepin/daemon/Timedate",
                                          "com.deepin.daemon.Timedate", "TimeUpdate", this,
                                          SLOT(onLocalTimeChanged()));

    QDBusConnection::systemBus().connect("org.freedesktop.login1", "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager", "PrepareForSleep", this, SLOT(stopCancelContinuousRecording(bool)));

    QDBusConnection::systemBus().connect("org.freedesktop.login1", "/org/freedesktop/login1",
                                         "org.freedesktop.login1.Manager", "PrepareForShutdown", this, SLOT(stopCancelContinuousRecording(bool)));
}

void CMainWindow::initThumbnails()
{
    m_thumbnail = new ThumbnailsBar;
    m_thumbnail->setParent(this);
    m_rightbtnmenu = new QMenu(this);//添加右键打开文件夹功能
    m_actOpenfolder = new QAction(this);

    m_thumbnail->setObjectName(THUMBNAIL);
    m_thumbnail->setAccessibleName(THUMBNAIL);
    m_thumbnail->move(0, height() - 10);

    m_thumbnail->setFixedHeight(LAST_BUTTON_HEIGHT + LAST_BUTTON_SPACE * 3);

    m_videoPre->setThumbnail(m_thumbnail);
    m_rightbtnmenu->setObjectName(BUTTON_RIGHT_MENU);
    m_rightbtnmenu->setAccessibleName(BUTTON_RIGHT_MENU);
    m_actOpenfolder->setText(tr("Open folder"));
    m_rightbtnmenu->addAction(m_actOpenfolder);
    m_thumbnail->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(m_thumbnail, &DLabel::customContextMenuRequested, this, [ = ](QPoint pos) {
        Q_UNUSED(pos);
        m_rightbtnmenu->exec(QCursor::pos());
    });

    //右键菜单打开文件
    connect(m_actOpenfolder, &QAction::triggered, this, [ = ] {

        QString currentDefaultSavePath("");
        QString currentsavePath("");

        if (m_nActTpye == ActTakePic)
        {
            QString currentDefaultPicSavePath = Settings::get().settings()->option("base.save.picdatapath")->defaultValue().toString();
            QString picSavePath = Settings::get().getOption("base.save.picdatapath").toString();
            if (picSavePath.compare(currentDefaultPicSavePath) == 0) {
                currentsavePath = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)
                + QDir::separator() + QObject::tr("Camera");
            } else
                currentsavePath = picSavePath;

        }
        if (m_nActTpye == ActTakeVideo)
        {
            QString currentDefaultSaveVideoPath = Settings::get().settings()->option("base.save.vddatapath")->defaultValue().toString();
            QString videoSavePath = Settings::get().getOption("base.save.vddatapath").toString();
            if (videoSavePath.compare(currentDefaultSaveVideoPath) == 0) {
                currentsavePath = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)
                                  + QDir::separator() + QObject::tr("Camera");
            } else
                currentsavePath = videoSavePath;
        }

        if (!QFileInfo(currentsavePath).exists())
        {
            QDir d;
            d.mkpath(currentsavePath);
        }

        Dtk::Widget::DDesktopServices::showFolder(currentsavePath);
    });

    m_thumbnail->setVisible(true);
    m_thumbnail->show();
    m_thumbnail->m_nMaxWidth = minWindowWidth;
}

void CMainWindow::initThumbnailsConn()
{
    //系统文件夹变化信号
    connect(&m_fileWatcher, SIGNAL(directoryChanged(const QString &)), m_thumbnail, SLOT(onFoldersChanged(const QString &)));
    //系统文件变化信号
    connect(&m_fileWatcher, SIGNAL(fileChanged(const QString &)), m_thumbnail, SLOT(onFoldersChanged(const QString &)));
    //系统文件夹变化信号
    connect(&m_fileWatcherUp, SIGNAL(directoryChanged(const QString &)), this, SLOT(onDirectoryChanged(const QString &)));
    //系统文件夹变化信号
    connect(&m_fileWatcherUp, SIGNAL(fileChanged(const QString &)), this, SLOT(onDirectoryChanged(const QString &)));
    //增删文件修改界面
    connect(m_thumbnail, SIGNAL(fitToolBar()), this, SLOT(onFitToolBar()));
    //修改标题栏按钮状态
    connect(m_thumbnail, SIGNAL(enableTitleBar(int)), this, SLOT(onEnableTitleBar(int)));
    //录像信号
    connect(m_thumbnail, SIGNAL(takeVd()), m_videoPre, SLOT(onTakeVideo()));
    //禁用设置
    connect(m_thumbnail, SIGNAL(enableSettings(bool)), this, SLOT(onEnableSettings(bool)));
    //拍照信号--显示倒计时
    connect(m_thumbnail, SIGNAL(takePic(bool)), m_videoPre, SLOT(onTakePic(bool)));
    //传递文件名，在拍照录制开始的时候，创建的文件不用于更新缩略图
    connect(m_videoPre, SIGNAL(filename(QString)), m_thumbnail, SLOT(onFileName(QString)));
}

void CMainWindow::setSelBtnHide()
{
    m_pSelectBtn->hide();
    qDebug() << "Hide camera selection button";
}

void CMainWindow::onLocalTimeChanged()
{
    m_SpaceKeyInterval = QDateTime::currentMSecsSinceEpoch();
}

void CMainWindow::setSelBtnShow()
{
    m_pSelectBtn->show();
    qDebug() << "Show camera selection button";
}

void CMainWindow::setupTitlebar()
{
    m_titlemenu = new DMenu(this);
    m_actionSettings = new QAction(tr("Settings"), this);

    titlebar()->setObjectName(TITLEBAR);
    titlebar()->setAccessibleName(TITLEBAR);
    m_titlemenu->setObjectName(TITLE_MUNE);
    m_titlemenu->setAccessibleName(TITLE_MUNE);
    m_actionSettings->setObjectName("SettingAction");
    m_titlemenu->addAction(m_actionSettings);
    titlebar()->setMenu(m_titlemenu);
    titlebar()->setParent(this);
}

void CMainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    int width = this->width();

    if (m_thumbnail) {
        m_thumbnail->m_nMaxWidth = width;
        m_thumbnail->widthChanged();
        onFitToolBar();
    }

    if (m_videoPre)
        m_videoPre->update();

}

void CMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_videoPre->getCapStatus()) {
        CloseDialog closeDlg(this, tr("Video recording is in progress. Close the window?"));
        closeDlg.setObjectName(CLOSE_DIALOG);
        closeDlg.setAccessibleName(CLOSE_DIALOG);
#ifdef UNITTEST
        closeDlg.show();
        event->ignore();
        closeDlg.close();
#else
        int ret = closeDlg.exec();

        switch (ret) {
        case 0:
            event->ignore();
            break;
        case 1:
            m_videoPre->onEndBtnClicked();
            event->accept();
            break;
        default:
            event->ignore();
            break;
        }
#endif
    }
}

void CMainWindow::changeEvent(QEvent *event)
{
    Q_UNUSED(event);

    if (windowState() == Qt::WindowMinimized || (windowState() == (Qt::WindowMinimized | Qt::WindowMaximized))) {
        if (m_thumbnail->m_nStatus == STATPicIng)
            m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
        /*锁屏取消、结束录制*/
        else if (m_thumbnail->m_nStatus == STATVdIng) {
            QPushButton *btn = m_videoPre->findChild<DPushButton *>(BUTTON_TAKE_VIDEO_END);
            if (btn->isVisible())
                btn->click();
            else
                m_thumbnail->findChild<DPushButton *>(BUTTON_PICTURE_VIDEO)->click();
        }
    }
}

void CMainWindow::onFitToolBar()
{
    if (m_thumbnail) {
        int n = m_thumbnail->m_hBox->count();
        int nWidth = 0;

        if (n <= 0) {
            nWidth = LAST_BUTTON_SPACE + LAST_BUTTON_WIDTH;
            m_thumbnail->m_showVdTime->hide();
            m_thumbnail->m_thumbLeftWidget->hide();
        } else {
            m_thumbnail->m_thumbLeftWidget->show();
            if (DataManager::instance()->getvideoCount() <= 0) {
                m_thumbnail->m_showVdTime->hide();
                nWidth = n * THUMBNAIL_WIDTH + LAST_BUTTON_SPACE * 4 + LAST_BUTTON_WIDTH;
            } else {
                m_thumbnail->m_showVdTime->show();
                nWidth = n * THUMBNAIL_WIDTH + LAST_BUTTON_SPACE * 5 + LAST_BUTTON_WIDTH + VIDEO_TIME_WIDTH;
            }

            if (DataManager::instance()->m_setIndex.size() >= 1)
                nWidth += DataManager::instance()->m_setIndex.size() * (SELECTED_WIDTH - THUMBNAIL_WIDTH);
            else
                nWidth += SELECTED_WIDTH - THUMBNAIL_WIDTH;

        }

        qDebug() << "onFitToolBar" << nWidth;
        int tmpheight = m_thumbnail->height();
        m_thumbnail->resize(nWidth, THUMBNAIL_HEIGHT + 50);
        int mtmpheight = m_thumbnail->height();
        m_thumbnail->m_hBox->setSpacing(0);
        m_thumbnail->m_hBox->setMargin(0);
        m_thumbnail->move((width() - m_thumbnail->width()) / 2,
                          height() - m_thumbnail->height() - 5);
    }

}

void CMainWindow::onEnableTitleBar(int nType)
{
    /**
     * 获取延时时间的索引
     * 0:不需要延迟拍照
     * 1:延迟3秒
     * 2:延迟6秒
     */
    int delaytime = Settings::get().getOption("photosetting.photosdelay.photodelays").toInt();
    //1、禁用标题栏视频；2、禁用标题栏拍照；3、恢复标题栏视频；4、恢复标题栏拍照
    switch (nType) {
    case 1:
        if (!(DataManager::instance()->getNowTabIndex() > 3
                && DataManager::instance()->getNowTabIndex() < 8)
                && !(DataManager::instance()->getNowTabIndex() == 2)
                && (delaytime == 0)) {
            /*焦点不在菜单、最小化、最大化、关闭和标题栏拍照按钮并且“延迟”索引等于0。
             * 立即将焦点移到主窗口，避免控件disable状态，焦点自动位移。
             */
            setFocus();
        }

        /*
         * 延迟索引大于0,并焦点在标题栏录制或摄像头切换按钮
         */
        if ((delaytime > 0) && (focusWidget() == m_pTitleVdBtn || focusWidget() == m_pSelectBtn))
            setFocus();

        m_pTitleVdBtn->setEnabled(false);
        m_pSelectBtn->setEnabled(false);
        break;
    case 2:
        if (!(DataManager::instance()->getNowTabIndex() > 3
                && DataManager::instance()->getNowTabIndex() < 8)
                && !(DataManager::instance()->getNowTabIndex() == 3)
                && (delaytime == 0)) {
            /*焦点不在菜单、最小化、最大化、关闭和标题栏拍照按钮并且“延迟”索引等于0。
             * 立即将焦点移到主窗口，避免控件disable状态，焦点自动位移。
             */
            setFocus();
        }

        if ((delaytime > 0) && (focusWidget() == m_pTitlePicBtn || focusWidget() == m_pSelectBtn))
            setFocus();

        m_pTitlePicBtn->setEnabled(false);
        m_pSelectBtn->setEnabled(false);
        break;
    case 3:
        m_pTitleVdBtn->setEnabled(true);
        m_pSelectBtn->setEnabled(true);
        break;
    case 4:
        m_pTitlePicBtn->setEnabled(true);
        m_pSelectBtn->setEnabled(true);
        break;
    default:
        break;
    }
}

void CMainWindow::onTitlePicBtn()
{
    if (m_nActTpye == ActTakePic)
        return;

    m_nActTpye = ActTakePic;
    qDebug() << "Switch to take picture state!";
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

    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::UnknownType
            || DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        QIcon iconVd(":/images/icons/light/record video.svg");
        m_pTitleVdBtn->setIcon(iconVd);
    } else {
        QIcon iconVd(":/images/icons/dark/button/record video_dark.svg");
        m_pTitleVdBtn->setIcon(iconVd);
    }

    m_thumbnail->ChangeActType(m_nActTpye);
    SettingPathsave();
}

void CMainWindow::onTitleVdBtn()
{
    if (m_nActTpye == ActTakeVideo)
        return;

    m_nActTpye = ActTakeVideo;
    qDebug() << "Switch to take video state!";
    //切换标题栏视频按钮颜色
    DPalette paPic = m_pTitleVdBtn->palette();
    QColor cloPic("#0081FF");
    paPic.setColor(DPalette::Dark, cloPic);
    paPic.setColor(DPalette::Light, cloPic);
    paPic.setColor(DPalette::Button, cloPic);
    m_pTitleVdBtn->setPalette(paPic);
    QIcon defaultIconVd(":/images/icons/light/button/transcribe.svg");
    m_pTitleVdBtn->setIcon(defaultIconVd);
    //切换标题栏拍照按钮颜色
    DPalette paVd = m_pTitlePicBtn->palette();
    QColor cloVd("#000000");
    cloVd.setAlpha(20);
    paVd.setColor(DPalette::Dark, cloVd);
    paVd.setColor(DPalette::Light, cloVd);
    paVd.setColor(DPalette::Button, cloVd);
    m_pTitlePicBtn->setPalette(paVd);

    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::UnknownType
            || DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        QIcon iconVd(":/images/icons/light/photograph.svg");
        m_pTitlePicBtn->setIcon(iconVd);
    } else {
        QIcon iconPic(":/images/icons/dark/button/photograph_dark.svg");
        m_pTitlePicBtn->setIcon(iconPic);
    }

    m_thumbnail->ChangeActType(m_nActTpye);
    SettingPathsave();
}

void CMainWindow::onSettingsDlgClose()
{
    /**********************************************/
    //先获取路径，再对路径进行修正
    lastVdFileName = Settings::get().getOption("base.save.vddatapath").toString();
    lastPicFileName = Settings::get().getOption("base.save.picdatapath").toString();

    if (lastPicFileName.compare(Settings::get().settings()->option("base.save.picdatapath")->defaultValue().toString()) == 0)
        lastPicFileName = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator() + QObject::tr("Camera");

    if (lastVdFileName.compare(Settings::get().settings()->option("base.save.vddatapath")->defaultValue().toString()) == 0)
        lastVdFileName = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + QObject::tr("Camera");


    //由于U盘、可移动磁盘的加入，如果路径不存在，可能是U盘被拔掉了，因此此时不能创建，而是恢复默认路径
    if (QDir(lastVdFileName).exists() == false)
        lastVdFileName = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + QDir::separator() + QObject::tr("Camera");

    if (QDir(lastPicFileName).exists() == false)
        lastPicFileName = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QDir::separator() + QObject::tr("Camera");

    /***next，发现这个路径已经不存在了，需要设置后台路径为默认，其他地方方法同***/

    if (lastVdFileName.size() && lastVdFileName[0] == '~')
        lastVdFileName.replace(0, 1, QDir::homePath());

    if (lastPicFileName.size() && lastPicFileName[0] == '~')
        lastPicFileName.replace(0, 1, QDir::homePath());

    m_videoPre->setSavePicFolder(lastPicFileName);
    m_videoPre->setSaveVdFolder(lastVdFileName);

    //关闭设置时，先删除原有的文件监控（需求上不再需要），再添加保存路径下图片和视频的缩略图
    bool bContainVd = m_fileWatcher.directories().contains(lastVdFileName);
    bool bContainPic = m_fileWatcher.directories().contains(lastPicFileName);
    QDir dirVd(lastVdFileName);
    dirVd.cdUp();
    QDir dirPic(lastPicFileName);
    dirPic.cdUp();
    QString strVd = dirVd.path();
    if (!bContainVd || !bContainPic) {
        m_fileWatcher.removePaths(m_fileWatcher.directories());
        m_fileWatcherUp.removePaths(m_fileWatcherUp.directories());
        m_fileWatcher.addPath(lastVdFileName);
        m_fileWatcher.addPath(lastPicFileName);

        QString strPic = dirPic.path();
        m_fileWatcherUp.addPath(strVd);
        if (strVd.compare(strPic) != 0) {
            m_fileWatcherUp.addPath(strPic);
        }
        m_thumbnail->addPaths(lastVdFileName, lastPicFileName);
    } else {
        if (QString::compare(lastVdFileName, lastPicFileName) == 0) {
            m_fileWatcher.removePaths(m_fileWatcher.directories());
            m_fileWatcherUp.removePaths(m_fileWatcherUp.directories());
            m_fileWatcher.addPath(lastVdFileName);
            m_fileWatcherUp.addPath(strVd);
            m_thumbnail->addPaths(lastVdFileName, lastPicFileName);
        }
    }

    int nContinuous = Settings::get().getOption("photosetting.photosnumber.takephotos").toInt();
    int nDelayTime = Settings::get().getOption("photosetting.photosdelay.photodelays").toInt();
    bool soundphoto = Settings::get().getOption("photosetting.audiosetting.soundswitchbtn").toBool();

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

    if (soundphoto)
        set_takeing_photo_sound(1);
    else
        set_takeing_photo_sound(0);

    m_videoPre->setInterval(nDelayTime);
    m_videoPre->setContinuous(nContinuous);
    Settings::get().settings()->sync();
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
    //恢复控件焦点状态
    recoverTabWidget(DataManager::instance()->getNowTabIndex());
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->setBtntooltip();
    qDebug() << "Cancel taking photo!";
}

void CMainWindow::onTakeVdDone()
{
    onEnableTitleBar(4); //恢复按钮状态
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->show();
    //恢复控件焦点状态
    recoverTabWidget(DataManager::instance()->getNowTabIndex());
    onEnableSettings(true);

    QTimer::singleShot(200, this, [ = ] {
        QString strFileName = lastVdFileName + QDir::separator() + DataManager::instance()->getstrFileName();
        QFile file(strFileName);

        if (!file.exists())
            usleep(200000);

        m_thumbnail->addFile(strFileName);
    });
    qDebug() << "Taking video completed!";

}

void CMainWindow::onTakeVdCancel()   //保存视频完成，通过已有的文件检测实现缩略图恢复，这里不需要额外处理
{
    onEnableTitleBar(4); //恢复按钮状态
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->show();
    onEnableSettings(true);
    recoverTabWidget(DataManager::instance()->getNowTabIndex());
    qDebug() << "Cancel taking video!";
}

void CMainWindow::onThemeChange(DGuiApplicationHelper::ColorType type)
{
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        m_pSelectBtn->setIconSize(QSize(37, 37));

        if (CamApp->isPanelEnvironment())
            m_pSelectBtn->setIcon(QIcon(":/table/Icons/light/Switch camera_dark.svg"));
        else
            m_pSelectBtn->setIcon(QIcon(":/images/icons/light/button/Switch camera.svg"));

        if (m_nActTpye == ActTakePic)
            m_pTitleVdBtn->setIcon(QIcon(":/images/icons/light/record video.svg"));
        else
            m_pTitlePicBtn->setIcon(QIcon(":/images/icons/light/photograph.svg"));
    }

    if (type == DGuiApplicationHelper::DarkType) {

        if (CamApp->isPanelEnvironment())
            m_pSelectBtn->setIcon(QIcon(":/table/Icons/dark/Switch camera.svg"));
        else
            m_pSelectBtn->setIcon(QIcon(":/images/icons/dark/button/Switch camera_dark.svg"));

        if (m_nActTpye == ActTakePic)
            m_pTitleVdBtn->setIcon(QIcon(":/images/icons/dark/button/record video_dark.svg"));
        else
            m_pTitlePicBtn->setIcon(QIcon(":/images/icons/dark/button/photograph_dark.svg"));
    }
}

void CMainWindow::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) {
        qDebug() << "shift pressed";
        DataManager::instance()->setShiftMulti(true);
        int nIndex = DataManager::instance()->getindexNow();
        //按下shift键就要更新第一个索引值
        if (-1 == DataManager::instance()->getLastIndex()) {
            DataManager::instance()->setLastIndex(nIndex);
        }
    }
    if (e->key() == Qt::Key_Control) {
        qDebug() << "ctrl pressed";
        DataManager::instance()->setCtrlMulti(true);
        DataManager::instance()->m_setIndex.insert(DataManager::instance()->getindexNow());
    }
}

void CMainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) {
        qDebug() << "shift released";
        DataManager::instance()->setShiftMulti(false);
        DataManager::instance()->setLastIndex(-1);
    }
    if (e->key() == Qt::Key_Control) {
        qDebug() << "ctrl pressed";
        DataManager::instance()->setCtrlMulti(false);
    }
}



bool CMainWindow::eventFilter(QObject *obj, QEvent *e)
{
    /**
     * @brief 关注焦点进入事件和鼠标点击事件。
     * 焦点进入：设置对应控件索引序号。
     * 鼠标点击：焦点移入预览界面，并将下标设置为0
     */

    DWindowMinButton *windowMinBtn = titlebar()->findChild<DWindowMinButton *>("DTitlebarDWindowMinButton");
    DWindowOptionButton *windowoptionButton = titlebar()->findChild<DWindowOptionButton *>("DTitlebarDWindowOptionButton");
    DWindowMaxButton *windowMaxBtn = titlebar()->findChild<DWindowMaxButton *>("DTitlebarDWindowMaxButton");
    DWindowCloseButton *windowCloseBtn = titlebar()->findChild<DWindowCloseButton *>("DTitlebarDWindowCloseButton");
    DPushButton *picvideobtn = findChild<DPushButton *>("PicVdBtn");
    DPushButton *endbtn = findChild<DPushButton *>("TakeVdEndBtn");
    ThumbWidget *thumbwidget = findChild<ThumbWidget *>("thumbLeftWidget");

    if ((obj == m_pSelectBtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 1;
    } else if ((obj == m_pTitlePicBtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 2;
    } else if ((obj == m_pTitleVdBtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 3;
    } else if ((obj == windowoptionButton) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 4;
    } else if ((obj == windowMinBtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 5;
    } else if ((obj == windowMaxBtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 6;
    } else if ((obj == windowCloseBtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 7;
    } else if ((obj == picvideobtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 8;
    } else if ((obj == endbtn) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 9;
    } else if ((obj == thumbwidget) && (e->type() == QEvent::FocusIn)) {
        DataManager::instance()->m_tabIndex = 10;
    } else if (e->type() == QEvent::MouseButtonPress) {
        DataManager::instance()->m_tabIndex = 0;
        m_videoPre->setFocus();
    } else {
        QWidget::eventFilter(obj, e);//调用父类事件过滤器
    }
    return QWidget::eventFilter(obj, e);
}

void CMainWindow::SettingPathsave()
{
    m_videoPre->setSavePicFolder(lastPicFileName);
}
