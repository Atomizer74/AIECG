#pragma once
#include "gl_core_4_4.h"

// Simple wrapper struct around stbi_load()
struct Image
{
	int width;
	int height;
	int format;

	unsigned char* data;

	Image(const char* szFilename);

	~Image();

	// Uploads the texture to GL and returns the texture ID
	GLuint Upload();
};