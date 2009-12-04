#pragma once

#include "common.h"

class TFNode
{
public:
	GLuint x;
	GLfloat r, g, b, a;
	TFNode(void);
	TFNode(GLuint x, GLfloat r, GLfloat g, GLfloat b, GLfloat a);
	~TFNode(void);
	bool operator<(const TFNode &node);
};

