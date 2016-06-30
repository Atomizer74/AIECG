#pragma once
#include "GL_Shader.h"
#include <vector>

namespace GL
{

	class Program
	{
		GLuint _programId;
		GLint _linkStatus;
		GLchar* _linkInfo;

		std::vector<GLuint> _attachedShaders;


		// Copy Constructor
		Program(const Program& refProgram) {}
		// Copy Assignment Operator
		Program& operator=(const Program& refProgram) {}

	public:
		// Move Constructor
		Program(Program&& refProgram);
		// Move Assignment Operator
		Program& operator=(Program&& refProgram);

		Program();
		~Program();

		GLuint Id() const { return this->_programId; }
		GLint LinkStatus() const { return this->_linkStatus; }
		const GLchar* LinkInfo() const { return this->_linkInfo; }

		void AddShader(const Shader& refShader);
		void Link();
		void Use();

		// Default Program Builder
		static Program* DefaultProgram();
	};

}