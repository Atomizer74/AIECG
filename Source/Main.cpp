
#include "Application.h"
#include "TestScene.h"
#include "OBJScene.h"

int main(int argc, char* argv[])
{
	Application app;

	app.AddScene("Test", new TestScene());
	app.AddScene("OBJ", new OBJScene());
	app.SetActiveScene("OBJ");

	if (app.Init())
	{
		app.Run();
		app.Shutdown();
	}

	return 0;
}