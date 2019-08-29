
#include <iostream>


#include <QtWidgets/qmenubar.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qstatusbar.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qdockwidget.h>


#include "GLWindow.h"

#include "MainWindow.h"


MainWindow::MainWindow()
{
	//create the actions and Menu: first create actions, then insert to menus
	createActions();
	createMenus();
	createToolbar();
	createDockWindows();	//create dock windows

	//message
	QString message = tr("A context menu is available by right-clicking");
	statusBar()->showMessage(message);		//setting statusbar

	//set property of Qmainwindow
	setWindowTitle(tr("Example"));
	setMinimumSize(700, 700);	//setting the minimum size so window cannot be resized than the given one

	//create a widget
	setCentralWidget(ViewWindow = new GLWindow(&modelMVC,this));	//set the opengl widget to central widget
		
}

void MainWindow::createActions()
{
	//create a action means "New" and set it to standard shortcut
	const QIcon newIcon = QIcon("images/newIcon.jpg");			//set the icon 
	newAct = new QAction(newIcon, tr("&New"), this);				//create a new action and parent it with QMainWindow
	newAct->setShortcuts(QKeySequence::New);			//create a std shortcut
	newAct->setStatusTip(tr("Create a new file"));		//show a message on the status bar	
	connect(newAct, &QAction::triggered, this, &MainWindow::newFile);


	openAct = new QAction(tr("&Open"), this);				//create a new action and parent it with QMainWindow
	openAct->setShortcuts(QKeySequence::Open);			//create a std shortcut
	openAct->setStatusTip(tr("Open a file"));		//show a message on the status bar


	saveAct = new QAction(tr("&Save"), this);				//create a new action and parent it with QMainWindow
	saveAct->setShortcuts(QKeySequence::Save);			//create a std shortcut
	saveAct->setStatusTip(tr("Save the current file"));		//show a message on the status bar

	exitAct = new QAction(tr("E&xit"), this);				//create a new action and parent it with QMainWindow
	exitAct->setShortcuts(QKeySequence::Quit);			//create a std shortcut
	exitAct->setStatusTip(tr("Exit the application"));		//show a message on the status bar


	//action for view Menu
	propertyAct = new QAction(tr("&Property"), this);
	propertyAct->setStatusTip(tr("Show the property page"));		//show a message on the status bar

	const QIcon IconTrackBall = QIcon("images/trackball.png");			//set the icon 
	trackballAct = new QAction(IconTrackBall, tr("&Trackball"), this);		//create a new action that enable the trackball
	trackballAct->setStatusTip(tr("Enable trackball"));		
	connect(trackballAct, &QAction::triggered, this, &MainWindow::enableTrackBall);

}

void MainWindow::enableTrackBall()
{
	//enable the trackball
	modelMVC.enableTrackBallFlag();
	//draw after this event is updated
	ViewWindow->repaint();
}

void MainWindow::createMenus()
{
	//create a menu and add the action 
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(newAct);
	fileMenu->addAction(openAct);
	fileMenu->addAction(saveAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);


	//create viewMenu
	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(propertyAct);	//add property page to 'view'
	viewMenu->addAction(trackballAct);	//add trackball action to 'view'

}

void MainWindow::createToolbar()
{
	fileToolBar = addToolBar(tr("File"));
	fileToolBar->addAction(newAct);			//add new to toolbar

	fileToolBar->addAction(trackballAct);	//add the trackball to the toolbar
	
}
    
void MainWindow::createDockWindows()
{
	//create a dock window and attach with main window
	QDockWidget* dock = new QDockWidget(tr("Properties"), this);
	dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);		//area of docking in the main window
	
	//list of item display in the dock
	itemsList = new QListWidget(dock);
	itemsList->addItems(QStringList()
		<< "Hi add some property"
		);
	dock->setWidget(itemsList);
	addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::newFile()
{
	std::cout << "Hi" << std::endl;
	

}
