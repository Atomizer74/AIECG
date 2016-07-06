#include "QuatScene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "GL_Uniform.h"


QuatScene::QuatScene() : _camera(nullptr)
{

}

QuatScene::~QuatScene()
{

}



bool QuatScene::Init()
{
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	_camera = new FlyCamera();
	_camera->LookAt(glm::vec3(10.0f), glm::vec3(0.0f));

	_time = 0;

	_calf.parent = nullptr;
	_shin.parent = &_calf;
	_foot.parent = &_shin;

	_calf.P = glm::vec3(0, 5, 0);
	_shin.P = glm::vec3(0, -2.5f, 0);
	_foot.P = glm::vec3(0, -2.5f, 0);

	_calf.R = glm::quat();
	_shin.R = glm::quat();
	_foot.R = glm::quat();


	_calfKeys[0] = _calfKeys[1] = _calf;
	_shinKeys[0] = _shinKeys[1] = _shin;
	_footKeys[0] = _footKeys[1] = _foot;

	// calf rotates from 60, -60
	_calfKeys[0].R = glm::angleAxis(glm::radians(60.0f), glm::vec3(1, 0, 0));
	_calfKeys[1].R = glm::angleAxis(glm::radians(-60.0f), glm::vec3(1, 0, 0));

	// shin rotates from 0, -80
	_shinKeys[1].R = glm::angleAxis(glm::radians(-80.0f), glm::vec3(1, 0, 0));

	// foot doesn't rotate

	return true;
}

void QuatScene::Shutdown()
{
	delete _camera;
	_camera = nullptr;
}


bool QuatScene::Update(float deltaTime)
{
	Gizmos::clear();


	// rotation test stuff
	_time += deltaTime;

	float dt = cosf(_time) * 0.5f + 0.5f;
	
	glm::vec4 t1(10, 0, 10, 1);
	glm::vec4 t2(-10, 0, 10, 1);
	glm::vec4 t3(-10, 0, -10, 1);

	glm::vec4 t12 = glm::mix(t1, t2, dt);
	glm::vec4 t23 = glm::mix(t2, t3, dt);

	glm::vec4 t = glm::mix(t12, t23, dt);

	glm::quat q1;
	glm::quat q2 = glm::angleAxis(3.14159f, glm::normalize(glm::vec3(0, 1, 0)));
	glm::quat q3 = glm::angleAxis(3.14159f, glm::normalize(glm::vec3(1, 0, 0)));

	glm::quat q12 = glm::slerp(q1, q2, dt);
	glm::quat q23 = glm::slerp(q2, q3, dt);

	glm::quat q = glm::slerp(q12, q23, dt);

	glm::mat4 m(1);

	m = glm::toMat4(q);
	
	m[3] = t;

	//Gizmos::addTransform(m, 5);

	//Gizmos::addAABBFilled(glm::vec3(t), glm::vec3(3, 0.25f, 1), glm::vec4(0, 0, 1, 1), &m);


	// Update bones based off keyframes
	_calf.P = glm::mix(_calfKeys[0].P, _calfKeys[1].P, dt);
	_calf.R = glm::slerp(_calfKeys[0].R, _calfKeys[1].R, dt);

	_shin.P = glm::mix(_shinKeys[0].P, _shinKeys[1].P, dt);
	_shin.R = glm::slerp(_shinKeys[0].R, _shinKeys[1].R, dt);

	_foot.P = glm::mix(_footKeys[0].P, _footKeys[1].P, dt);
	_foot.R = glm::slerp(_footKeys[0].R, _footKeys[1].R, dt);


	// draw calf
	glm::mat4 calfM = glm::toMat4(_calf.R);
	calfM[3] = glm::vec4(_calf.P, 1);

	// draw shin
	glm::mat4 shinM = glm::toMat4(_shin.R);
	shinM[3] = glm::vec4(_shin.P, 1);

	shinM = calfM * shinM;

	// draw foot
	glm::mat4 footM = glm::toMat4(_foot.R);
	footM[3] = glm::vec4(_foot.P, 1);

	footM = shinM * footM;


	Gizmos::addTransform(calfM);
	Gizmos::addTransform(shinM);
	Gizmos::addTransform(footM);

	Gizmos::addLine(glm::vec3(calfM[3]), glm::vec3(shinM[3]), glm::vec4(1, 1, 0, 1));
	Gizmos::addLine(glm::vec3(shinM[3]), glm::vec3(footM[3]), glm::vec4(1, 1, 0, 1));


	_camera->Update(deltaTime);

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

void QuatScene::Render(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed


	Gizmos::draw(_camera->GetProjectionView());

}


void QuatScene::Activate()
{

}

void QuatScene::Deactivate()
{

}
