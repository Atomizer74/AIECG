#include "Application.h"

#include "Profiler.h"
#include "Gizmos.h"
#include <iostream>
#include <queue>
#include "imgui_impl_glfw_gl3.h"

void __stdcall logGLDebugCallback(unsigned int source,
								  unsigned int type,
								  unsigned int id,
								  unsigned int severity,
								  int length,
								  const char* message,
								  const void* userParam);


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

	// Enable debug callbacks
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(logGLDebugCallback, nullptr);

	// Create Gizmos
	Gizmos::create();

	// IMGUI
	ImGui_ImplGlfwGL3_Init(_window, true);
	ImGui::GetIO().DisplaySize.x = 1280;
	ImGui::GetIO().DisplaySize.y = 720;

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

	// IMGUI Shutdown
	ImGui_ImplGlfwGL3_Shutdown();

	// Destroy Gizmos
	Gizmos::destroy();

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

		// IMGUI new frame
		ImGui_ImplGlfwGL3_NewFrame();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - _prevTime).count()) / 1000.0f;

		if (_activeScene != nullptr && _activeScene->Update(deltaTime) == false)
			break;

		if (_activeScene != nullptr)
		{
			_activeScene->OnGUI(deltaTime);
			_activeScene->Render(deltaTime);
		}

		// Profiling information
		auto profiles = Profile::ProfileData();
		if (ImGui::Begin("Profiler"))
		{
			for each (auto profile in profiles)
			{
				// TODO: Convert to appropriate measurement
				ImGui::Text("%s[%fms-%fms]: %fms", profile.name.c_str(), (profile.min.count() / 1000000.0f), (profile.max.count() / 1000000.0f), (profile.total.count() / profile.runCount) / 1000000.0f);
			}
			ImGui::End();
		}

		// IMGUI Render
		ImGui::Render();

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



void __stdcall logGLDebugCallback(unsigned int source,
								  unsigned int type,
								  unsigned int id,
								  unsigned int severity,
								  int length,
								  const char* message,
								  const void* userParam)
{
	char src[16], t[20];

	if (source == GL_DEBUG_SOURCE_API)
		strcpy(src, "OpenGL");
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
		strcpy(src, "Windows");
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER)
		strcpy(src, "Shader Compiler");
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY)
		strcpy(src, "Third Party");
	else if (source == GL_DEBUG_SOURCE_APPLICATION)
		strcpy(src, "Application");
	else if (source == GL_DEBUG_SOURCE_OTHER)
		strcpy(src, "Other");

	if (type == GL_DEBUG_TYPE_ERROR)
		strcpy(t, "Error");
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		strcpy(t, "Deprecated Behavior");
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		strcpy(t, "Undefined Behavior");
	else if (type == GL_DEBUG_TYPE_PORTABILITY)
		strcpy(t, "Portability");
	else if (type == GL_DEBUG_TYPE_PERFORMANCE)
		strcpy(t, "Performance");
	else if (type == GL_DEBUG_TYPE_MARKER)
		strcpy(t, "Marker");
	else if (type == GL_DEBUG_TYPE_PUSH_GROUP)
		strcpy(t, "Push Group");
	else if (type == GL_DEBUG_TYPE_POP_GROUP)
		strcpy(t, "Pop Group");
	else if (type == GL_DEBUG_TYPE_OTHER)
		strcpy(t, "Other");

	if (severity == GL_DEBUG_SEVERITY_HIGH)
		std::cout << "GL Error: " << id << "\n\tType: " << t << "\n\tSource: " << src << "\n\tMessage: " << message << std::endl;
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		std::cout << "GL Warning: " << id << "\n\tType: " << t << "\n\tSource: " << src << "\n\tMessage: " << message << std::endl;
	else if (severity == GL_DEBUG_SEVERITY_LOW)
		std::cout << "GL: " << id << "\n\tType: " << t << "\n\tSource: " << src << "\n\tMessage: " << message << std::endl;
	//else if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
	//	std::cout << "GL Message: " << id << "\n\tType: " << t << "\n\tSource: " << src << "\n\tMessage: " << message << std::endl;
}