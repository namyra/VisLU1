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
	void removeNode(GLuint x);
	std::map<int, TFNode>::iterator begin();
	std::map<int, TFNode>::iterator end();
	void setFBO(GLuint fbo, GLuint texture);
	void generate(void);
	void clear(void);
	void setHistogram(int* histogram);
	float histogram[256];

private:
	
	std::map<int, TFNode> nodes;
	QWidget *parent;
	GLuint fbo;
	GLuint texture;
};

