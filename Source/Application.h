#pragma once

#include "Scene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <map>
#include <chrono>

class Application
{
public:
	Application();
	~Application();

	virtual bool Init();
	virtual void Shutdown();

	void Run();

	void AddScene(const char* szName, Scene* pScene);
	bool SetActiveScene(const char* szName);

protected:
	GLFWwindow* _window;

	std::chrono::high_resolution_clock::time_point _prevTime;

	Scene* _activeScene;
	std::map<std::string, Scene*> _scenes;
};