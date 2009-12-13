#include "TFView.h"
#include <QMouseEvent>
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
	
	for (unsigned int i = 1; i < 256; i++) {
		QPen pen(QColor(i, 0.25, 255 - i, 255), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
		scene()->addLine(static_cast<double>(i), sceneHeight, static_cast<double>(i), sceneHeight * (1 - tf->histogram[i]), pen);
	}

	QPen pen2(Qt::black, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);

	for(node = tf->begin(); node < tf->end(); node++)
	{
		scene()->addRect(((float)node->x / 2048 * sceneWidth - 4.0) , (1 - node->a) * sceneHeight - 4.0, 9, 9, QPen(), QBrush(QColor(node->r * 255, node->g * 255, node->b * 255)));
		 
		if(node == tf->begin())
		{
			prevNode = *node;
			continue;
		}
		 
		scene()->addLine((float)prevNode.x / 2048 * sceneWidth, (1 - prevNode.a) * sceneHeight, (float)node->x / 2048 * sceneWidth, (1 - node->a) * sceneHeight, pen2);
		 
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

			QColor color = QColorDialog::getColor(Qt::green, this);
			if (color.isValid()) {
				float r = color.redF();
				float g = color.greenF();
				float b = color.blueF();
				tf->addNode(TFNode(pos.x() / sceneWidth * 2048, r, g, b, 1 - pos.y() / sceneHeight));

				tf->generate();
				drawTF();
			}
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		tf->clear();
		tf->generate();
		drawTF();
	}
}

TFView::~TFView(void)
{
}
