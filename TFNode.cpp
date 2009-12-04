#include "TFNode.h"

TFNode::TFNode(void)
{
	x = 0;
	r = g = b = a = 0.0;
}

TFNode::TFNode(GLuint x, GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
	this->x = x;
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

TFNode::~TFNode(void)
{
}

bool TFNode::operator < (const TFNode &node)
{
	return this->x < node.x;
}
