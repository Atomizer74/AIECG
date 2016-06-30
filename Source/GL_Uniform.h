#pragma once

#include <glm/glm.hpp>

namespace GL
{

	/*
	Static class to manage uniforms on the active shader in a safe way
	*/
	class Uniform
	{
		Uniform() = delete;
		~Uniform() = delete;

	public:

		static bool Exists(const char* szName);

		static void Set(const char* szName, float value);
		static void Set(const char* szName, double value);
		static void Set(const char* szName, const glm::vec2& value);
		static void Set(const char* szName, const glm::vec3& value);
		static void Set(const char* szName, const glm::vec4& value);
		static void Set(const char* szName, const glm::mat2& value);
		static void Set(const char* szName, const glm::mat3& value);
		static void Set(const char* szName, const glm::mat4& value);
	};

}