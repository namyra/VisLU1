#include "glwidget.h"

#include <qapplication.h>
#include <qtimer.h>

GLWidget::GLWidget(int timerInterval, QWidget *parent) : QGLWidget(parent)
{
    if(timerInterval == 0)
        timer = 0;
    else
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(timeOutSlot()));
        timer->start(timerInterval);
    }

	g_fCoordX = g_fCoordY = g_fCoordZ = 0.0f;

	tf = new TFTexture(this);

	updateGL();
}

GLWidget::~GLWidget() {
	check_gl_error("deleting textures...");
	glDeleteTextures(1,&transferTexture);
	glDeleteTextures(1,&dataTexture);
	glDeleteTextures(1,&tex_front);
	glDeleteTextures(1,&tex_back);
	check_gl_error("...deleting textures");
}

QSize GLWidget::sizeHint() const
{
    return QSize(800, 600);
}

/* OpenGL debugging helper function */
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

TFTexture* GLWidget::transferFunction()
{
	return tf;
}

char* GLWidget::readShader(char *fn) {

	FILE *fp;
	char *content = NULL;
	int count=0;

	if (fn != NULL) {

		fp = fopen(fn,"rt");

		if (fp != NULL) {
										      
			fseek(fp, 0, SEEK_END);
        			count = ftell(fp);
        			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}

			fclose(fp);
		}
	}
	return content;
}

void GLWidget::setShaders(void) {

	glGenFramebuffersEXT(1, &fbo_front);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_front);
	glGenRenderbuffersEXT(1, &render_front);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, render_front);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 2048, 2048);
	glGenTextures(1, &tex_front);
	glBindTexture(GL_TEXTURE_2D, tex_front);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 2048, 2048, 0, GL_BGRA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_front, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_front);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		qDebug() << "the FBO is not complete";
		std::cout<<"The FBO is not complete"<<std::endl;
		std::cout<<"Error: "<<glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)<<std::endl;
	}
	glGenFramebuffersEXT(1, &fbo_back);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_back);
	glGenRenderbuffersEXT(1, &render_back);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, render_back);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, 2048, 2048);
	glGenTextures(1, &tex_back);
	glBindTexture(GL_TEXTURE_2D, tex_back);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 2048, 2048, 0, GL_BGRA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_back, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, render_back);
	if(glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)!=GL_FRAMEBUFFER_COMPLETE_EXT) {
		qDebug() << "the FBO is not complete";
		std::cout<<"The FBO is not complete"<<std::endl;
		std::cout<<"Error: "<<glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT)<<std::endl;
	}

	glGenFramebuffersEXT(1, &fbo_transfer);
	check_gl_error("generate FBO");
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo_transfer);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 256, 256, 0, GL_BGRA, GL_FLOAT, NULL);
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
		qDebug() << "Error: " << glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	}

	tf->setFBO(fbo_transfer, transferTexture);
	tf->generate();

	glViewport(0, 0, width(), height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
	rayShaderV = glCreateShaderObjectARB(GL_VERTEX_SHADER);
	rayShaderF = glCreateShaderObjectARB(GL_FRAGMENT_SHADER);
	check_gl_error("create shaders");

	GLcharARB* fragmentSource;
	GLcharARB* vertexSource;
	GLcharARB* raySourceV;
	GLcharARB* raySourceF;

	vertexSource = readShader("GLSL/slice.vert");
	fragmentSource = readShader("GLSL/slice.frag");
	raySourceV = readShader("GLSL/raycast.vert");
	raySourceF = readShader("GLSL/raycast.frag");

	glShaderSource(fragmentShader, 1, const_cast<const GLchar**>(&fragmentSource), 0);
	glShaderSource(vertexShader, 1, const_cast<const GLchar**>(&vertexSource), 0);
	glShaderSource(rayShaderV, 1, const_cast<const GLchar**>(&raySourceV), 0);
	glShaderSource(rayShaderF, 1, const_cast<const GLchar**>(&raySourceF), 0);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	glCompileShader(rayShaderV);
	glCompileShader(rayShaderF);
	check_gl_error("compile shaders");

	transferProgram = glCreateProgram();
	check_gl_error("create program");
	glAttachShader(transferProgram, vertexShader);
	glAttachShader(transferProgram, fragmentShader);
	check_gl_error("attach shaders");
	glLinkProgram(transferProgram);
	check_gl_error("link program");

	rayProgram = glCreateProgram();
	check_gl_error("create program");
	glAttachShader(rayProgram, rayShaderV);
	glAttachShader(rayProgram, rayShaderF);
	check_gl_error("attach shaders");
	glLinkProgram(rayProgram);
	check_gl_error("link program");

	GLint compiledv, compiledf, linked;

	glGetShaderiv(rayShaderV, GL_COMPILE_STATUS, &compiledv);
	glGetShaderiv(rayShaderF, GL_COMPILE_STATUS, &compiledf);

	glGetProgramiv(rayProgram, GL_LINK_STATUS, &linked);

	if (!compiledv)
		qDebug() << "vertex shader not compiled";
	if (!compiledf)
		qDebug() << "ray shader not compiled";

	if (!linked)
		qDebug() << "not linked ";

	GLchar log[40960];
	GLint len;
	glGetShaderInfoLog(vertexShader, 40960, &len, log); 
	std::cout << log << std::endl;

	glGetShaderInfoLog(fragmentShader, 40960, &len, log); 
	std::cout << log << std::endl;

	glGetShaderInfoLog(rayShaderV, 40960, &len, log); 
	std::cout << log << std::endl;
	qDebug() << log;

	glGetShaderInfoLog(rayShaderF, 40960, &len, log); 
	std::cout << log << std::endl;
	qDebug() << log;

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
		qDebug() << "Error initializing GLEW";
		exit(1);
	}
	std::cout << "- GLEW initialized." << std::endl << std::endl;

	if (glewIsSupported("GL_VERSION_2_0")) {
		printf("Ready for OpenGL 2.0\n");
		qDebug() << "Ready for OpenGL 2.0";
	}else {
		printf("OpenGL 2.0 not supported\n");
		qDebug() << "OpenGL 2.0 not supported";
		exit(1);
	}

	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader) {
		printf("Ready for GLSL\n");
		qDebug() << "Ready for GLSL";
	} else {
		printf("Not totally ready :( \n");
		qDebug() << "Not totally ready";
		exit(1);
	}

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

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

