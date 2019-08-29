#include <QtWidgets/qapplication.h>
#include <QtGui/qsurfaceformat.h>
#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);


	//request for double buffer and depth buffer
	QSurfaceFormat format;
	format.setDepthBufferSize(24);	//depth buffer size 24 bit
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);	//enable double buffer
	format.setProfile(QSurfaceFormat::CoreProfile);	//core profile
	QSurfaceFormat::setDefaultFormat(format);

	MainWindow mainWin;
	mainWin.show();

	return app.exec();

}