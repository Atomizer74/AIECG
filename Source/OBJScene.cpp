#include "OBJScene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "GL_Uniform.h"
#include "Mesh.h"

OBJScene::OBJScene() : _program(nullptr), _mesh(nullptr)
{

}

OBJScene::~OBJScene()
{

}



bool OBJScene::Init()
{
	// Setup Gizmos
	Gizmos::create();

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	_camera = new FlyCamera();
	_camera->LookAt(glm::vec3(10.0f), glm::vec3(0.0f));


	// Set up our shaders
	_program = new GL::Program();

	GL::Shader vertShader(GL::ShaderType::VERTEX_SHADER, "./shaders/vertTest.txt");
	GL::Shader fragShader(GL::ShaderType::FRAGMENT_SHADER, "./shaders/fragTest.txt");

	_program->AddShader(vertShader);
	_program->AddShader(fragShader);
	_program->Link();

	// TODO: Should test to make sure the shaders compiled

	// Setup mesh
	_mesh = new Mesh();
	if (_mesh->loadObj("models/bunny.obj") == false)
	{
		return false;
	}

	return true;
}

void OBJScene::Shutdown()
{
	// Cleanup the shader program
	delete _program;
	_program = nullptr;

	delete _mesh;
	_mesh = nullptr;

	delete _camera;
	_camera = nullptr;

	// Destroy Gizmos
	Gizmos::destroy();
}


bool OBJScene::Update(float deltaTime)
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

void OBJScene::Render(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed


	Gizmos::draw(_camera->GetProjectionView());

	glm::mat4 worldMatrix(1);
	glm::mat4 pvw = _camera->GetProjectionView() * worldMatrix;

	// Set up our program
	_program->Use();
	GL::Uniform::Set("projectionViewWorldMatrix", pvw);

	// Draw mesh
	_mesh->draw(GL_TRIANGLES);
}


void OBJScene::Activate()
{

}

void OBJScene::Deactivate()
{

}
