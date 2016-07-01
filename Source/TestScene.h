#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <stb_image.h>

// Simple wrapper struct around stbi_load()
struct Image
{
	int width;
	int height;
	int format;

	unsigned char* data;

	Image(const char* szFilename) : width(0), height(0), format(0), data(nullptr)
	{
		data = stbi_load(szFilename, &width, &height, &format, STBI_rgb);
	}

	~Image()
	{
		if (data != nullptr)
			delete[] data;

		data = nullptr;
	}

	// Uploads the texture to GL and returns the texture ID
	GLuint Upload()
	{
		GLuint id = 0;

		if (data != nullptr)
		{
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			switch (format)
			{
			case STBI_grey:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
					break;
				}
			case STBI_grey_alpha:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, data);
					break;
				}
			case STBI_rgb:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					break;
				}
			case STBI_rgb_alpha:
				{
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
					break;
				}
			default:
				return 0;
			}
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		return id;
	}
};



class TestScene : public Scene
{
public:
	TestScene();
	virtual ~TestScene();

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
	GLuint _texNormal;

	GL::Program* _program;
};