#include <QtCore/qtimer.h>
#include <QtGui/qevent.h>
#include "SharedData.h"
#include "Trackball.h"
#include "TrackBallSphere.h"
#include "TestPlyModel.h"
#include "Frame.h"


#include "GLWindow.h"

GLWindow::GLWindow(SharedData* modelMVC, QWidget* parent): QOpenGLWidget(parent), 
screenWidth(0), screenHeight(0), theModel(modelMVC)
{
	//track ball
	trackball = new Trackball;
	trackBallMesh = new TrackBallSphere;

	//arrow
	frameMesh = new Frame;

	//3D ply object
	plyModel = new TestPlyModel;
	
}


GLWindow::~GLWindow()
{
	//delete the track ball mesh after use
	if (trackball)
		delete trackball;

	if (trackBallMesh)
		delete trackBallMesh;

	//delete the ply model after use
	if (plyModel)
		delete plyModel;

	if (frameMesh)
		delete frameMesh;
}

void GLWindow::initializeGL()
{
	//initialise glew at first before using opengl functions
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cout << "Failed to initialise the glew" << std::endl;
		exit(EXIT_FAILURE);
	}

	//opengl version and graphic card driver
	std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
	//graphic card
	std::cout << "GL renderer: " << glGetString(GL_RENDERER) << std::endl;

	//background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);	 //black color
	
	
	//load data to GPU
	sendDataToGPU();

	//set a timer
	myTimer = new QTimer;
	connect(myTimer, &QTimer::timeout, this, &GLWindow::myUpdate);
	myTimer->start(0);	//start immediately wihtout any delay

}

void GLWindow::sendDataToGPU()
{
	//load the trackball data
	trackBallMesh->loadData();

	//load data
	frameMesh->loadData();

	//load cube data
	plyModel->loadData();
}



void GLWindow::paintGL()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	//draw objects
	drawObjects();

	//check for any error
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::cerr << "OpenGL error: " << err << std::endl;
	}
	
}


void GLWindow::drawObjects()
{
	//render the trackball 
	if (theModel->queryTrackBallFlag())
		trackBallMesh->render(trackball->getTrackBallOrientation());

	//draw 3D model
	plyModel->render(trackball->getTrackBallOrientation());

	//draw frame
	frameMesh->render(trackball->getTrackBallOrientation());
}

void GLWindow::screenToClip(Vector2f& screenCoordinate)
{
	//y coordinate
	screenCoordinate.y = 1.0f - (screenCoordinate.y/ (0.5f * screenHeight) );
	screenCoordinate.x = (screenCoordinate.x / (0.5f * screenWidth)) - 1.0f;
}

void GLWindow::myUpdate()
{
	//draw again
	static unsigned int counter = 0;
	counter++;
	if (counter % 100 == 0)
	{
		repaint();
		counter = 0;
	}
}



void GLWindow::resizeGL(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
	float ratio = (float)screenWidth / screenHeight;

	//set the current aspect ratio to model
	plyModel->setAspectRatio(ratio);
	
	//set the current aspect ratio to frame
	frameMesh->setAspectRatio(ratio);

	//set the current aspect ratio to trackball
	trackBallMesh->setAspectRatio(ratio);

}

void GLWindow::mousePressEvent(QMouseEvent* buttonPress)
{
	if (buttonPress->buttons() & Qt::LeftButton)
	{
		//current mouse position
		QPoint mousePos = buttonPress->pos();

		//covert the mouse movement points from pixel->Normalized Coordinates
		lastMousePos = Vector2f(mousePos.x(), mousePos.y());
		screenToClip(lastMousePos);

		//set the position of trackball
		if (theModel->queryTrackBallFlag())
			trackball->setMousePosition(lastMousePos);
	}
	

	//ask opengl to render
	repaint();
}

void GLWindow::mouseMoveEvent(QMouseEvent* Move)
{
	//check whether the left button is pressed
	if (Move->buttons() & Qt::LeftButton)
	{
		//current mouse position
		QPoint mousePos = Move->pos();

		//convert the mouse movement points from pixel->Normalized Coordinates
		Vector2f currentMousePos(mousePos.x(), mousePos.y());
		screenToClip(currentMousePos);

		//if trackball is reuired 
		if (theModel->queryTrackBallFlag())
		{
			//send the two vectors to Trackball
			trackball->computeTrackballAxisAngle(currentMousePos);
		}

		//update mouse position
		lastMousePos = currentMousePos;
		
	}

	
	
}


