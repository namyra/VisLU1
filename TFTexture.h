#pragma once

#include "TFNode.h"
#include "qgl.h"

class TFTexture
{
public:
	TFTexture(void);
	TFTexture(QWidget *parent);
	~TFTexture(void);
	void setFBO(GLuint fbo);
	void addNode(TFNode node);
	std::vector<TFNode>::iterator begin();
	std::vector<TFNode>::iterator end();
	void generate();

private:
	std::vector<TFNode> nodes;
	QWidget *parent;
	GLuint fbo;
};

