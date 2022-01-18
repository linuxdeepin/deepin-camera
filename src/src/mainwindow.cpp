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

//videowidget
#define BUTTON_TAKE_VIDEO_END QObject::tr("TakeVdEndBtn")

#define BUTTON_PICTURE_VIDEO QObject::tr("PicVdBtn")

using namespace dc;
extern bool g_bMultiSlt; //是否多选
extern QSet<int> g_setIndex;
extern int g_indexNow;
extern QString g_strFileName;
extern int g_videoCount;

QString CMainWindow::lastVdFileName = {""};
QString CMainWindow::lastPicFileName = {""};

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

static QWidget *createPicSelectableLineEditOptionHandle(QObject *opt)
{
    DTK_CORE_NAMESPACE::DSettingsOption *option = qobject_cast<DTK_CORE_NAMESPACE::DSettingsOption *>(opt);
    DLineEdit *picPathLineEdit = new DLineEdit;//文本框
    DWidget *main = new DWidget;
    QHBoxLayout *horboxlayout = new QHBoxLayout;
    DSuggestButton *icon = new DSuggestButton(main);
    QWidget *optionWidget = new QWidget;
    QFormLayout *optionLayout = new QFormLayout(optionWidget);
    DDialog *optinvaliddialog = new DDialog(optionWidget);
    //主目录相对路径
    QString relativeHomePath = QString("~") + QDir::separator();
    //保存上一次正确的文件夹路径，否则为默认文件夹
    static QString lastPicPath = nullptr;

    main->setLayout(horboxlayout);
    main->setContentsMargins(0, 0, 0, 0);
    horboxlayout->setAlignment(Qt::AlignTop);
    horboxlayout->setContentsMargins(0, 0, 0, 0);
    QFont ft("SourceHanSansSC");
    ft.setPixelSize(14);
    ft.setWeight(QFont::Medium);
    picPathLineEdit->setFont(ft);

    icon->setAutoDefault(false);
    picPathLineEdit->setFixedHeight(37);
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

    icon->setIcon(DStyleHelper(icon->style()).standardIcon(DStyle::SP_SelectElement, nullptr));
    icon->setIconSize(QSize(24, 24));
    icon->setFixedSize(41, 37);

    horboxlayout->addWidget(picPathLineEdit);
    horboxlayout->addWidget(icon);

    optionWidget->setFixedHeight(37);

    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);
    optionLayout->setVerticalSpacing(0);
    optionLayout->setMargin(0);

    main->setMinimumWidth(240);
    DLabel *lab = new DLabel(QObject::tr(option->name().toStdString().c_str()));
    lab->setAlignment(Qt::AlignVCenter);
    optionLayout->addRow(lab, main);
    workaround_updateStyle(optionWidget, "light");
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

    DLineEdit *videoPathLineEdit = new DLineEdit;
    DWidget *main = new DWidget;
    QHBoxLayout *horboxlayout = new QHBoxLayout;
    DSuggestButton *icon = new DSuggestButton(main);
    QWidget *optionWidget = new QWidget;
    QFormLayout *optionLayout = new QFormLayout(optionWidget);
    DDialog *optinvaliddialog = new DDialog(optionWidget);
    //主目录相对路径
    QString relativeHomePath = QString("~") + QDir::separator();
    static QString lastVideoPath = nullptr;

    main->setLayout(horboxlayout);
    main->setContentsMargins(0, 0, 0, 0);
    horboxlayout->setAlignment(Qt::AlignTop);
    horboxlayout->setContentsMargins(0, 0, 0, 0);
    QFont ft("SourceHanSansSC");
    ft.setPixelSize(14);
    ft.setWeight(QFont::Medium);
    videoPathLineEdit->setFont(ft);
    icon->setAutoDefault(false);
    videoPathLineEdit->setFixedHeight(37);
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

    icon->setIcon(DStyleHelper(icon->style()).standardIcon(DStyle::SP_SelectElement, nullptr));
    icon->setIconSize(QSize(24, 24));
    icon->setFixedSize(41, 37);
    horboxlayout->addWidget(videoPathLineEdit);
    horboxlayout->addWidget(icon);
    optionLayout->setContentsMargins(0, 0, 0, 0);
    optionLayout->setSpacing(0);
    optionWidget->setFixedHeight(37);
    main->setMinimumWidth(240);
    DLabel *lab = new DLabel(QObject::tr(option->name().toStdString().c_str()));
    lab->setAlignment(Qt::AlignVCenter);
    optionLayout->addRow(lab, main);
    workaround_updateStyle(optionWidget, "light");
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
        //打开文件夹
        selectVideoSavePath = DFileDialog::getExistingDirectory(nullptr, QObject::tr("Open folder"),
                                                                lastVideoPath,
                                                                DFileDialog::ShowDirsOnly | DFileDialog::DontResolveSymlinks);

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
        Settings::get().settings()->setOption("base.general.last_open_path", pi);
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

