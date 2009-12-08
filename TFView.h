#pragma once

#include <QGraphicsView>
#include "tftexture.h"

class TFView :
	public QGraphicsView
{
public:
	TFView(void);
	TFView(QGraphicsScene *scene, TFTexture *transferFunction);
	~TFView(void);
	void mousePressEvent(QMouseEvent *event);
	void clear();
	void drawTF();

private:
	QPoint dragStartPosition;
	TFTexture *tf;
};

