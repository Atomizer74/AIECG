#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"
#include "Image.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>


class EmptyScene : public Scene
{
public:
	EmptyScene();
	virtual ~EmptyScene();

	virtual bool Init();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Render(float deltaTime);

	virtual void Activate();
	virtual void Deactivate();

protected:
	Camera* _camera;
};