void GLWidget::drawBoundingBox(GLuint& fbo) {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	double x = static_cast<double>(g_Volume.GetWidth());
	double y = static_cast<double>(g_Volume.GetHeight());
	double z = static_cast<double>(g_Volume.GetDepth());

	double m = std::max(x, std::max(y, z));
	
	x /= m;
	y /= m;
	z /= m;

	//x = y = z = 1;
	
	glPushMatrix();
	
	gluLookAt	(0.0, 0.0, 4.0,		// position of eye
				0.0, 0.0, 0.0,		// at, where pointing at
				0.0, 1.0, 0.0);		// up vector of the camera

	glRotatef(g_fCoordX*360, 1.0, 0.0, 0.0);
	glRotatef(g_fCoordY*360, 0.0, 1.0, 0.0);
	glRotatef(g_fCoordZ*360, 0.0, 0.0, 1.0);

	glBegin(GL_QUADS);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex3f(x, -y, -z);
		glColor4f(0.0, 0.0, 0.0, 1.0);
		glVertex3f(-x, -y, -z);
		glColor4f(0.0, 1.0, 0.0, 1.0);
		glVertex3f(-x, y, -z);
		glColor4f(1.0, 1.0, 0.0, 1.0);
		glVertex3f(x, y, -z);

		glColor4f(0.0, 0.0, 0.0, 1.0);
		glVertex3f(-x, -y, -z);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex3f(x, -y, -z);
		glColor4f(1.0, 0.0, 1.0, 1.0);
		glVertex3f(x, -y, z);
		glColor4f(0.0, 0.0, 1.0, 1.0);
		glVertex3f(-x, -y, z);

		glColor4f(0.0, 0.0, 0.0, 1.0);
		glVertex3f(-x, -y, -z);
		glColor4f(0.0, 0.0, 1.0, 1.0);
		glVertex3f(-x, -y, z);
		glColor4f(0.0, 1.0, 1.0, 1.0);
		glVertex3f(-x, y, z);
		glColor4f(0.0, 1.0, 0.0, 1.0);
		glVertex3f(-x, y, -z);

		glColor4f(0.0, 0.0, 1.0, 1.0);
		glVertex3f(-x, -y, z);
		glColor4f(1.0, 0.0, 1.0, 1.0);
		glVertex3f(x, -y, z);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glVertex3f(x, y, z);
		glColor4f(0.0, 1.0, 1.0, 1.0);
		glVertex3f(-x, y, z);

		glColor4f(1.0, 1.0, 0.0, 1.0);
		glVertex3f(x, y, -z);
		glColor4f(0.0, 1.0, 0.0, 1.0);
		glVertex3f(-x, y, -z);
		glColor4f(0.0, 1.0, 1.0, 1.0);
		glVertex3f(-x, y, z);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glVertex3f(x, y, z);

		glColor4f(1.0, 0.0, 1.0, 1.0);
		glVertex3f(x, -y, z);
		glColor4f(1.0, 0.0, 0.0, 1.0);
		glVertex3f(x, -y, -z);
		glColor4f(1.0, 1.0, 0.0, 1.0);
		glVertex3f(x, y, -z);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		glVertex3f(x, y, z);
	glEnd();

	glPopMatrix();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void GLWidget::paintGL()
{
	if (dataTexture) //texture is created
	{
		if(GLEW_EXT_texture3D) //graphics hw supports 3d textures
		{
			if(!viewMode)
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glViewport(0, 0, float(width()), float(height()));
				gluPerspective(40.0, float(height())/float(width()), 0.01, 50.0);
				glMatrixMode(GL_MODELVIEW);
				glEnable(GL_DEPTH_TEST);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);

				glUseProgram(0);
				glDisable(GL_TEXTURE_3D);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_LIGHTING);

				//static Timer timTimer;

				glEnable(GL_CULL_FACE);

				glCullFace(GL_BACK);
				drawBoundingBox(fbo_front);
				glCullFace(GL_FRONT);
				drawBoundingBox(fbo_back);

				glClearColor(0.0, 0.0, 0.0, 0.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

				glDisable(GL_CULL_FACE);

				double x = static_cast<double>(g_Volume.GetWidth());
				double y = static_cast<double>(g_Volume.GetHeight());
				double z = static_cast<double>(g_Volume.GetDepth());

				double m = std::max(x, std::max(y, z));
				
				x /= m;
				y /= m;
				z /= m;

				x = y = z = 1;

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, dataTexture);
				glEnable(GL_TEXTURE_3D);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, transferTexture);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, tex_front);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, tex_back);
				glEnable(GL_TEXTURE_2D);

				glUseProgram(rayProgram);

				glUniform1iARB(glGetUniformLocation(rayProgram, "tex_intensity"), 0);
				glUniform1iARB(glGetUniformLocation(rayProgram, "tex_transfer"), 1);
				glUniform1iARB(glGetUniformLocation(rayProgram, "tex_entry"), 2);
				glUniform1iARB(glGetUniformLocation(rayProgram, "tex_exit"), 3);

				glMatrixMode (GL_MODELVIEW);
				glPushMatrix ();
				glLoadIdentity ();
				glMatrixMode (GL_PROJECTION);
				glPushMatrix ();
				glLoadIdentity ();
				glBegin (GL_QUADS);
					glTexCoord2f(0.0f, 0.0f);
					glVertex3i(-1, -1, 0.2);
					glTexCoord2f(0.0f, float(height())/2048.0);
					glVertex3i(1, -1, 0.2);
					glTexCoord2f(float(width())/2048.0, float(height())/2048.0);
					glVertex3i(1, 1, 0.2);
					glTexCoord2f(float(width())/2048.0, 0.0f);
					glVertex3i(-1, 1, 0.2);
				glEnd ();

				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			} else {
				glClearColor(0.0, 0.0, 0.0, 0.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
				glPushMatrix();

				 glEnable(GL_BLEND);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_3D, dataTexture);
				glEnable(GL_TEXTURE_3D);
				 
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, transferTexture);
				glEnable(GL_TEXTURE_2D);
				 
				glUseProgram(transferProgram);
				 
				glUniform1iARB(glGetUniformLocation(transferProgram, "slice_sampler"), 0);
				glUniform1iARB(glGetUniformLocation(transferProgram, "transfer_sampler"), 1);
 
				glMatrixMode (GL_MODELVIEW);
				glPushMatrix ();
				glLoadIdentity ();
				glMatrixMode (GL_PROJECTION);
				glPushMatrix ();
				glLoadIdentity ();

				glTranslatef(-0.5, 0.5, 0);
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
				glTexCoord3f(0.0f, g_fCoordY, 0.0f);
				glVertex3f(-0.5,-0.5,-1);
				glTexCoord3f(1.0f, g_fCoordY, 0.0f);
				glVertex3f(0.5,-0.5,-1);
				glTexCoord3f(1.0f, g_fCoordY, 1.0f);
				glVertex3f(0.5,0.5,-1);
				glTexCoord3f(0.0f, g_fCoordY, 1.0f);
				glVertex3f(-0.5,0.5,-1);
				glEnd();
				 
				glTranslatef(-1.0, -1.0, 0);
				glBegin(GL_QUADS);
				glTexCoord3f(g_fCoordX, 0.0f, 0.0f);
				glVertex3f(-0.5,-0.5,-1);
				glTexCoord3f(g_fCoordX, 1.0f, 0.0f);
				glVertex3f(0.5,-0.5,-1);
				glTexCoord3f(g_fCoordX, 1.0f, 1.0f);
				glVertex3f(0.5,0.5,-1);
				glTexCoord3f(g_fCoordX, 0.0f, 1.0f);
				glVertex3f(-0.5,0.5,-1);
				glEnd();

				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();

				glPopMatrix();
			}
		}
	}

	//estimate fps
	/*const double dTimePerFrame = double(timTimer);
	const double dFramesPerSecond = dTimePerFrame > 0.0 ? 1.0 / dTimePerFrame : 0.0;
	timTimer.start();
	printf("- %06.2f frames/second \r",dFramesPerSecond);*/

}

