#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

class SP3Scene : public Scene
{
public:
	SP3Scene();
	virtual ~SP3Scene();

	virtual bool Init();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Render(float deltaTime);

	virtual void Activate();
	virtual void Deactivate();

protected:
	Camera* _camera;

	float _time; // Used for our shaders

	// Stuff for GL mesh
	GLuint _vao, _vbo, _ibo;

	GL::Program* _program;
};