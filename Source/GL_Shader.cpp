#include "GL_Shader.h"

#include <fstream>
#include <sstream>

namespace GL
{

	// Return a named file's contents as a string
	std::string load_file(char const * filepath)
	{
		std::ifstream src(filepath);
		std::ostringstream buf;
		buf << src.rdbuf();
		return buf.str();
	}


	// Move Constructor
	Shader::Shader(Shader&& refShader)
	{
		this->_shaderId = refShader._shaderId;
		this->_type = refShader._type;
		this->_compileStatus = refShader._compileStatus;
		this->_compileInfo = refShader._compileInfo;

		refShader._shaderId = 0;
		refShader._compileStatus = GL_FALSE;
		refShader._compileInfo = nullptr;
	}

	// Move Assignment Operator
	Shader& Shader::operator=(Shader&& refShader)
	{
		this->_shaderId = refShader._shaderId;
		this->_type = refShader._type;
		this->_compileStatus = refShader._compileStatus;
		this->_compileInfo = refShader._compileInfo;

		refShader._shaderId = 0;
		refShader._compileStatus = GL_FALSE;
		refShader._compileInfo = nullptr;

		return (*this);
	}

	// Load file contents and pass it through to the source constructor
	Shader::Shader(ShaderType&& type, const char* szFilename) : Shader((const GLchar*)load_file(szFilename).c_str(), std::move(type))
	{

	}

	Shader::Shader(const GLchar* source, ShaderType&& type) : _type((GLenum)type), _compileStatus(GL_FALSE), _compileInfo(nullptr)
	{
		this->_shaderId = glCreateShader(this->_type);

		glShaderSource(this->_shaderId, 1, &source, NULL);
		glCompileShader(this->_shaderId);

		glGetShaderiv(this->_shaderId, GL_COMPILE_STATUS, &this->_compileStatus);

		this->_compileInfo = new GLchar[1024];
		glGetShaderInfoLog(this->_shaderId, 1024, 0, this->_compileInfo);
	}

	Shader::~Shader()
	{
		if (this->_compileStatus != GL_FALSE && this->_shaderId != 0)
			glDeleteShader(this->_shaderId);

		this->_shaderId = 0;
		this->_compileStatus = GL_FALSE;

		if (this->_compileInfo != nullptr)
		{
			delete[] this->_compileInfo;
			this->_compileInfo = nullptr;
		}
	}



	Shader* Shader::GetDefaultVertex()
	{
		static GLchar src[] =
			"#version 410\n \
			\
			layout(location = 0) in vec4 position; \
			layout(location = 1) in vec4 colour; \
			\
			out vec4 vColour; \
			\
			uniform mat4 projectionViewWorldMatrix;\
			\
			void main() { \
				vColour = colour; \
				gl_Position = projectionViewWorldMatrix * position; \
			}";
		static Shader shader(src, ShaderType::VERTEX_SHADER);
		return &shader;
	}

	Shader* Shader::GetDefaultFragment()
	{
		static GLchar src[] =
			"#version 410\n \
			\
			in vec4 vColour; \
			\
			out vec4 fragColour; \
			\
			void main() { \
				fragColour = vColour; \
			}";
		static Shader shader(src, ShaderType::FRAGMENT_SHADER);
		return &shader;
	}

}