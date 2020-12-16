/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
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
#ifndef SHORTCUT_H
#define SHORTCUT_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

struct ShortcutItem {
    QString name;
    QString value;
    ShortcutItem(QString n, QString v): name(n), value(v) {}
};

struct ShortcutGroup {
    QString groupName;
    QList<ShortcutItem> groupItems;
};

/**
 * @brief setPic 取消image带来的警告
 * space停止拍照或录像，Alt + M调用菜单，Ctrl + C复制，Ctrl + O打开文件夹，Ctrl + P打印，F1帮助，Delete删除，Ctrl + Shift + ?列出快捷键
 */
class Shortcut : public QObject
{
    Q_OBJECT
public:
    explicit Shortcut(QObject *parent = 0);

    /**
    * @brief toStr 数据类型转换
    */
    QString toStr();

private:
    QJsonObject          m_shortcutObj;
    QList<ShortcutGroup> m_shortcutGroups;
};

#endif // SHORTCUT_H
