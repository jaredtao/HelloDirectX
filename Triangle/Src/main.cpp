#include "Application.h"
#include "MainWindow.h"
#include "TriangleRender.h"
using namespace Tao3D;

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR /*cmdParamarg*/, int /* cmdShow */)
{
	Application	   app;
	TriangleRender render;
	MainWindow	   window;
	app.setWindow(&window);
	app.setRender(&render);
	app.init(1024, 768, "Triangle");

	window.show();
	app.exec();
	return 0;
}
