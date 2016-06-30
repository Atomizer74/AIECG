#pragma once
#include "gl_core_4_4.h"


namespace GL
{

	enum class ShaderType
	{
		VERTEX_SHADER = GL_VERTEX_SHADER,
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
		GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
		TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
		TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER
	};

	class Shader
	{
		GLuint _shaderId;
		GLenum _type;
		GLint _compileStatus;
		GLchar* _compileInfo;

		// Copy Constructor
		Shader(const Shader& refShader) {}
		// Copy Assignment Operator
		Shader& operator=(const Shader& refShader) {}

	public:
		// Move Constructor
		Shader(Shader&& refShader);
		// Move Assignment Operator
		Shader& operator=(Shader&& refShader);

		// Load shader from file
		Shader(ShaderType&& type, const char* szFilename);
		// Load shader from a source string
		Shader(const GLchar* source, ShaderType&& type);
		virtual ~Shader();


		GLuint Id() const { return this->_shaderId; }
		GLenum Type() const { return this->_type; }
		GLint CompileStatus() const { return this->_compileStatus; }
		const GLchar* CompileInfo() const { return this->_compileInfo; }

		// Default Shader Builders
		static Shader* GetDefaultVertex();
		static Shader* GetDefaultFragment();
	};
}