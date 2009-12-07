#include "TFView.h"
#include <QMouseEvent>
#include <algorithm>

TFView::TFView(QGraphicsScene *scene, TFTexture *tf) : QGraphicsView(scene)
{
	this->tf = tf;
	drawTF();
}

void TFView::drawTF()
{
	sort(tf->begin(), tf->end());

	std::vector<TFNode>::iterator node;
	TFNode prevNode;

	for(node = tf->begin(); node < tf->end(); node++)
	{
		if(node == tf->begin())
		{
			prevNode = *node;
			continue;
		}
		
		scene()->addLine((float)prevNode.x / 256 * width(), (1 - prevNode.a) * 150, (float)node->x / 256 * width(), (1 - node->a) * 150);

		prevNode = *node;
	}

	for(node = tf->begin(); node < tf->end(); node++)
	{
		scene()->addRect((float)node->x / 256 * width() - 4, (1 - node->a) * 150 - 4, 9, 9, QPen(), QBrush(QColor(node->r * 255, node->g * 255, node->b * 255)));
	}
}

void TFView::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		if(scene()->itemAt(mapToScene(event->pos())))
			dragStartPosition = event->pos();
		//else
		//	scene()->addRect(QRectF(mapToScene(event->pos()), QSizeF(5, 5)));
	}
}

TFView::~TFView(void)
{
}
