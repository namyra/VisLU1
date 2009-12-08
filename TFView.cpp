#include "TFView.h"
#include <QMouseEvent>
#include <QDebug>
#include <algorithm>

TFView::TFView(QGraphicsScene *scene, TFTexture *tf) : QGraphicsView(scene)
{
	this->tf = tf;
}

void TFView::clear()
{
	QList<QGraphicsItem *> allItems = this->items();
	foreach(QGraphicsItem* i, allItems)
	{
		scene()->removeItem(i);
	}
}

void TFView::drawTF()
{
	qint32 sceneWidth = viewport()->width();
	qint32 sceneHeight = viewport()->height();

	clear();

	sort(tf->begin(), tf->end());
	std::vector<TFNode>::iterator node;
	TFNode prevNode;

	for(node = tf->begin(); node < tf->end(); node++)
	{
		scene()->addRect((float)node->x / 256 * sceneWidth - 4, (1 - node->a) * sceneHeight - 4, 9, 9, QPen(), QBrush(QColor(node->r * 255, node->g * 255, node->b * 255)));

		if(node == tf->begin())
		{
			prevNode = *node;
			continue;
		}
		
		scene()->addLine((float)prevNode.x / 256 * sceneWidth, (1 - prevNode.a) * sceneHeight, (float)node->x / 256 * sceneWidth, (1 - node->a) * sceneHeight);

		prevNode = *node;
	}
}

void TFView::mousePressEvent(QMouseEvent *event)
{
	qint32 sceneWidth = viewport()->width();
	qint32 sceneHeight = viewport()->height();

	if(event->button() == Qt::LeftButton)
	{
		QPointF pos = mapToScene(event->pos());

		if(scene()->itemAt(pos))
			dragStartPosition = event->pos();
		else
		{
			tf->addNode(TFNode(pos.x() / sceneWidth * 256, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float)rand() / RAND_MAX, 1 - pos.y() / sceneHeight));
			tf->generate();
			drawTF();
		}
	}
}

TFView::~TFView(void)
{
}
