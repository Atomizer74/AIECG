#include "Image.h"

#include <stb_image.h>

Image::Image(const char* szFilename) : width(0), height(0), format(0), data(nullptr)
{
	data = stbi_load(szFilename, &width, &height, &format, STBI_rgb);
}

Image::~Image()
{
	if (data != nullptr)
		stbi_image_free(data);

	data = nullptr;
}

// Uploads the texture to GL and returns the texture ID
GLuint Image::Upload()
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	return id;
}