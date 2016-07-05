#include "TextureAtlas.h"
#include "Image.h"
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


TextureAtlas::TextureAtlas(int count, int texSize, glm::ivec4 defaultColour) : _atlasData(nullptr), _atlasID(0)
{
	this->_texCount = count;
	this->_texSize = texSize;
	this->_texBufferSize = this->_texSize + (BUFFERSIZE * 2);

	// Allocate space for the pixel data
	this->_pixelCount = (this->_texBufferSize * this->_texBufferSize) * (this->_texCount * this->_texCount);
	this->_atlasSize = this->_pixelCount * 4;
	this->_atlasData = new GLubyte[this->_atlasSize];

	// Initialize pixel data
	for (int i = 0; i < this->_pixelCount; i++)
	{
		// Default - This order might be wrong
		this->_atlasData[(i * 4)] = defaultColour.r; // Red
		this->_atlasData[(i * 4) + 1] = defaultColour.g; // Green
		this->_atlasData[(i * 4) + 2] = defaultColour.b; // Blue
		this->_atlasData[(i * 4) + 3] = defaultColour.a; // Alpha
	}
}

TextureAtlas::~TextureAtlas()
{
	// Free the atlas pixel data
	if (this->_atlasData != nullptr)
	{
		delete[] this->_atlasData;
		this->_atlasData = nullptr;
	}

	// Delete the atlas texture
	if (this->_atlasID != 0)
	{
		glDeleteTextures(1, &this->_atlasID);
		this->_atlasID = 0;
	}

	// Free the texture name map
	this->_texMap.clear();
}


