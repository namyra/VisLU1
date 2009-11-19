#include "glwidget.h"

#include <qapplication.h>
#include <qtimer.h>

GLWidget::GLWidget(int timerInterval, QWidget *parent) : QGLWidget(parent)
{
    rtri = blue = gquad = bquad = 0.0f;
    green = 0.5f;
    rdir = red = rquad = 1.0f;

    if(timerInterval == 0)
        timer = 0;
    else
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
        timer->start(timerInterval);
    }

	g_zIncrement = 0.5f;
	g_fCoordZ = 0.5f;
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 600);
}

void GLWidget::check_gl_error (std::string from) {
	std::cout << from + " - ";
	char buf[100];
	GLenum i;
	const char *e;
	qDebug() << "";
	qDebug() << from.c_str();
	switch ((i = glGetError())) {
		case GL_NO_ERROR: std::cout << "no error"; break;
		case GL_INVALID_ENUM:          qDebug() << "invalid enum";      break;
		case GL_INVALID_VALUE:         qDebug() <<  "invalid value";     break;
		case GL_INVALID_OPERATION:     qDebug() <<  "invalid operation"; break;
		case GL_STACK_OVERFLOW:        qDebug() <<  "stack overflow";    break;
		case GL_STACK_UNDERFLOW:       qDebug() <<  "stack underflow";   break;
		case GL_OUT_OF_MEMORY:         qDebug() <<  "out of memory";     break;
		default:
		qDebug() << "unknown error"; break;
	}
}

void GLWidget::setShaders(void) {

	glGenFramebuffersEXT(1, &fbo);
	check_gl_error("generate FBO");
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	check_gl_error("bind FBO");

	glGenRenderbuffersEXT(1, &depth_rb);
	check_gl_error("generate renderbuffer");
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depth_rb);
	check_gl_error("bind renderbuffer");
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 256, 256);
	check_gl_error("buffer storage");
	
	glGenTextures(1, &transferTexture);
	check_gl_error("generate transfer texture");
	glBindTexture(GL_TEXTURE_2D, transferTexture);
	check_gl_error("bind transfer texture");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	check_gl_error("teximage2d transfer texture");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	check_gl_error("tex parameters filter");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	check_gl_error("tex parameters wrapping");

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, transferTexture, 0);
	check_gl_error("framebuffer");
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depth_rb);
	check_gl_error("renderbuffer");

	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		qDebug() << "the FBO is not complete";
		std::cout<<"The FBO is not complete"<<std::endl;
		std::cout<<"Error: "<<glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)<<std::endl;
	}

	glViewport(0, 0, 256, 256);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 256.0, 0.0, 256.0, -1.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	check_gl_error("projection change");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float border = 64;
	float border2 = 148;
	glBegin(GL_QUADS);
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex3f(0,0,-1);
		glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
		glVertex3f(border,0,-1);
		glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
		glVertex3f(border,256,-1);
		glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
		glVertex3f(0,256,-1);
		
		glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
		glVertex3f(border,0,-1);
		glColor4f(0.2f, 0.6f, 1.0f, 1.0f);
		glVertex3f(border2,0,-1);
		glColor4f(0.2f, 0.6f, 1.0f, 1.0f);
		glVertex3f(border2,256,-1);
		glColor4f(1.0f, 0.0f, 0.0f, 0.8f);
		glVertex3f(border,256,-1);

		glColor4f(0.2f, 0.6f, 1.0f, 1.0f);
		glVertex3f(border2,0,-1);
		glColor4f(0.3f, 1.0f, 0.4f, 1.0f);
		glVertex3f(256,0,-1);
		glColor4f(0.3f, 1.0f, 0.4f, 1.0f);
		glVertex3f(256,256,-1);
		glColor4f(0.2f, 0.6f, 1.0f, 1.0f);
		glVertex3f(border2,256,-1);
	glEnd();
	check_gl_error("transfer texture quad");

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	check_gl_error("bind FBO");

	glViewport(0, 0, width(), height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	check_gl_error("projection change back");

	GLcharARB* fragmentSource;
	GLcharARB* vertexSource;

	// Create Shader And Program Objects
	transferProgram = glCreateProgramObjectARB();
	vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
	check_gl_error("create shaders");

	vertexSource = "void main() {gl_TexCoord[0] = gl_MultiTexCoord0; gl_TexCoord[1] = gl_MultiTexCoord1;gl_Position = ftransform();}";
	fragmentSource = "uniform sampler3D slice_sampler;uniform sampler2D transfer_sampler;void main(){vec4 intensity = texture3D(slice_sampler,vec3(gl_TexCoord[0]) );if (intensity.x < 0.1) {discard;} else { gl_FragColor = texture2D(transfer_sampler,vec2(intensity.x, intensity.y));}}";

	glShaderSource(fragmentShader, 1, const_cast<const GLchar**>(&fragmentSource), 0);
	glShaderSourceARB(vertexShader, 1, const_cast<const GLchar**>(&vertexSource), 0);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	check_gl_error("compile shaders");

	transferProgram = glCreateProgram();
	check_gl_error("create program");
	glAttachShader(transferProgram, vertexShader);
	glAttachShader(transferProgram, fragmentShader);
	check_gl_error("attach shaders");
	glLinkProgram(transferProgram);
	check_gl_error("link program");

	GLint compiledv, compiledf, linked;

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiledv);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiledf);

	glGetProgramiv(transferProgram, GL_LINK_STATUS, &linked);

	if (!compiledv)
		qDebug() << "vertex shader not compiled";
	if (!compiledf)
		qDebug() << "fragment shader not compiled";

	if (!linked)
		qDebug() << "not linked ";

	GLchar log[40960];
	GLint len;
	glGetShaderInfoLog(vertexShader, 40960, &len, log); 
	std::cout << log << std::endl;

	glGetShaderInfoLog(fragmentShader, 40960, &len, log); 
	std::cout << log << std::endl;

	glUseProgram(transferProgram);
	check_gl_error("use program");
}

