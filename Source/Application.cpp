#include "Application.h"

#include "Gizmos.h"
#include <iostream>


Application::Application() : _window(nullptr), _activeScene(nullptr)
{

}


Application::~Application()
{

}


bool Application::Init()
{
	// Initialize GLFW
	if (glfwInit() == GL_FALSE)
		return false;

	// Create the main window
	_window = glfwCreateWindow(1280, 720, "AIE Computer Graphics", nullptr, nullptr);
	if (_window == nullptr)
	{
		glfwTerminate();
		return false;
	}

	// Set window as current
	glfwMakeContextCurrent(_window);

	// Attempt to use modern OpenGL
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwTerminate();
		return false;
	}

	// Display the OpenGL Version to the console
	std::cout << "GL: " << ogl_GetMajorVersion() << "." << ogl_GetMinorVersion() << std::endl;


	// Initialise scenes
	for (auto& s : _scenes)
	{
		s.second->Init();
	}

	_prevTime = std::chrono::high_resolution_clock::now();

	return true;
}

void Application::Shutdown()
{
	// Cleanup scenes
	for (auto& s : _scenes)
	{
		s.second->Shutdown();
		delete s.second;
	}

	// Destroy GLFW
	glfwTerminate();
	_window = nullptr;
}

void Application::Run()
{
	// Main Loop
	while (glfwWindowShouldClose(_window) == GL_FALSE &&
		   glfwGetKey(_window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		// Poll for any window events
		glfwPollEvents();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _prevTime).count()) / 1000.0f;

		if (_activeScene != nullptr && _activeScene->Update(deltaTime) == false)
			break;

		if (_activeScene != nullptr)
			_activeScene->Render(deltaTime);


		// Swap our GPU buffers
		glfwSwapBuffers(_window);

		_prevTime = currentTime;
	}
}

void Application::AddScene(const char* szName, Scene* pScene)
{
	if (_scenes.find(szName) == _scenes.end())
		_scenes[szName] = pScene;
	else
		std::cout << "Failed to add scene [" << szName << "], already exists!" << std::endl;
}

bool Application::SetActiveScene(const char* szName)
{
	auto iter = _scenes.find(szName);

	if (iter == _scenes.end())
		return false;

	//deactivate old scene
	if (_activeScene != nullptr)
		_activeScene->Deactivate();

	_activeScene = iter->second;

	_activeScene->Activate();

	return true;
}