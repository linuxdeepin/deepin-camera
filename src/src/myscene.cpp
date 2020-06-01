/*
 * (c) 2020, Uniontech Technology Co., Ltd. <support@deepin.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
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
