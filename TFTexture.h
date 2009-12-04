#pragma once

#include "TFNode.h"
#include "qgl.h"

class TFTexture
{
public:
	TFTexture(void);
	~TFTexture(void);
	void addNode(TFNode node);
	void generate();

private:
	std::vector<TFNode> *nodes;
};