void GLWidget::initializeGL()
{
	glViewport(0, 0, width(), height());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Initialize GLEW
	std::cout << "- Initializing GLEW ..." << std::endl;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		// glewInit failed, something is seriously wrong
		std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}
	std::cout << "- GLEW initialized." << std::endl << std::endl;

	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("Not totally ready :( \n");
		exit(1);
	}

    //glShadeModel(GL_SMOOTH);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	setShaders();
}

void GLWidget::resizeGL(int width, int height)
{
    height = height ? height : 1;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLWidget::paintGL()
{
	static Timer timTimer;
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glPushMatrix();

	if (dataTexture) //texture is created
	{
		if(GLEW_EXT_texture3D) //graphics hw supports 3d textures
		{
			glEnable(GL_BLEND);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_3D, dataTexture);
			glEnable(GL_TEXTURE_3D);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, transferTexture);
			glEnable(GL_TEXTURE_2D);

			//draw quad with texture taken from 3d volume
			glUseProgram(transferProgram);

			glUniform1iARB(glGetUniformLocation(transferProgram, "slice_sampler"), 0);
			glUniform1iARB(glGetUniformLocation(transferProgram, "transfer_sampler"), 1);

			//glEnable(GL_COLOR);
			//glColor4f(1.0, 0.0, 0.0, 1.0);
			glTranslatef(-0.5, 0, 0);
			glBegin(GL_QUADS);
			glTexCoord3f(0.0f, 0.0f, g_fCoordZ);
			glVertex3f(-0.5,-0.5,-1);
			glTexCoord3f(1.0f, 0.0f, g_fCoordZ);
			glVertex3f(0.5,-0.5,-1);
			glTexCoord3f(1.0f, 1.0f, g_fCoordZ);
			glVertex3f(0.5,0.5,-1);
			glTexCoord3f(0.0f, 1.0f, g_fCoordZ);
			glVertex3f(-0.5,0.5,-1);
			glEnd();

			glTranslatef(1.0, 0, 0);
			glBegin(GL_QUADS);
			glTexCoord3f(0.0f, g_fCoordZ, 0.0f);
			glVertex3f(-0.5,-0.5,-1);
			glTexCoord3f(1.0f, g_fCoordZ, 0.0f);
			glVertex3f(0.5,-0.5,-1);
			glTexCoord3f(1.0f, g_fCoordZ, 1.0f);
			glVertex3f(0.5,0.5,-1);
			glTexCoord3f(0.0f, g_fCoordZ, 1.0f);
			glVertex3f(-0.5,0.5,-1);
			glEnd();

			glTranslatef(-1.0, -1.0, 0);
			glBegin(GL_QUADS);
			glTexCoord3f(g_fCoordZ, 0.0f, 0.0f);
			glVertex3f(-0.5,-0.5,-1);
			glTexCoord3f(g_fCoordZ, 1.0f, 0.0f);
			glVertex3f(0.5,-0.5,-1);
			glTexCoord3f(g_fCoordZ, 1.0f, 1.0f);
			glVertex3f(0.5,0.5,-1);
			glTexCoord3f(g_fCoordZ, 0.0f, 1.0f);
			glVertex3f(-0.5,0.5,-1);
			glEnd();


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, transferTexture);
			glEnable(GL_TEXTURE_2D);
			glUseProgram(0);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			//glEnable(GL_COLOR);
			glColor4f(1.0, 0.0, 0.0, 0.5);
			glTranslatef(-0.5, 0, 0);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5,-0.5,-1);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.5,-0.5,-1);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5,0.5,-1);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.5,0.5,-1);
			glEnd();

		}
		else // 3d texture not supported
		{
			glEnable(GL_TEXTURE_2D);
			//draw quad with 2d texture
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(-0.5,-0.5,-1);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(0.5,-0.5,-1);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(0.5,0.5,-1);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(-0.5,0.5,-1);
			glEnd();
		}
	}

	glPopMatrix();
	//glutSwapBuffers();

	//estimate fps
	const double dTimePerFrame = double(timTimer);
	const double dFramesPerSecond = dTimePerFrame > 0.0 ? 1.0 / dTimePerFrame : 0.0;
	timTimer.start();
	printf("- %06.2f frames/second \r",dFramesPerSecond);
}

