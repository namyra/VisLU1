/*! \file glwidget.h
	\brief GLWidget header file.

	Contains the declarations for the GLWidget class, which is the main display widget, showing the slices/volume renders.
*/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <algorithm>

#include "common.h"
#include <qgl.h>
#include "Volume.h"
#include "TFTexture.h"

//! Display widget for the volume dataset.
/*!
	Provides the widget that takes care of all the rendering of the volume data.
	Most of the work happens here.
	Includes both the 2d (slicing) and 3d (direct volume rendering) processes.
*/
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:

	//! Constructor.
	/*!
		\param timerInterval The interval (in milliseconds) the widget uses to redraw.
		\param parent This widget's parent widget.
		\sa ~GLWidget()
	*/
    GLWidget(int timerInterval=0, QWidget *parent=0);
	
	//! Default destructor.
	/*!
		Deletes all the generated textures.
		\sa GLWidget()
	*/
	~GLWidget();

	//! Tells Qt which size this widget prefers to have.
	/*!
		Overwritten from Widget.
	*/
    QSize sizeHint() const;

	//! Loads a volume dataset.
	/*!
		Also generates the texture containing the dataset (3d if supported by hardware, 2d otherwise).
		\param fileName The filename of the volume data (.dat).
	*/
	void loadDataSet(std::string fileName);

	//! Utility function for debugging.
	/*!
		Outputs the last occurring OpenGL error (at the time of the function call)
		as well as a custom label to indicate where in the code this function call is.
		\param from The custom label indicating where in the code a function call occurrs.
	*/
	void check_gl_error (std::string from);

	//! Access function for the transfer function.
	/*!
		\return The transfer function used by this widget.
	*/
	TFTexture* transferFunction();

protected:

	//! Initialises OpenGL.
	/*!
		Overwritten from QGLWidget.
		Initialises GLEW and GLSL. Most of the actual initialisation happens in setShaders().
		\sa setShaders(), paintGL(), resizeGL()
	*/
	void initializeGL(void);

	//! Resizes the GL viewport on widget resize.
	/*!
		Overwritten from QGLWidget.
		\param width New viewport width.
		\param height New viewport height.
		\sa initializeGL(), paintGL()
	*/
    void resizeGL(int width, int height);

	//! Renders the scene.
	/*!
		Most of the work happens here. Displays the slice or direct volume render of the data set using the set transfer function.
		\sa drawBoundingBox(), initializeGL(), resizeGL()
	*/
    void paintGL();

	//! Calls for update of the widget.
	/*!
		Is called whenever the timer sends a signal to timeOutSlot().
		Calls updateGL(), which in turn automatically calls for a redraw via paintGL().
		\sa timeOutSlot(), paintGL()
	*/
    void timeOut();

protected slots:

	//! Slot for the peroidic timer signal.
	/*!
		Calls timeOut(), which in turn calls for a redraw of this widget.
		/sa timeOut(), paintGL()
	*/
    void timeOutSlot();

	//! Slot to set the x coordinate.
	/*!
		\param x The new x coordinate.
	*/
    void setX(int x);

	//! Slot to set the y coordinate.
	/*!
		\param y The new y coordinate.
	*/
    void setY(int y);

	//! Slot to set the z coordinate.
	/*!
		\param z The new z coordinate.
	*/
	void setZ(int z);

	//! Slot to toggle between 2d and 3d view.
	/*!
		\param v Whether to use slicing (true) or direct volume rendering (false).
	*/
	void setView(bool v);

private:

	//! View mode of the widget.
	/*!
		Slicing if true, direct volume rendering if false.
	*/
	bool viewMode;

	//! The timer taking care of continual updates to this widget.
    QTimer *timer;

	//! The volume data.
	Volume g_Volume;

	//! The x coordinate of the widget.
	/*!
		Determines which slice to show in x direction in slicing mode, and the rotation in x direction in direct volume render mode.
	*/
	float g_fCoordX;

	//! The y coordinate of the widget.
	/*!
		Determines which slice to show in y direction in slicing mode, and the rotation in y direction in direct volume render mode.
	*/
	float g_fCoordY;

	//! The z coordinate of the widget.
	/*!
		Determines which slice to show in z direction in slicing mode, and the rotation in z direction in direct volume render mode.
	*/
	float g_fCoordZ;

	//! The OpenGL id for the vertex shader for slicing.
	GLuint vertexShader;

	//! The OpenGL id for the fragment shader for slicing.
	GLuint fragmentShader;

	//! The OpenGL id for the vertex shader for ray casting.
	GLuint rayShaderV;

	//! The OpenGL id for the fragment shader for ray casting.
	GLuint rayShaderF;

	//! The OpenGL id for the transfer function shader program.
	GLuint transferProgram;

	//! The OpenGL id for the ray casting shader program.
	GLuint rayProgram;

	//! The OpenGL id for the frame buffer object used for rendering the transfer function texture.
	GLuint fbo_transfer;

	//! The OpenGL id for the frame buffer object used for rendering the front faces cube texture for ray casting.
	GLuint fbo_front;

	//! The OpenGL id for the frame buffer object used for rendering the back faces cube texture for ray casting.
	GLuint fbo_back;

	//! The OpenGL id for the volume data texture.
	GLuint dataTexture;

	//! The OpenGL id for the gradient texture.
	GLuint gradientTexture;

	//! The OpenGL id for the transfer function texture.
	GLuint transferTexture;

	//! The OpenGL id for the front faces cube texture.
	GLuint tex_front;

	//! The OpenGL id for the back faces cube texture.
	GLuint tex_back;

	//! The OpenGL id for the front faces cube render buffer.
	GLuint render_front;

	//! The OpenGL id for the back faces cube render buffer.
	GLuint render_back;

	//! The OpenGL id for the transfer function render buffer.
	GLuint depth_rb;

	//! The transfer texture used by this widget.
	TFTexture *tf;

	//! Utility function to get the next higher power of two for any given integer.
	/*!
		Used to generate an appropriately sized texture for an arbitrarily large volume dataset if non-power-of-two textures are not supported.
		\param iNumber The integer you want to know the next higher power of two for.
	*/
	const int GetNextPowerOfTwo(const int iNumber);

	//! Does most of initialisation work.
	/*!
		Generates all the required OpenGL objects (textures, buffers, shaders, etc.).
		Loads all the required shader files.
		\sa readShader(), initializeGL()
	*/
	void setShaders(void);

	//! Reads a shader file into a string.
	/*!
		\param fn The filename of the shader file.
		\return The character pointer containing the shader code.
	*/
	char* readShader(char *fn);

	//! Renders the bounding box for the volume data.
	/*!
		Used to generate the cube textures for ray casting.
		\param fbo The frame buffer to render the bounding box into.
	*/
	void drawBoundingBox(GLuint& fbo);
};

#endif // GLWIDGET_H
