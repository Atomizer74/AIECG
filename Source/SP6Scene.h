#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"
#include "Image.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>

namespace SP6
{

	class SP6Scene : public Scene
	{
	public:
		SP6Scene();
		virtual ~SP6Scene();

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

		// Textures
		GLuint _texDiffuse;
		GLuint _texDirt;

		GL::Program* _program;

		unsigned int _indexCount;
		glm::vec2 _perlinHeights;
	};
}