void GLWidget::setColor(int color)
{
    switch(color)
    {
    case 0:
        rquad = 1.0f;
        gquad = bquad = 0.0f;
        break;

    case 1:
        gquad = 1.0f;
        rquad = bquad = 0.0f;
        break;

    case 2:
        bquad = 1.0f;
        rquad = gquad = 0.0f;
    }
}

void GLWidget::setDirection(int direction)
{
    if(direction == 0)
        rdir = 1.0f;
    else
        rdir = -1.0f;
}

void GLWidget::setRed(int intensity)
{
    red = (float)intensity / (255.0f * 16.0f);
}

void GLWidget::setGreen(int intensity)
{
    green = (float)intensity / (255.0f * 16.0f);
}

void GLWidget::setBlue(int intensity)
{
    blue = (float)intensity / (255.0f * 16.0f);
}

void GLWidget::timeOut()
{
	if (g_Volume.GetDepth()>1)
	{
		//update the slice z-coordinate
		g_fCoordZ += g_zIncrement;
		if (g_fCoordZ > 1.f)
		{
			g_zIncrement*=-1.f;
			g_fCoordZ = 1.f;
		}
		else if (g_fCoordZ<0.f)
		{
			g_zIncrement *=-1.f;
			g_fCoordZ = 0.f;
		}
	}

    updateGL();
}

