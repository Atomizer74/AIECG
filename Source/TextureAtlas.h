#pragma once

/*
Description: The idea of this class is to combine individual smaller textures into one large texture, that can then be indexed to calculate the UV coordinates
*/

#include "gl_core_4_4.h"
#include <map>
#include <string>
#include <glm\glm.hpp>

class TextureAtlas
{
	// Extra pixel buffer around every texture in the atlas
	const int BUFFERSIZE = 128;

	// The number of textures on each dimension(_texCount x _texCount)
	int _texCount;
	// The size of each texture
	int _texSize;
	// The size of each texture + the buffer
	int _texBufferSize;

	// The OpenGL texture id for the atlas
	GLuint _atlasID;

	// Maps a name to an ID in the atlas
	std::map<std::string, int> _texMap;

	// Contains the size in bytes of the pixel data
	int _atlasSize;
	// Total number of pixels contained within the atlas
	int _pixelCount;

	// Contains the pixel data for the atlas
	GLubyte* _atlasData;

public:

	TextureAtlas(int count, int texSize, glm::ivec4 defaultColour = glm::ivec4(0x00, 0x00, 0x00, 0xFF));
	virtual ~TextureAtlas();

	// Uploads the atlas texture to GL
	GLuint Upload();
	// Binds the atlas GL texture for rendering on texture slot
	void Bind(int slot = 0) { glActiveTexture(GL_TEXTURE0 + slot); glBindTexture(GL_TEXTURE_2D, this->_atlasID); }
	// Gets the atlas' GL texture id
	GLuint GetID() { return this->_atlasID; }

	// Adds the file at szFilename in slot index of the atlas, mapped to szName
	void AddTex(const char* szName, const char* szFilename, int index);

	// Calculate the min UV for the index(from 0.0 to 1.0)
	glm::vec2 MinUV(int index);
	// Calculate the max UV for the index(from 0.0 to 1.0)
	glm::vec2 MaxUV(int index);

	// Gets the index for szName, returns -1 if szName isn't mapped
	int GetIndex(const char* szName);
};