// Uploads the atlas texture to GL
GLuint TextureAtlas::Upload()
{
	// TEMPORARY: Save out the atlas so I can see what is actually being output
	static int counter = 0;
	std::string outfile = "textures/atlas_" + std::to_string(counter++) + ".png";
	//int ret = stbi_write_png(outfile.c_str(), this->_texBufferSize * this->_texCount, this->_texBufferSize * this->_texCount, 4, this->_atlasData, this->_texBufferSize * this->_texCount * 4);

	
	// If we already have uploaded the atlas, delete our old texture
	if (this->_atlasID != 0)
	{
		glDeleteTextures(1, &this->_atlasID);
		this->_atlasID = 0;
	}

	// As long as our atlas data exists, upload it to GL
	if (this->_atlasData != nullptr)
	{
		glGenTextures(1, &this->_atlasID);
		glBindTexture(GL_TEXTURE_2D, this->_atlasID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->_texBufferSize * this->_texCount, this->_texBufferSize * this->_texCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->_atlasData);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	return this->_atlasID;
}

// Adds the file at szFilename in slot index of the atlas, mapped to szName
void TextureAtlas::AddTex(const char* szName, const char* szFilename, int index)
{
	// Load image from file
	Image img(szFilename);
	
	// For now, we check to make sure the dimensions of the image match the texSize of the atlas
	if (img.width != this->_texSize || img.height != this->_texSize)
		return; // Don't add a texture if the dimensions do not match

	// Copy pixels from image into index location in the atlas
	int atlasXOffset = ((index % this->_texCount) * this->_texBufferSize) + BUFFERSIZE;
	int atlasYOffset = ((index / this->_texCount) * this->_texBufferSize) + BUFFERSIZE;
	int atlasRowScan = (this->_texBufferSize * this->_texCount * 4);
	int texRowScan = (img.width * img.format); // This is the number of bytes in a row for the image file

	for (int y = -BUFFERSIZE; y < (this->_texSize + BUFFERSIZE); y++)
	{
		for (int x = -BUFFERSIZE; x < (this->_texSize + BUFFERSIZE); x++)
		{
			// Calculate the actual byte offset into the atlas data of this pixel
			int byteOffsetAtlas = ((atlasYOffset + y) * atlasRowScan) + ((atlasXOffset + x) * 4);
			// Calculate the byte offset into the texture data for this pixel
			// We do a little trick to cause the texture to wrap around into the buffer when copied
			int texY = ((y < 0) ? (img.height + y) : ((y >= img.height) ? ((y - img.height) % img.height) : (y)));
			int texX = ((x < 0) ? (img.width + x) : ((x >= img.width) ? ((x - img.width) % img.width) : (x)));
			int byteOffsetTex = (texY * texRowScan) + (texX * img.format);

			// Copy the pixel from the texture into the atlas based on the texture format
			switch (img.format)
			{
			case STBI_grey:
				{
					// Again, the order of the bytes could be wrong
					this->_atlasData[byteOffsetAtlas] = img.data[byteOffsetTex]; // Red
					this->_atlasData[byteOffsetAtlas + 1] = img.data[byteOffsetTex]; // Green
					this->_atlasData[byteOffsetAtlas + 2] = img.data[byteOffsetTex]; // Blue
					this->_atlasData[byteOffsetAtlas + 3] = 0xFF; // Alpha
					break;
				}
			case STBI_grey_alpha:
				{
					this->_atlasData[byteOffsetAtlas] = img.data[byteOffsetTex]; // Red
					this->_atlasData[byteOffsetAtlas + 1] = img.data[byteOffsetTex]; // Green
					this->_atlasData[byteOffsetAtlas + 2] = img.data[byteOffsetTex]; // Blue
					this->_atlasData[byteOffsetAtlas + 3] = img.data[byteOffsetTex + 1]; // Alpha
					break;
				}
			case STBI_rgb:
				{
					this->_atlasData[byteOffsetAtlas] = img.data[byteOffsetTex]; // Red
					this->_atlasData[byteOffsetAtlas + 1] = img.data[byteOffsetTex + 1]; // Green
					this->_atlasData[byteOffsetAtlas + 2] = img.data[byteOffsetTex + 2]; // Blue
					this->_atlasData[byteOffsetAtlas + 3] = 0xFF; // Alpha
					break;
				}
			case STBI_rgb_alpha:
				{
					this->_atlasData[byteOffsetAtlas] = img.data[byteOffsetTex]; // Red
					this->_atlasData[byteOffsetAtlas + 1] = img.data[byteOffsetTex + 1]; // Green
					this->_atlasData[byteOffsetAtlas + 2] = img.data[byteOffsetTex + 2]; // Blue
					this->_atlasData[byteOffsetAtlas + 3] = img.data[byteOffsetTex + 3]; // Alpha
					break;
				}
			default:
				return; // Something went horribly wrong, return
			}
		}
	}

	// Add szName to the map
	this->_texMap[szName] = index;
}


// Calculate the min UV for the index(from 0.0 to 1.0)
glm::vec2 TextureAtlas::MinUV(int index)
{
	glm::vec2 min;

	min.x = (((index % this->_texCount) * this->_texBufferSize) + BUFFERSIZE) / (float)(this->_texCount * this->_texBufferSize);
	min.y = (((index / this->_texCount) * this->_texBufferSize) + BUFFERSIZE) / (float)(this->_texCount * this->_texBufferSize);

	return min;
}

// Calculate the max UV for the index(from 0.0 to 1.0)
glm::vec2 TextureAtlas::MaxUV(int index)
{
	glm::vec2 max;

	max.x = ((((index % this->_texCount) + 1) * this->_texBufferSize) - BUFFERSIZE) / (float)(this->_texCount * this->_texBufferSize);
	max.y = ((((index / this->_texCount) + 1) * this->_texBufferSize) - BUFFERSIZE) / (float)(this->_texCount * this->_texBufferSize);

	return max;
}


// Gets the index for szName, returns -1 if szName isn't mapped
int TextureAtlas::GetIndex(const char* szName)
{
	if (this->_texMap.find(szName) == this->_texMap.end())
		return -1;

	return this->_texMap.at(szName);
}