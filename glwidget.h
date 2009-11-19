#ifndef GLWIDGET_H
#define GLWIDGET_H

#include "common.h"
#include <qgl.h>
#include "Volume.h"
#include "Timer.h"

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
    void setRed(int intensity);
    void setGreen(int intensity);
    void setBlue(int intensity);
    void setDirection(int direction);
    void setColor(int color);

private:
    QTimer *timer;
	Volume g_Volume;
	GLuint dataTexture;
	float g_zIncrement;
	float g_fCoordZ;
	GLuint fragmentShader;
	GLuint vertexShader;
	GLuint transferProgram;
	GLuint transferTexture;
	GLuint fbo;
	GLuint depth_rb;

    GLfloat rtri, rdir, red, green, blue, rquad, gquad, bquad;
	const int GetNextPowerOfTwo(const int iNumber);
	void setShaders(void);
};

#endif // GLWIDGET_H
