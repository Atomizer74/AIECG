#include "Camera.h"
#include "GLFW/glfw3.h"


Camera::Camera(float fovY, float aspect, float near, float far) : _transform(1), _view(1)
{
	SetPerspective(fovY, aspect, near, far);
}


void Camera::SetPerspective(float fovY, float aspect, float near, float far)
{
	_projection = glm::perspective(fovY, aspect, near, far);
	_projectionView = _projection * _view;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far)
{
	_projection = glm::ortho(left, right, bottom, top, near, far);
	_projectionView = _projection * _view;
}

void Camera::LookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
	_view = glm::lookAt(eye, target, up);
	_transform = glm::inverse(_view);
	_projectionView = _projection * _view;
}


FlyCamera::FlyCamera(float speed) : Camera(3.14159f * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f), _speed(speed)
{

}

void FlyCamera::Update(float deltaTime)
{
	GLFWwindow* window = glfwGetCurrentContext();

	float frameSpeed = _speed * deltaTime;

	// Increased move speed
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		frameSpeed = (_speed * 5) * deltaTime;

	// Movement
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		_transform[3] -= _transform[2] * frameSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		_transform[3] += _transform[2] * frameSpeed;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_transform[3] -= _transform[0] * frameSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_transform[3] += _transform[0] * frameSpeed;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		_transform[3] -= _transform[1] * frameSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		_transform[3] += _transform[1] * frameSpeed;


	// Rotation
	static bool sbMouseButtonDown = false;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) // Right Mouse Button
	{
		static double prevMouseX = 0;
		static double prevMouseY = 0;

		if (sbMouseButtonDown == false)
		{
			sbMouseButtonDown = true;
			glfwGetCursorPos(window, &prevMouseX, &prevMouseY);
		}

		double mouseX = 0, mouseY = 0;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		double deltaX = mouseX - prevMouseX;
		double deltaY = mouseY - prevMouseY;

		prevMouseX = mouseX;
		prevMouseY = mouseY;

		// Pitch
		if (deltaY != 0)
		{
			glm::mat4 mat = glm::axisAngleMatrix(glm::vec3(_transform[0]), (float)deltaY / -200.0f);
			_transform[0] = mat * _transform[0];
			_transform[1] = mat * _transform[1];
			_transform[2] = mat * _transform[2];
		}

		// Yaw
		if (deltaX != 0)
		{
			glm::mat4 mat = glm::axisAngleMatrix(glm::vec3(0,1,0), (float)deltaX / -200.0f);
			_transform[0] = mat * _transform[0];
			_transform[1] = mat * _transform[1];
			_transform[2] = mat * _transform[2];
		}
	}
	else
	{
		sbMouseButtonDown = false;
	}

	// Update matrices
	_view = glm::inverse(_transform);
	_projectionView = _projection * _view;
}