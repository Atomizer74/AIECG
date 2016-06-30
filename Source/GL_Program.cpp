#include "GL_Program.h"

namespace GL
{

	// Move Constructor
	Program::Program(Program&& refProgram)
	{
		this->_programId = refProgram._programId;
		this->_linkStatus = refProgram._linkStatus;
		this->_linkInfo = refProgram._linkInfo;

		refProgram._programId = 0;
		refProgram._linkStatus = GL_FALSE;
		refProgram._linkInfo = nullptr;
	}

	// Move Assignment Operator
	Program& Program::operator=(Program&& refProgram)
	{
		this->_programId = refProgram._programId;
		this->_linkStatus = refProgram._linkStatus;
		this->_linkInfo = refProgram._linkInfo;

		refProgram._programId = 0;
		refProgram._linkStatus = GL_FALSE;
		refProgram._linkInfo = nullptr;

		return (*this);
	}


	Program::Program() : _linkStatus(GL_FALSE), _linkInfo(nullptr)
	{
		this->_programId = glCreateProgram();
	}

	Program::~Program()
	{
		if (this->_linkStatus != GL_FALSE && this->_programId != 0)
			glDeleteProgram(this->_programId);

		this->_programId = 0;
		this->_linkStatus = GL_FALSE;

		if (this->_linkInfo != nullptr)
		{
			delete[] this->_linkInfo;
			this->_linkInfo = nullptr;
		}
	}



	void Program::AddShader(const Shader& refShader)
	{
		glAttachShader(this->_programId, refShader.Id());
		this->_attachedShaders.push_back(refShader.Id());
	}

	void Program::Link()
	{
		glLinkProgram(this->_programId);
		glGetProgramiv(this->_programId, GL_LINK_STATUS, &this->_linkStatus);

		this->_linkInfo = new GLchar[1024];
		glGetProgramInfoLog(this->_programId, 1024, 0, this->_linkInfo);

		for each (auto shader in this->_attachedShaders)
			glDetachShader(this->_programId, shader);
	}

	void Program::Use()
	{
		glUseProgram(this->_programId);
	}


	Program* Program::DefaultProgram()
	{
		static Program program;
		
		// See if program has already been compiled
		if (program.LinkStatus() == GL_FALSE)
		{
			program.AddShader(*Shader::GetDefaultVertex());
			program.AddShader(*Shader::GetDefaultFragment());
			program.Link();
		}

		return &program;
	}

} // namespace GL