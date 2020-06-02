/*
* Copyright (C) 2020 ~ %YEAR% Uniontech Software Technology Co.,Ltd.
*
* Author:     shicetu <shicetu@uniontech.com>
*
* Maintainer: shicetu <shicetu@uniontech.com>
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

#include "myscene.h"
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QDebug>
#include <QGraphicsItem>
#include<QWindow>


MyScene::MyScene(QObject *parent):
    QGraphicsScene(parent)
{
}
void MyScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    QPointF pos(event->scenePos().x() - beforePos.x(), event->scenePos().y() - beforePos.y());
}
void MyScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    foreach (QGraphicsItem *item, items(event->scenePos())) {
        qDebug() << getSceneName();
        emit selectEff(getSceneName());
        break;
    }
}
void MyScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
}

void MyScene::setSceneName(QString name)
{
    if (name == NULL)
        return;
    sceneName = name;
}

QString MyScene::getSceneName()
{
    return sceneName;
}
