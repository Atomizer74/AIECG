#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

class Mesh;

class OBJScene : public Scene
{
public:
	OBJScene();
	virtual ~OBJScene();

	virtual bool Init();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Render(float deltaTime);

	virtual void Activate();
	virtual void Deactivate();

protected:
	Camera* _camera;

	GL::Program* _program;

	Mesh* _mesh;
};