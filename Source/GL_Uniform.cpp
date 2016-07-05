#include "GL_Uniform.h"

#include "gl_core_4_4.h"
#include <glm/gtc/type_ptr.hpp>

namespace GL
{

	bool Uniform::Exists(const char* szName)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		return (uniLoc != -1);
	}

	void Uniform::Set(const char* szName, int value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);
		
		if (uniLoc == -1)
			return;

		glUniform1i(uniLoc, value);
	}

	void Uniform::Set(const char* szName, float value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniform1f(uniLoc, value);
	}

	void Uniform::Set(const char* szName, double value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniform1d(uniLoc, value);
	}

	void Uniform::Set(const char* szName, const glm::vec2& value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniform2f(uniLoc, value.x, value.y);
	}

	void Uniform::Set(const char* szName, const glm::vec3& value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniform3f(uniLoc, value.x, value.y, value.z);
	}

	void Uniform::Set(const char* szName, const glm::vec4& value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniform4f(uniLoc, value.x, value.y, value.z, value.w);
	}

	void Uniform::Set(const char* szName, const glm::mat2& value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniformMatrix2fv(uniLoc, (GLsizei)1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void Uniform::Set(const char* szName, const glm::mat3& value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniformMatrix3fv(uniLoc, (GLsizei)1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}

	void Uniform::Set(const char* szName, const glm::mat4& value)
	{
		GLint id;
		glGetIntegerv(GL_CURRENT_PROGRAM, &id);
		GLint uniLoc = glGetUniformLocation(id, szName);

		if (uniLoc == -1)
			return;

		glUniformMatrix4fv(uniLoc, (GLsizei)1, GL_FALSE, (GLfloat*)glm::value_ptr(value));
	}



} // namespace GL