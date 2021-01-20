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
#include "shortcut.h"

Shortcut::Shortcut(QObject *parent)
    : QObject(parent)
{
    ShortcutGroup group1;
    ShortcutGroup group3;

    group1.groupName = tr("Camera");
    group3.groupName = tr("Settings");
    group1.groupItems << ShortcutItem(tr("Stop recording"), "space")
                      << ShortcutItem(tr("Stop taking photos"), "space")
                      << ShortcutItem(tr("Context menu"), "Alt + M")
                      << ShortcutItem(tr("Copy"), "Ctrl + C")
                      << ShortcutItem(tr("Delete"), "Delete")
                      << ShortcutItem(tr("Open"), "Ctrl + O")
                      << ShortcutItem(tr("Print"), "Ctrl + P");
    group3.groupItems << ShortcutItem(tr("Help"), "F1")
                      << ShortcutItem(tr("Display shortcuts"), "Ctrl + Shift + ?");
    m_shortcutGroups << group1 << group3;

    // convert to json object
    QJsonArray jsonGroups;
    for (auto scg : m_shortcutGroups) {
        QJsonObject jsonGroup;
        jsonGroup.insert("groupName", scg.groupName);
        QJsonArray jsonGroupItems;
        for (auto sci : scg.groupItems) {
            QJsonObject jsonItem;
            jsonItem.insert("name", sci.name);
            jsonItem.insert("value", sci.value);
            jsonGroupItems.append(jsonItem);
        }
        jsonGroup.insert("groupItems", jsonGroupItems);
        jsonGroups.append(jsonGroup);
    }

    m_shortcutObj.insert("shortcut", jsonGroups);
}

QString Shortcut::toStr()
{
    QJsonDocument doc(m_shortcutObj);
    return doc.toJson().data();
}
