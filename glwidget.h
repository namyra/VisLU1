#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <algorithm>

#include "common.h"
#include <qgl.h>
#include "Volume.h"
#include "Timer.h"
#include "TFTexture.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(int timerInterval=0, QWidget *parent=0);
	~GLWidget();
    QSize sizeHint() const;
	void loadDataSet(std::string fileName);
	void check_gl_error (std::string from);
	TFTexture* transferFunction();

protected:
	void initializeGL(void);
    void resizeGL(int width, int height);
    void paintGL();

    void timeOut();

protected slots:
    void timeOutSlot();
    void setX(int x);
    void setY(int y);
    void setZ(int z);
	void setView(bool v);

private:
	bool viewMode;
    QTimer *timer;
	Volume g_Volume;
	GLuint dataTexture;
	GLuint gradientTexture;
	float g_fCoordX;
	float g_fCoordY;
	float g_fCoordZ;
	GLuint fragmentShader;
	GLuint vertexShader;
	GLuint rayShaderV;
	GLuint rayShaderF;
	GLuint transferProgram;
	GLuint transferTexture;
	GLuint rayProgram;
	GLuint fbo_transfer;
	GLuint fbo_front;
	GLuint fbo_back;
	GLuint tex_front;
	GLuint tex_back;
	GLuint render_front;
	GLuint render_back;
	GLuint depth_rb;
	TFTexture *tf;

	const int GetNextPowerOfTwo(const int iNumber);
	void setShaders(void);
	char* readShader(char *fn);

	void drawBoundingBox(GLuint& fbo);
};

#endif // GLWIDGET_H