void GLWidget::setX(int x)
{
    g_fCoordX = (float)x / (255.0f * 16.0f);
}

void GLWidget::setY(int y)
{
    g_fCoordY = (float)y / (255.0f * 16.0f);
}

void GLWidget::setZ(int z)
{
    g_fCoordZ = (float)z / (255.0f * 16.0f);
}

void GLWidget::setView(bool v)
{
	viewMode = v;
}

void GLWidget::timeOut()
{
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
	int* histogram = g_Volume.getHistogram();
	tf->setHistogram(histogram);
	
	//create texture
	glGenTextures(1, &dataTexture);
	glGenTextures(1, &gradientTexture);
	GLenum glError = GL_NO_ERROR;
	glError = glGetError();
	if (glError != GL_NO_ERROR) {
		std::cerr<<"Error generating texture: "<<gluErrorString(glError)<<std::endl;
		check_gl_error("Error generating texture");
	} else if (dataTexture) {
		if(GLEW_EXT_texture3D) {
			//bind texture
			glBindTexture(GL_TEXTURE_3D, dataTexture);

				
			glError = glGetError();
			if (glError != GL_NO_ERROR) {
				std::cerr<<"Could not bind texture: "<<gluErrorString(glError)<<std::endl;
				qDebug() << "Could not bind texture: ";
				qDebug() << gluErrorString(glError);
			} else {
				int iTextureWidth=g_Volume.GetWidth();
				int iTextureHeight=g_Volume.GetHeight();
				int iTextureDepth=g_Volume.GetDepth();

				if (!GLEW_ARB_texture_non_power_of_two) {
					iTextureWidth = GetNextPowerOfTwo(g_Volume.GetWidth());
					iTextureHeight = GetNextPowerOfTwo(g_Volume.GetHeight());
					iTextureDepth = GetNextPowerOfTwo(g_Volume.GetDepth());				 
				}
				check_gl_error("GLEW_ARB_texture_non_power_of_two");

				//upload texture
				glTexImage3D(GL_TEXTURE_3D_EXT, 0, GL_LUMINANCE, iTextureWidth, iTextureHeight, iTextureDepth, 0, GL_LUMINANCE, GL_FLOAT, NULL);
				check_gl_error("glTexImage3D");
				glTexSubImage3D(GL_TEXTURE_3D_EXT, 0, 0, 0, 0, g_Volume.GetWidth(), g_Volume.GetHeight(), g_Volume.GetDepth(), GL_LUMINANCE, GL_FLOAT, (void *) g_Volume.Get());
				check_gl_error("glTexSubImage3D");

				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				check_gl_error("glTexParameteri FILTER");

				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				check_gl_error("glTexParameteri WRAP");

				glError = glGetError();
				if (glError != GL_NO_ERROR) {
					std::cerr<<"Could not upload texture: "<<gluErrorString(glError)<<std::endl;
					qDebug() << "Could not upload texture: ";
					qDebug() << gluErrorString(glError);
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

