#ifndef GLWIDGET_H
#define GLWIDGET_H

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
    QSize sizeHint() const;
	void loadDataSet(std::string fileName);
	void check_gl_error (std::string from);

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

private:
    QTimer *timer;
	Volume g_Volume;
	GLuint dataTexture;
	float g_fCoordX;
	float g_fCoordY;
	float g_fCoordZ;
	GLuint fragmentShader;
	GLuint vertexShader;
	GLuint transferProgram;
	GLuint transferTexture;
	GLuint fbo;
	GLuint depth_rb;
	TFTexture *transferFunction;

	const int GetNextPowerOfTwo(const int iNumber);
	void setShaders(void);
	char* readShader(char *fn);
};

#endif // GLWIDGET_H
