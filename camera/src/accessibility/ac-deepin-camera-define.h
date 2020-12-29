/**
 * Copyright (C) 2020 UOS Technology Co., Ltd.
 *
 * to mark the desktop UI
 **/

#ifndef DESKTOP_ACCESSIBLE_UI_DEFINE_H
#define DESKTOP_ACCESSIBLE_UI_DEFINE_H

#include <QString>
#include <QObject>

// 使用宏定义，方便国际化操作
//mainwindow
#define BUTTOM_TITLE_PICTURE QObject::tr("TitlePicBtn")
#define BUTTOM_TITLE_VEDIO QObject::tr("TitleVdBtn")
#define BUTTOM_TITLE_SELECT QObject::tr("SelectBtn")
#define BUTTOM_TITLE_BOX QObject::tr("TitleButtonBox")
#define BUTTON_OPTION_LINE_EDIT QObject::tr("OptionLineEditBtn")
#define BUTTON_TITLE_BOX_LIST QObject::tr("TitleBtnlistBox")
#define BUTTON_RIGHT_MENU QObject::tr("Rightbtnmenu")

#define MAIN_WINDOW QObject::tr("MainWindow")
#define TITLE_MUNE QObject::tr("titleMenu")
#define TITLEBAR QObject::tr("TitleBar")
#define OPTION_FORMAT_LABER QObject::tr("OptionFormatLabel")
#define OPTION_FRAME QObject::tr("OptionFrame")
#define OPTION_SELECTABLE_LINE_EDIT QObject::tr("OptionSelectableLineEdit")
#define OPTION_INVALID_DIALOG QObject::tr("OptionInvalidDialog")
#define SETTING_DIALOG QObject::tr("SettingDialog")
#define VIDEO_PREVIEW_WIDGET QObject::tr("VideoPreviewWidget")
#define THUMBNAIL QObject::tr("Thumbnail")

//#define SETTING_ACTION QObject::tr("SettingAction")
#define CLOSE_DIALOG QObject::tr("CloseDlg")

//imageitem
#define PRINT_DIALOG QObject::tr("PrintDialog")

//closedialog
#define CANCEL_BUTTON QObject::tr("CancelBtn")
#define CLOSE_BUTTON QObject::tr("CloseBtn")


//thumbnailbar
#define BUTTON_PICTURE_VIDEO QObject::tr("PicVdBtn")

//videowidget
#define BUTTON_TAKE_VIDEO_END QObject::tr("TakeVdEndBtn")

////顶部菜单栏
#define TITLEBAR QObject::tr("TitleBar")


#endif // DESKTOP_ACCESSIBLE_UI_DEFINE_H
