#include "SP3Scene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "GL_Uniform.h"

SP3Scene::SP3Scene() : _program(nullptr)
{

}

SP3Scene::~SP3Scene()
{

}


const unsigned int ROWS = 50;
const unsigned int COLS = 50;


bool SP3Scene::Init()
{
	// Set our timer
	_time = 0.0f;

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	_camera = new FlyCamera();
	_camera->LookAt(glm::vec3(50, 50, 50), glm::vec3(0.0f));

	// Vertex information
	struct Vertex
	{
		glm::vec4	position;
		glm::vec4	colour;
	};

	// Our vertex list
	Vertex* vertices = new Vertex[ROWS * COLS];

	for (unsigned int r = 0; r < ROWS; r++)
	{
		for (unsigned int c = 0; c < COLS; c++)
		{
			vertices[r * COLS + c].position = glm::vec4((float)c, 0, (float)r, 1);
			glm::vec3 colour = glm::vec3(sinf((c / (float)(COLS - 1)) * (r / (float)(ROWS - 1))));
			vertices[r * COLS + c].colour = glm::vec4(colour, 1);
		}
	}

	// Indices for the triangles
	GLuint* indices = new GLuint[(ROWS - 1) * (COLS - 1) * 6];

	unsigned int index = 0;
	for (unsigned int r = 0; r < (ROWS - 1); r++)
	{
		for (unsigned int c = 0; c < (COLS - 1); c++)
		{
			// Triangle 1
			indices[index++] = r * COLS + c;
			indices[index++] = (r + 1) * COLS + c;
			indices[index++] = (r + 1) * COLS + (c + 1);

			// Triangle 2
			indices[index++] = r * COLS + c;
			indices[index++] = (r + 1) * COLS + (c + 1);
			indices[index++] = r * COLS + (c + 1);
		}
	}

	// Generate our vertex buffer and index buffer
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	// Generate our vertex array
	glGenVertexArrays(1, &_vao);

	// Bind our vertex array
	glBindVertexArray(_vao);


	// VERTICES
	// Tell GL that our _vbo is an array buffer(vertex buffer) and bind it to the vertex array
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	// Tell GL how big our selected array buffer(_vbo) is, where it is in memory(vertices) and how to handle it
	glBufferData(GL_ARRAY_BUFFER, (ROWS * COLS) * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// Tell GL the structure of our array buffer(_vbo)
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(Vertex::position));


	// INDICES
	// Tell GL that our _ibo is an element array buffer(index buffer) and bind it to the vertex array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);

	// Tell GL how big our selected element array buffer(_ibo) is, where it is in memory(indices) and how to handle it
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ROWS - 1) * (COLS - 1) * 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);



	// Unbind our vertex array so that no further attempts to change a vertex array wont affect _vao
	glBindVertexArray(0);
	// Unbind our array buffer(vertex buffer)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Unbind our element array buffer(index buffer)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Cleanup our vertices and indicies
	delete[] vertices;
	delete[] indices;

	// Set up our shaders
	_program = new GL::Program();

	GL::Shader vertShader(GL::ShaderType::VERTEX_SHADER, "./shaders/vertSP3.txt");
	GL::Shader fragShader(GL::ShaderType::FRAGMENT_SHADER, "./shaders/fragSP3.txt");

	_program->AddShader(vertShader);
	_program->AddShader(fragShader);
	_program->Link();

	return true;
}

void SP3Scene::Shutdown()
{
	// Cleanup the shader program
	delete _program;
	_program = nullptr;

	// Cleanup the buffers and array
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
	glDeleteVertexArrays(1, &_vao);

	delete _camera;
	_camera = nullptr;
}


bool SP3Scene::Update(float deltaTime)
{
	_camera->Update(deltaTime);

	// Update our timer
	_time += deltaTime;

	return true;
}

void SP3Scene::Render(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed


	glm::mat4 worldMatrix(1);
	worldMatrix[3] = glm::vec4(-(ROWS / 2.0f), 0, -(COLS / 2.0f), 1); // Translate the world/model matrix to center the grid
	glm::mat4 pvw = _camera->GetProjectionView() * worldMatrix;

	// Set up our program
	_program->Use();
	GL::Uniform::Set("projectionViewWorldMatrix", pvw);
	GL::Uniform::Set("time", (_time * 20.0f));
	GL::Uniform::Set("heightScale", 2.0f);
	GL::Uniform::Set("freqScale", 0.3f);

	// Re-bind our vertex array object that we set up previously
	glBindVertexArray(_vao);

	// Index count
	unsigned int indexCount = (ROWS - 1) * (COLS - 1) * 6;

	// Tell it what to draw from our bound buffers
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	// Unbind out vertex array object so that other code doesn't accidently change our _vao
	glBindVertexArray(0);
}


void SP3Scene::Activate()
{

}

void SP3Scene::Deactivate()
{

}
