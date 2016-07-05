#include "SP6Scene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "GL_Uniform.h"


namespace SP6
{

	float getHeight(int x, int y, float scale, int octaves, float amplitude, float persistance)
	{
		float height = 0;

		glm::vec2 p((float)x, (float)y);

		for (int o = 0; o < octaves; o++)
		{
			float frequency = powf(2, (float)o);

			height += glm::perlin(p * scale * frequency) * amplitude;

			amplitude *= persistance;
		}

		return height;
	}



	SP6Scene::SP6Scene() : _program(nullptr), _camera(nullptr)
	{

	}

	SP6Scene::~SP6Scene()
	{

	}



	bool SP6Scene::Init()
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode

		//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		_camera = new FlyCamera();
		_camera->LookAt(glm::vec3(100, 200, 100), glm::vec3(0.0f));

		// Vertex information
		struct Vertex
		{
			glm::vec4	position;
			glm::vec4	colour;
			glm::vec2	texCoord;
			glm::vec4	normal;
		};

		unsigned int size = 1000;

		Vertex* vertices = new Vertex[size * size];
		_indexCount = (size - 1) * (size - 1) * 6;
		unsigned int* indices = new unsigned int[_indexCount];

		_perlinHeights = glm::vec2(FLT_MAX, FLT_MIN);

		for (int r = 0; r < size; r++)
		{
			for (int c = 0; c < size; c++)
			{
				float height = getHeight(c, r, 1.0f / size, 6, 100.0f, 0.5f);

				vertices[(r * size) + c].colour = glm::vec4(1);
				vertices[(r * size) + c].position = glm::vec4(c - (size * 0.5f), height, r - (size * 0.5f), 1);
				vertices[(r * size) + c].texCoord = glm::vec2(c / (float)(size - 1), r / (float)(size - 1));
				vertices[(r * size) + c].normal = glm::vec4(0, 1, 0, 0);

				// Lowest Point
				if (height < _perlinHeights.x)
					_perlinHeights.x = height;
				// Highest Point
				if (height > _perlinHeights.y)
					_perlinHeights.y = height;
			}
		}

		unsigned int index = 0;
		for (int r = 0; r < size - 1; r++)
		{
			for (int c = 0; c < size - 1; c++)
			{
				indices[index++] = (r * size) + c;
				indices[index++] = ((r + 1) * size) + c;
				indices[index++] = (r * size) + (c + 1);

				indices[index++] = (r * size) + (c + 1);
				indices[index++] = ((r + 1) * size) + c;
				indices[index++] = ((r + 1) * size) + (c + 1);
			}
		}


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
		glBufferData(GL_ARRAY_BUFFER, size * size * sizeof(Vertex), vertices, GL_STATIC_DRAW);

		// Tell GL how big our selected element array buffer(_ibo) is, where it is in memory(indices) and how to handle it
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);

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

		// Cleanup our vertices and indicies
		delete[] indices;
		delete[] vertices;

		// Set up our shaders
		_program = new GL::Program();

		GL::Shader vertShader(GL::ShaderType::VERTEX_SHADER, "./shaders/vertSP6.glsl");
		GL::Shader fragShader(GL::ShaderType::FRAGMENT_SHADER, "./shaders/fragSP6.glsl");

		_program->AddShader(vertShader);
		_program->AddShader(fragShader);
		_program->Link();

		// Load our texture
		Image diffuse("textures/hrgrass.jpg");
		Image dirt("textures/hrdirt.jpg");

		// Upload texture to GL
		_texDiffuse = diffuse.Upload();
		_texDirt = dirt.Upload();

		return true;
	}

	void SP6Scene::Shutdown()
	{
		// Cleanup textures
		glDeleteTextures(1, &_texDiffuse);

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


	bool SP6Scene::Update(float deltaTime)
	{
		_camera->Update(deltaTime);

		Gizmos::clear();
		Gizmos::addTransform(glm::mat4(1));


		return true;
	}

	void SP6Scene::Render(float deltaTime)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed


		Gizmos::draw(_camera->GetProjectionView());

		glm::mat4 worldMatrix(1);
		glm::mat4 pvw = _camera->GetProjectionView() * worldMatrix;

		// Set up our program
		_program->Use();
		GL::Uniform::Set("projectionViewWorldMatrix", pvw);
		GL::Uniform::Set("diffuse", 0);
		GL::Uniform::Set("dirt", 1);
		GL::Uniform::Set("perlinHeights", _perlinHeights);

		// Re-bind our vertex array object that we set up previously
		glBindVertexArray(_vao);

		// Re-bind the textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texDiffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _texDirt);

		// Tell it what to draw from our bound buffers
		glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

		// Unbind the textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);

		// Unbind out vertex array object so that other code doesn't accidently change our _vao
		glBindVertexArray(0);
	}


	void SP6Scene::Activate()
	{

	}

	void SP6Scene::Deactivate()
	{

	}

}