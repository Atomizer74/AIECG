#pragma once

#include <glm\glm.hpp>
#include <glm\ext.hpp>

class Camera
{
public:
	Camera(float fovY, float aspect, float near, float far);
	virtual ~Camera() {}

	void SetPerspective(float fovY, float aspect, float near, float far);
	void SetOrthographic(float left, float right, float bottom, float top, float near, float far);

	void LookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up = glm::vec3(0, 1, 0));


	const glm::mat4&	GetTransform() const		{ return _transform; }
	const glm::mat4&	GetView() const				{ return _view; }
	const glm::mat4&	GetProjection() const		{ return _projection; }
	const glm::mat4&	GetProjectionView() const	{ return _projectionView; }


	virtual void Update(float deltaTime) {}

protected:

	glm::mat4 _transform;

	glm::mat4 _view;
	glm::mat4 _projection;

	glm::mat4 _projectionView;
};

class FlyCamera : public Camera
{
public:
	FlyCamera(float speed = 10);
	virtual ~FlyCamera() {}

	void SetSpeed(float speed) { _speed = speed; }
	float GetSpeed() const { return _speed; }

	virtual void Update(float deltaTime);

protected:
	float _speed;
};