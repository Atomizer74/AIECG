#pragma once

#include "Scene.h"
#include "GL_Program.h"
#include "Camera.h"
#include "ParticleSystem.h"


class ParticleScene : public Scene
{
public:
	ParticleScene();
	virtual ~ParticleScene();

	virtual bool Init();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Render(float deltaTime);

	virtual void Activate();
	virtual void Deactivate();

protected:
	Camera* _camera;
	GL::Program* _program;
	ParticleEmitter* _emitter;

	GLuint _texFire1;
	GLuint _texFire2;
	GLuint _texFire3;
};