void CMainWindow::setWayland(bool bTrue)
{
    m_bWayland = bTrue;
    m_devnumMonitor = new DevNumMonitor();
    m_devnumMonitor->start();

    connect(m_devnumMonitor, SIGNAL(seltBtnStateEnable()), this, SLOT(setSelBtnShow()));
    //多设备信号
    connect(m_devnumMonitor, SIGNAL(seltBtnStateDisable()), this, SLOT(setSelBtnHide()));

    connect(m_devnumMonitor, SIGNAL(existDevice()), m_videoPre, SLOT(restartDevices()));

    if (bTrue) {
        m_pDBus = new QDBusInterface("org.freedesktop.login1", "/org/freedesktop/login1",
                                     "org.freedesktop.login1.Manager", QDBusConnection::systemBus());

//        m_pDBusLockFront = new QDBusInterface("com.deepin.dde.lockFront","/com/deepin/dde/lockFront",
//                                              "com.deepin.dde.lockFront",QDBusConnection::sessionBus());

        m_pDBusSessionMgr = new QDBusInterface("com.deepin.SessionManager", "/com/deepin/SessionManager",
                                               "com.deepin.SessionManager", QDBusConnection::sessionBus());

        m_pLockTimer = new QTimer;
        m_pLockTimer->setInterval(300);
        connect(m_pLockTimer, SIGNAL(timeout()), this, SLOT(onTimeoutLock()));//默认
        m_pLockTimer->start();
        //接收休眠信号，仅wayland使用
        connect(m_pDBus, SIGNAL(PrepareForSleep(bool)), this, SLOT(onSleepWhenTaking(bool)));

//        connect(m_pDBusLockFront, SIGNAL(Visible(bool)), this, SLOT(onVisible(bool)));
    }
}

