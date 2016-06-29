
#include "Application.h"
#include "TestScene.h"

int main(int argc, char* argv[])
{
	Application app;

	app.AddScene("Test", new TestScene());
	app.SetActiveScene("Test");

	if (app.Init())
	{
		app.Run();
		app.Shutdown();
	}

	return 0;
}