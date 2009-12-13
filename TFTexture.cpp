#include "TFTexture.h"
#include <algorithm>

TFTexture::TFTexture(QWidget *parent)
{
	this->parent = parent;

	nodes = std::vector<TFNode>();

	addNode(TFNode(0, 0.0, 0.0, 0.0, 0.0));
	addNode(TFNode(96 * 8.0, 0.5, 0.1, 0.0, 0.05));
	addNode(TFNode(128 * 8.0, 0.9, 0.05, 0.1, 0.1));
	addNode(TFNode(164 * 8.0, 0.75, 0.8, 0.5, 0.36));
	addNode(TFNode(196 * 8.0, 0.95, 0.89, 0.65, 0.76));
	addNode(TFNode(256 * 8.0, 0.99, 0.99, 0.96, 0.97));
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

void TFTexture::clear(void)
{
	nodes.clear();
}

void TFTexture::setFBO(GLuint fbo, GLuint texture)
{
	this->fbo = fbo;
	this->texture = texture;
}

void TFTexture::setHistogram(int* histogram)
{
	int histHeight = 0;
	for (int i = 1; i < 256; i++) {
		if (histogram[i] > histHeight)
			histHeight = histogram[i];
	}
	for (int i = 1; i < 256; i++) {
		this->histogram[i] = static_cast<float>(histogram[i]) / histHeight;
	}
}


void TFTexture::generate(void)
{
	glPushMatrix();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	 
	glViewport(0, 0, 2048, 2048);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 2048.0, 0.0, 2048.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	 
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
		glVertex3f(prevNode.x, 0, -1);
		glColor4f(node->r, node->g, node->b, node->a);
		glVertex3f(node->x, 0, -1);
		glColor4f(node->r, node->g, node->b, node->a);
		glVertex3f(node->x, 2048, -1);
		glColor4f(prevNode.r, prevNode.g, prevNode.b, prevNode.a);
		glVertex3f(prevNode.x, 2048, -1);
		 
		prevNode = *node;
	}
	glEnd();
	 
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	 
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	 
	glViewport(0, 0, parent->width(), parent->height());
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glPopMatrix();
}
