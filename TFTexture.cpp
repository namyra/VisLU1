#include "TFTexture.h"
#include <QDebug>
#include <algorithm>
#include "glwidget.h"

TFTexture::TFTexture(QWidget *parent)
{
	this->parent = parent;

	nodes = std::vector<TFNode>();

	addNode(TFNode(0, 1.0, 1.0, 1.0, 0.0));
	addNode(TFNode(64, 1.0, 0.0, 0.0, 0.8));
	addNode(TFNode(148, 0.2, 0.6, 1.0, 1.0));
	addNode(TFNode(256, 0.3, 1.0, 0.4, 1.0));
}

TFTexture::~TFTexture(void)
{
}

void TFTexture::setFBO(GLuint fbo)
{
	this->fbo = fbo;
}

void TFTexture::setDepthRB(GLuint depth_rb)
{
	this->depth_rb = depth_rb;
}

void TFTexture::setTexture(GLuint texture)
{
	this->texture = texture;
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
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	GLWidget::check_gl_error("bind FBO");

	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
	GLWidget::check_gl_error("bind renderbuffer");
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 256, 256);
	GLWidget::check_gl_error("buffer storage");

	glBindTexture(GL_TEXTURE_2D, texture);
	GLWidget::check_gl_error("bind transfer texture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	GLWidget::check_gl_error("teximage2d transfer texture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLWidget::check_gl_error("tex parameters filter");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLWidget::check_gl_error("tex parameters wrapping");

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);
	GLWidget::check_gl_error("framebuffer");
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
	GLWidget::check_gl_error("renderbuffer");

	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		qDebug() << "The FBO is not complete";
		qDebug() << "Error: " << glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	}

	glViewport(0, 0, 256, 256);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 256.0, 0.0, 256.0, -1.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLWidget::check_gl_error("projection change");

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
		glVertex3f(prevNode.x, 0, -1);
		glColor4f(node->r, node->g, node->b, node->a);
		glVertex3f(node->x, 0, -1);
		glColor4f(node->r, node->g, node->b, node->a);
		glVertex3f(node->x, 256, -1);
		glColor4f(prevNode.r, prevNode.g, prevNode.b, prevNode.a);
		glVertex3f(prevNode.x, 256, -1);

		prevNode = *node;
	}
	glEnd();
	GLWidget::check_gl_error("transfer texture quad");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	GLWidget::check_gl_error("bind FBO");

	glViewport(0, 0, parent->width(), parent->height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLWidget::check_gl_error("projection change back");
}