void GLWidget::timeOutSlot()
{
    timeOut();
}

const int GLWidget::GetNextPowerOfTwo(const int iNumber) {
	int iPowerOfTwo = 1;

	while (iPowerOfTwo < iNumber)
		iPowerOfTwo = iPowerOfTwo * 2;

	return iPowerOfTwo;
}

void GLWidget::loadDataSet(std::string fileName) {

	//load volume
	g_Volume.load(fileName);

	//create texture
	glGenTextures(1, &dataTexture);
	GLenum glError = GL_NO_ERROR;
	glError = glGetError();
	if (glError != GL_NO_ERROR) {
		std::cerr<<"Error generating texture: "<<gluErrorString(glError)<<std::endl;
	} else if (dataTexture) {
		if(GLEW_EXT_texture3D) {
			g_zIncrement = 1.f / float(g_Volume.GetDepth()-1);
			//bind texture
			glBindTexture(GL_TEXTURE_3D, dataTexture);
				
			glError = glGetError();
			if (glError != GL_NO_ERROR) {
				std::cerr<<"Could not bind texture: "<<gluErrorString(glError)<<std::endl;
			} else {
				int iTextureWidth=g_Volume.GetWidth();
				int iTextureHeight=g_Volume.GetHeight();
				int iTextureDepth=g_Volume.GetDepth();

				if (!GLEW_ARB_texture_non_power_of_two) {
					iTextureWidth = GetNextPowerOfTwo(g_Volume.GetWidth());
					iTextureHeight = GetNextPowerOfTwo(g_Volume.GetHeight());
					iTextureDepth = GetNextPowerOfTwo(g_Volume.GetDepth());				 
				}

				//upload texture
				glTexImage3D(GL_TEXTURE_3D_EXT, 0, GL_LUMINANCE, iTextureWidth, iTextureHeight, iTextureDepth, 0, GL_LUMINANCE, GL_FLOAT, NULL);
				glTexSubImage3D(GL_TEXTURE_3D_EXT, 0, 0, 0, 0, g_Volume.GetWidth(), g_Volume.GetHeight(), g_Volume.GetDepth(), GL_LUMINANCE, GL_FLOAT, (void *) g_Volume.Get());
					
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glError = glGetError();
				if (glError != GL_NO_ERROR) {
					std::cerr<<"Could not upload texture: "<<gluErrorString(glError)<<std::endl;
				}
			}
		}
		else {
			std::cout<<"3D textures not supported!"<<std::endl;

			//bind texture
			glBindTexture(GL_TEXTURE_2D, dataTexture);

			glError = glGetError();
			if (glError != GL_NO_ERROR) {
				std::cerr<<"Could not bind texture: "<<gluErrorString(glError)<<std::endl;
			} else {
				int iTextureWidth=g_Volume.GetWidth();
				int iTextureHeight=g_Volume.GetHeight();
				int iTextureDepth=g_Volume.GetDepth();

				if (!GLEW_ARB_texture_non_power_of_two) {
					iTextureWidth = GetNextPowerOfTwo(g_Volume.GetWidth());
					iTextureHeight = GetNextPowerOfTwo(g_Volume.GetHeight());
					iTextureDepth = GetNextPowerOfTwo(g_Volume.GetDepth());				 
				}

				//upload 
				glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, iTextureWidth, iTextureHeight, 0, GL_LUMINANCE, GL_FLOAT, NULL);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, g_Volume.GetWidth(), g_Volume.GetHeight(), GL_LUMINANCE, GL_FLOAT, (void *) &(g_Volume.Get(0,0, int(float(g_Volume.GetDepth())*0.5f) )));
					
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glError = glGetError();
				if (glError != GL_NO_ERROR) {
					std::cerr<<"Could not upload texture: "<<gluErrorString(glError)<<std::endl;
				}
			}
		}
	}
	std::cout << std::endl;
}
