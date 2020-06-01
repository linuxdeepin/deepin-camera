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
