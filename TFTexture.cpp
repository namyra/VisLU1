#include "TFTexture.h"
#include <algorithm>

TFTexture::TFTexture(void)
{
	nodes = std::vector<TFNode>();

	addNode(TFNode(0, 1.0, 1.0, 1.0, 0.0));
	addNode(TFNode(64, 1.0, 0.0, 0.0, 0.8));
	addNode(TFNode(148, 0.2, 0.6, 1.0, 1.0));
	addNode(TFNode(256, 0.3, 1.0, 0.4, 1.0));
}

TFTexture::~TFTexture(void)
{
}

void TFTexture::addNode(TFNode node)
{
	nodes.push_back(node);
}

std::vector<TFNode>::iterator TFTexture::begin()
{
	return nodes.begin();
}

std::vector<TFNode>::iterator TFTexture::end()
{
	return nodes.end();
}

void TFTexture::generate()
{
	glViewport(0, 0, 256, 256);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 256.0, 0.0, 256.0, -1.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
//	check_gl_error("projection change");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);

	sort(nodes.begin(), nodes.end());

	std::vector<TFNode>::iterator node;
	TFNode prevNode;

	for(node = nodes.begin(); node < nodes.end(); node++)
	{
		if(node == nodes.begin())
		{
			prevNode = *node;
			continue;
		}
		
		glColor4f(prevNode.r, prevNode.g, prevNode.b, prevNode.a);
		glVertex3f(prevNode.x,0,-1);
		glColor4f(node->r, node->g, node->b, node->a);
		glVertex3f(node->x,0,-1);
		glColor4f(node->r, node->g, node->b, node->a);
		glVertex3f(node->x,256,-1);
		glColor4f(prevNode.r, prevNode.g, prevNode.b, prevNode.a);
		glVertex3f(prevNode.x,256,-1);

		prevNode = *node;
	}
	glEnd();
//	check_gl_error("transfer texture quad");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
//	check_gl_error("bind FBO");
}
