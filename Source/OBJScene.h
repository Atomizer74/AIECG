#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

class Mesh;


enum class LightType
{
	POINT,
	DIRECTIONAL,
	SPOT
};

// Simple light struct for setting up lights in a scene
struct Light
{
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 colour;

	Light() : position(0), direction(0), colour(1) {}
	~Light() {}
};

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
	float _time;

	Camera* _camera;

	GL::Program* _program;

	Mesh* _mesh;

	glm::vec3 _ambientLight;
	Light _light;
};