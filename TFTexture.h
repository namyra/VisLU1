#pragma once

#include <QDebug>

#include "TFNode.h"
#include "qgl.h"

class TFTexture
{
public:
	TFTexture(QWidget *parent);
	~TFTexture(void);
	void addNode(TFNode node);
	std::vector<TFNode>::iterator begin();
	std::vector<TFNode>::iterator end();
	void setFBO(GLuint fbo, GLuint texture);
	void generate(void);
	void clear(void);
	void setHistogram(int* histogram);
	float histogram[256];

private:
	
	std::vector<TFNode> nodes;
	QWidget *parent;
	GLuint fbo;
	GLuint texture;
};

