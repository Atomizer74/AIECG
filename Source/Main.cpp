#include "gl_core_4_4.h"

#include <GLFW\glfw3.h>
#include <iostream>
#include "Gizmos.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;



int main(int argc, char* argv[])
{
	// Initialize GLFW
	if (glfwInit() == GL_FALSE)
		return -1;
	
	// Create the main window
	GLFWwindow* window = glfwCreateWindow(1280, 720, "AIE Computer Graphics", nullptr, nullptr);
	if (window == nullptr)
	{
		glfwTerminate();
		return -1;
	}

	// Set window as current
	glfwMakeContextCurrent(window);

	// Attempt to use modern OpenGL
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwTerminate();
		return -1;
	}

	// Setup Gizmos
	Gizmos::create();

	// Display the OpenGL Version to the console
	std::cout << "GL: " << ogl_GetMajorVersion() << "." << ogl_GetMinorVersion() << std::endl;


	// TODO: Should probably do this elsewhere
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	mat4 projection = glm::perspective(glm::pi<float>() * 0.25f,
								   1280.0f / 720.0f,
								   0.1f, 1000.0f);
	mat4 view = glm::lookAt(vec3(5.0f), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

	mat4 camera = projection * view;




	// Main Loop
	while (glfwWindowShouldClose(window) == GL_FALSE &&
		   glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		// Poll for any window events
		glfwPollEvents();

		// TODO: Stuff - Put this elsewhere
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed

		Gizmos::clear();

		Gizmos::addAABBFilled(vec3(0.0f), vec3(0.5f), vec4(1, 0, 0, 1));

		Gizmos::draw(camera);




		// Swap our GPU buffers
		glfwSwapBuffers(window);
	}


	// Destroy Gizmos
	Gizmos::destroy();
	// Destroy GLFW
	glfwTerminate();

	return 0;
}