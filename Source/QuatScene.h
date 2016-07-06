#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"
#include "Image.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>



class QuatScene : public Scene
{
public:
	QuatScene();
	virtual ~QuatScene();

	virtual bool Init();
	virtual void Shutdown();

	virtual bool Update(float deltaTime);
	virtual void Render(float deltaTime);

	virtual void Activate();
	virtual void Deactivate();

protected:
	Camera* _camera;

	float _time;

	struct Bone
	{
		Bone* parent;

		glm::vec3 P;
		glm::vec3 S;
		glm::quat R;
	};

	Bone _calf, _shin, _foot;

	Bone _calfKeys[2], _shinKeys[2], _footKeys[2];
};