void CMainWindow::settingDialog()
{
    m_SetDialog = new DSettingsDialog(this);
    m_SetDialog->setFixedSize(820, 600);
    m_SetDialog->widgetFactory()->registerWidget("selectableEditpic", createPicSelectableLineEditOptionHandle);
    m_SetDialog->widgetFactory()->registerWidget("selectableEditvd", createVdSelectableLineEditOptionHandle);
    m_SetDialog->widgetFactory()->registerWidget("formatLabel", createFormatLabelOptionHandle);

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
                         && (list_stream_formats[format_index].list_stream_cap[i].width % 16) == 0
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

                    if ((resolutiontemp1[0].toInt() < resolutiontemp2[0].toInt()) ||
                            (resolutiontemp1[0].toInt() == resolutiontemp2[0].toInt() && (resolutiontemp1[1].toInt() < resolutiontemp2[1].toInt()))) {
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

CMainWindow::CMainWindow(DWidget *w): DMainWindow (w)
{
    m_bWayland = false;

    m_nActTpye = ActTakePic;

    initUI();
    initTitleBar();
    initConnection();
    m_thumbnail->addPaths(lastVdFileName, lastPicFileName);
    QDir dir;
    QString strCache = QString(getenv("HOME")) + QString("/") + QString(".cache/deepin/deepin-camera/");
    dir.mkpath(strCache);
}

CMainWindow::~CMainWindow()
{
    qInfo() << "stop_encoder_thread";
}

void CMainWindow::slotPopupSettingsDialog()
{
    settingDialog();
    m_SetDialog->exec();
    settingDialogDel();
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
        qInfo() << "init Nublock shutdown.";
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

    //int fd = -1;
    m_replySleep = m_pLoginMgrSleep->callWithArgumentList(QDBus::Block, "Inhibit", m_argSleep);
    if (m_replySleep.isValid()) {
        /*fd = */(void)m_replySleep.value().fileDescriptor();
    }
    //如果for结束则表示没有发现未保存的tab项，则放开阻塞睡眠
    if (m_replySleep.isValid()) {
        QDBusReply<QDBusUnixFileDescriptor> tmp = m_replySleep;
        m_replySleep = QDBusReply<QDBusUnixFileDescriptor>();
        qInfo() << "init Nublock sleep.";
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
        qInfo() << "Nublock shutdown.";
    }

    if (m_bWayland) {
        initBlockSleep();

        if (bTrue) {
            m_replySleep = m_pLoginMgrSleep->callWithArgumentList(QDBus::Block, "Inhibit", m_argSleep);
        } else {
            QDBusReply<QDBusUnixFileDescriptor> tmp = m_replySleep;
            m_replySleep = QDBusReply<QDBusUnixFileDescriptor>();
            //m_pLoginManager->callWithArgumentList(QDBus::NoBlock, "Inhibit", m_arg);
            qInfo() << "Nublock sleep.";
        }
    }
}

void CMainWindow::onNoCam()
{
    onEnableTitleBar(3); //恢复按钮状态
    onEnableTitleBar(4); //恢复按钮状态
    onEnableSettings(true);
    m_thumbnail->m_nStatus = STATNULL;
    //m_thumbnail->setBtntooltip();
    m_thumbnail->show();
}

void CMainWindow::onSleepWhenTaking(bool bTrue)
{
    if (m_bWayland && bTrue) {
        qInfo() << "onSleepWhenTaking(bool)";
        m_videoPre->endBtnClicked();
        qInfo() << "onSleepWhenTaking(over)";
    }
}

void CMainWindow::onVisible(bool bTrue)
{
    qInfo() << "onVisible " << bTrue;
    if (bTrue) {
        qInfo() << "locked";
        if (m_videoPre->getCapstatus()) {
            m_videoPre->endBtnClicked();
        }
        m_videoPre->m_imgPrcThread->stop();
        m_bLocked = true;
        qInfo() << "lock end";
    }
    if (!bTrue && m_bLocked) {
        qInfo() << "restart use camera cause ScreenBlack or PoweerLock";
        //打开摄像头
        m_videoPre->changeDev();
        qInfo() << "v4l2core_start_stream OK";
    }
}

void CMainWindow::onTimeoutLock()
{
    if (m_pDBusSessionMgr) {
        if (m_pDBusSessionMgr->property("Locked").value<bool>()) {
            qInfo() << "locked";
            if (m_videoPre->getCapstatus()) {
                m_videoPre->endBtnClicked();
            }
            m_videoPre->m_imgPrcThread->stop();
            m_bLocked = true;
            qInfo() << "lock end";
        } else {
            if (m_bLocked) {
                qInfo() << "restart use camera cause ScreenBlack or PoweerLock";
                //打开摄像头
                m_videoPre->changeDev();
                qInfo() << "v4l2core_start_stream OK";
                m_bLocked = false;
            }
        }

    }
}

void CMainWindow::initUI()
{
    this->resize(MinWindowWidth, MinWindowHeight);
    m_videoPre = new videowidget(this);

    this->setCentralWidget(m_videoPre);

    QPalette paletteTime = m_videoPre->palette();
    paletteTime.setBrush(QPalette::Dark, QColor(/*"#202020"*/0, 0, 0, 51)); //深色
    m_videoPre->setPalette(paletteTime);

    lastVdFileName = lastOpenedPath(QStandardPaths::MoviesLocation);//如果路径不存在会自动使用并创建默认路径
    lastPicFileName = lastOpenedPath(QStandardPaths::PicturesLocation);

    m_fileWatcher.addPath(lastVdFileName);
    m_fileWatcher.addPath(lastPicFileName);

    m_videoPre->setSaveVdFolder(lastVdFileName);
    m_videoPre->setSavePicFolder(lastPicFileName);

    int nContinuous = Settings::get().getOption("photosetting.photosnumber.takephotos").toInt();
    int nDelayTime = Settings::get().getOption("photosetting.photosdelay.photodelays").toInt();

    setupTitlebar();

    m_thumbnail = new ThumbnailsBar(this);
    m_thumbnail->move(0, height() - 10);
    m_thumbnail->setFixedHeight(LAST_BUTTON_HEIGHT + LAST_BUTTON_SPACE * 2);
    m_videoPre->setthumbnail(m_thumbnail);

    m_thumbnail->show();
    m_thumbnail->setVisible(true);
    m_thumbnail->show();

    m_thumbnail->m_nMaxWidth = MinWindowWidth;

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
    m_pTitlePicBtn->setFocusPolicy(Qt::NoFocus);

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
    pSelectBtn->hide();
    if (type == DGuiApplicationHelper::UnknownType || type == DGuiApplicationHelper::LightType) {
        pSelectBtn->setIcon(QIcon(":/images/icons/light/button/Switch camera.svg"));
    } else {
        pSelectBtn->setIcon(QIcon(":/images/icons/dark/button/Switch camera_dark.svg"));
    }

    titlebar()->setIcon(QIcon::fromTheme("deepin-camera"));
    titlebar()->addWidget(pSelectBtn, Qt::AlignLeft);
}

void CMainWindow::initConnection()
{
    connect(CamApp, &CApplication::popupConfirmDialog, this, [ = ] {
        if (m_videoPre->getCapstatus())
        {
            CloseDialog closeDlg (this, tr("Video recording is in progress. Close the window?"));
            int ret = closeDlg.exec();
            if (ret == 1) {
                m_videoPre->endBtnClicked();
                qApp->quit();
            }
        } else
        {
            qApp->quit();
        }
    });
    //connect(this, SIGNAL(windowstatechanged(Qt::WindowState windowState)), this, SLOT(onCapturepause(Qt::WindowState windowState)));
    //系统文件夹变化信号
    connect(&m_fileWatcher, SIGNAL(directoryChanged(const QString &)), m_thumbnail, SLOT(onFoldersChanged(const QString &)));
    //系统文件变化信号
    connect(&m_fileWatcher, SIGNAL(fileChanged(const QString &)), m_thumbnail, SLOT(onFoldersChanged(const QString &)));
    //传递文件名，在拍照录制开始的时候，创建的文件不用于更新缩略图
    connect(m_videoPre, SIGNAL(filename(QString)), m_thumbnail, SLOT(onFileName(QString)));
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
    //录制取消 (倒计时3秒内的)
    connect(m_videoPre, SIGNAL(takeVdCancel()), this, SLOT(onTakeVdCancel()));
    //录制关机/休眠阻塞
    connect(m_videoPre, SIGNAL(updateBlockSystem(bool)), this, SLOT(updateBlockSystem(bool)));

    //没有相机了，结束拍照、录制
    connect(m_videoPre, SIGNAL(noCam()), this, SLOT(onNoCam()));
    //相机被抢占了，结束拍照、录制
    connect(m_videoPre, SIGNAL(noCamAvailable()), this, SLOT(onNoCam()));
    //设备切换信号
    connect(pSelectBtn, SIGNAL(clicked()), m_videoPre, SLOT(changeDev()));

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

    if (m_thumbnail) {
        m_thumbnail->fitSize(width);
        onFitToolBar();
    }

    m_videoPre->update();
}

void CMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_videoPre->getCapstatus()) {
        CloseDialog closeDlg (this, tr("Video recording is in progress. Close the window?"));
        int ret = closeDlg.exec();
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
    //    qInfo() << this->windowState() << endl;
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
        m_fileWatcher.addPath(lastVdFileName);
        m_fileWatcher.addPath(lastPicFileName);

        QString strPic = dirPic.path();
        m_thumbnail->addPaths(lastVdFileName, lastPicFileName);
    } else {
        if (QString::compare(lastVdFileName, lastPicFileName) == 0) {
            m_fileWatcher.removePaths(m_fileWatcher.directories());
            m_fileWatcher.addPath(lastVdFileName);
            m_thumbnail->addPaths(lastVdFileName, lastPicFileName);
        }
    }

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
    Settings::get().settings()->sync();
}

void CMainWindow::onEnableSettings(bool bTrue)
{
    m_actionSettings->setEnabled(bTrue);
}

void CMainWindow::onTakePicDone()
{
    qInfo() << "onTakePicDone";
    onEnableTitleBar(3); //恢复按钮状态
    onEnableSettings(true);
    m_thumbnail->m_nStatus = STATNULL;
    m_thumbnail->setBtntooltip();
    QString strPath = m_videoPre->m_imgPrcThread->m_strPath;
    m_thumbnail->addFile(strPath);
}

void CMainWindow::onTakePicOnce()
{
    qInfo() << "onTakePicOnce";
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
        QString strFileName = lastVdFileName + "/" + g_strFileName;
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
        qInfo() << "shift pressed";
        g_bMultiSlt = true;
        g_setIndex.insert(g_indexNow);
    }
}

void CMainWindow::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Shift) {
        qInfo() << "shift released";
        g_bMultiSlt = false;
        //g_setIndex.clear();
        //g_setIndex.insert(g_indexNow);
    }
}
