#include "TestScene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "GL_Uniform.h"


TestScene::TestScene() : _program(nullptr), _camera(nullptr)
{
	
}

TestScene::~TestScene()
{

}



bool TestScene::Init()
{
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	_camera = new FlyCamera();
	_camera->LookAt(glm::vec3(10.0f), glm::vec3(0.0f));

	// Vertex information
	struct Vertex
	{
		glm::vec4	position;
		glm::vec4	colour;
		glm::vec2	texCoord;
		glm::vec4	normal;
	};

	// Our vertex list
	Vertex vertices[4]
	{
		{ glm::vec4(-5, 0, -5, 1), glm::vec4(1, 0, 0, 1), glm::vec2(0, 0), glm::vec4(0, 1, 0, 0) },
		{ glm::vec4( 5, 0, -5, 1), glm::vec4(0, 0, 1, 1), glm::vec2(1, 0), glm::vec4(0, 1, 0, 0) },
		{ glm::vec4(-5, 0,  5, 1), glm::vec4(0, 1, 0, 1), glm::vec2(0, 1), glm::vec4(0, 1, 0, 0) },
		{ glm::vec4( 5, 0,  5, 1), glm::vec4(1, 1, 1, 1), glm::vec2(1, 1), glm::vec4(0, 1, 0, 0) }
	};

	// Indices for 2 triangles
	GLuint indices[6]
	{
		0, 2, 1,
		1, 2, 3
	};

	// Generate our vertex buffer and index buffer
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	// Generate our vertex array
	glGenVertexArrays(1, &_vao);

	// Bind our vertex array
	glBindVertexArray(_vao);


	// Tell GL that our _vbo is an array buffer(vertex buffer) and bind it to the vertex array
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	// Tell GL that our _ibo is an element array buffer(index buffer) and bind it to the vertex array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	// Tell GL how big our selected array buffer(_vbo) is, where it is in memory(vertices) and how to handle it
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// Tell GL how big our selected element array buffer(_ibo) is, where it is in memory(indices) and how to handle it
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	// Tell GL the structure of our array buffer(_vbo)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(Vertex::position));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::colour)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::colour) + sizeof(Vertex::texCoord)));

	// Unbind our vertex array so that no further attempts to change a vertex array wont affect _vao
	glBindVertexArray(0);

	// Set up our shaders
	_program = new GL::Program();

	GL::Shader vertShader(GL::ShaderType::VERTEX_SHADER, "./shaders/vertTest.glsl");
	GL::Shader fragShader(GL::ShaderType::FRAGMENT_SHADER, "./shaders/fragTest.glsl");

	_program->AddShader(vertShader);
	_program->AddShader(fragShader);
	_program->Link();

	// Load our texture
	Image diffuse("textures/crate.png");
	Image normal("textures/dirt_normal.png");

	// Upload texture to GL
	_texDiffuse = diffuse.Upload();
	_texNormal = normal.Upload();

	return true;
}

void TestScene::Shutdown()
{
	// Cleanup textures
	glDeleteTextures(1, &_texDiffuse);
	glDeleteTextures(1, &_texNormal);

	// Cleanup shader program
	delete _program;
	_program = nullptr;

	// Cleanup the buffers and array
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
	glDeleteVertexArrays(1, &_vao);

	delete _camera;
	_camera = nullptr;
}


bool TestScene::Update(float deltaTime)
{
	_camera->Update(deltaTime);

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1));

	static float time = 0;
	time += deltaTime;
	_lightDirection.x = sinf(time);
	_lightDirection.y = cosf(time);
	_lightDirection.z = 0;

	// Draw light vector
	Gizmos::addLine(glm::vec3(0), _lightDirection * 3, glm::vec4(1,1,0,1));
	
	/*glm::vec4 white(1);
	glm::vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; i++)
	{
		Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
						glm::vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(glm::vec3(10, 0, -10 + i),
						glm::vec3(-10, 0, -10 + i),
						i == 10 ? white : black);
	}*/

	return true;
}

void TestScene::Render(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed

	
	Gizmos::draw(_camera->GetProjectionView());

	glm::mat4 worldMatrix(1);
	glm::mat4 pvw = _camera->GetProjectionView() * worldMatrix;

	// Set up our program
	_program->Use();
	GL::Uniform::Set("projectionViewWorldMatrix", pvw);
	GL::Uniform::Set("diffuse", 0);
	GL::Uniform::Set("normal", 1);
	GL::Uniform::Set("lightDirection", _lightDirection);

	// Re-bind our vertex array object that we set up previously
	glBindVertexArray(_vao);

	// Re-bind the textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texDiffuse);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _texNormal);

	// Tell it what to draw from our bound buffers
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	// Unbind the textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Unbind out vertex array object so that other code doesn't accidently change our _vao
	glBindVertexArray(0);
}


void TestScene::Activate()
{

}

void TestScene::Deactivate()
{

}
