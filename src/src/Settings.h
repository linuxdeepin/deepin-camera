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

#ifndef DC_SETTINGS_H
#define DC_SETTINGS_H

#include "videowidget.h"

#include <DSettingsOption>
#include <DSettingsGroup>
#include <DSettings>

#include <QObject>
#include <QPointer>


#include <qsettingbackend.h>
/**
* @brief dc　相机设置界面命名空间
*/
namespace dc {
using namespace Dtk::Core;

/**
* @brief Settings　相机设置界面
*/
class Settings: public QObject
{
    Q_OBJECT
public:
     ~Settings();
    /**
    * @brief get　获取设置对象
    */
    static Settings &get();

    /**
    * @brief get　初始化
    */
    void init();

    /**
    * @brief configPath　获取设置路径
    */
    QString configPath() const
    {
        return m_configPath;
    }

    /**
    * @brief settings　获取DSettings对象
    */
    QPointer<DSettings> settings()
    {
        return m_settings;
    }

    /**
    * @brief group 获取组
    */
    QPointer<DSettingsGroup> group(const QString &name)
    {
        return settings()->group(name);
    }

    /**
    * @brief base　获取group.base
    */
    QPointer<DSettingsGroup> base()
    {
        return group("base");
    }

    /**
    * @brief generalOption 获取json下的base.general.%1
    * @param opt
    */
    QVariant generalOption(const QString &opt);

    /**
    * @brief setPathOption 设置json下的base.save.%1的值
    * @param opt
    * @param v
    */
    void setPathOption(const QString &opt, const QVariant &v);

    /**
    * @brief getOption 获取json下的值
    * @param opt
    */
    QVariant getOption(const QString &opt);

    /**
    * @brief setOption 设置json下的值
    * @param opt
    */
    void setOption(const QString &opt, const QVariant &v);

    /**
    * @brief setBackOption 设置config.ini下的opt的值
    * @param opt
    * @param v
    */
    void setBackOption(const QString &opt, const QVariant &v);

    /**
    * @brief setBackOption 获取config.ini下的值
    * @param opt
    */
    QVariant getBackOption(const QString &opt);

public slots:

    /**
     * @brief resolutionchanged 设置新分辨率表
     */
    void setNewResolutionList();

signals:

    /**
     * @brief resolutionchanged 分辨率改变信号
     * @param
     */
    void resolutionchanged(const QString &);
     /**
     * @brief delayTimeChanged 延迟拍照时间改变信号
     * @param
     */
    void delayTimeChanged(const QString &);

    /**
    * @brief mirrorModeChanged 镜像设置改变信号
    * @param bMirror true 镜像  false 非镜像
    */
    void mirrorModeChanged(bool bMirror);

    /**
    * @brief flashLightChanged 闪光灯开启关闭
    * @param bLight true 开启闪光  false 关闭闪光
    */
    void flashLightChanged(bool bLight);

private:
    Settings();
    static Settings     m_instance;
    QString             m_configPath;
    QPointer<DSettings> m_settings;
    QSettingBackend*    m_backend;
};

}

#endif /* ifndef _DMR_SETTINGS_H */
