#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

class TestScene : public Scene
{
public:
	TestScene();
	virtual ~TestScene();

	virtual bool Init();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Render(float deltaTime);

	virtual void Activate();
	virtual void Deactivate();

protected:
	Camera* _camera;

	// Stuff for GL mesh
	GLuint _vao, _vbo, _ibo;

	GL::Program* _program;
};