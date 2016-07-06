#include "EmptyScene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "GL_Uniform.h"


EmptyScene::EmptyScene() : _camera(nullptr)
{

}

EmptyScene::~EmptyScene()
{

}



bool EmptyScene::Init()
{
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	_camera = new FlyCamera();
	_camera->LookAt(glm::vec3(10.0f), glm::vec3(0.0f));


	return true;
}

void EmptyScene::Shutdown()
{
	delete _camera;
	_camera = nullptr;
}


bool EmptyScene::Update(float deltaTime)
{
	_camera->Update(deltaTime);

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
						glm::vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(glm::vec3(10, 0, -10 + i),
						glm::vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}

	return true;
}

void EmptyScene::Render(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed


	Gizmos::draw(_camera->GetProjectionView());

}


void EmptyScene::Activate()
{

}

void EmptyScene::Deactivate()
{

}
