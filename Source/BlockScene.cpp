#include "BlockScene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <vector>
#include <array>

#include "Gizmos.h"
#include "GL_Uniform.h"
#include <imgui.h>


namespace Block
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



	BlockScene::BlockScene() : _program(nullptr), _camera(nullptr)
	{

	}

	BlockScene::~BlockScene()
	{

	}



	bool BlockScene::Init()
	{
		Profile::Begin("SceneInit");
		// Some initial GL settings
		glClearColor(0.3f, 0.4f, 0.8f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_MULTISAMPLE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable wireframe mode

		// Setup the camera
		//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
		_camera = new FlyCamera();
		_camera->LookAt(glm::vec3(-10, (Chunk::CHUNK_Y / 2.0f) + 20, -10), glm::vec3(0, (Chunk::CHUNK_Y / 2.0f) + 10, 0));

		
		_world = new World();
		
		for (int x = 0; x < World::WORLD_X; x++)
		{
			for (int z = 0; z < World::WORLD_Z; z++)
			{
				_world->LoadChunk(x, z);
			}
		}

		_world->GenerateMeshes();
		
		// Set up our shaders
		_program = new GL::Program();

		GL::Shader vertShader(GL::ShaderType::VERTEX_SHADER, "./shaders/vertBlock.glsl");
		GL::Shader fragShader(GL::ShaderType::FRAGMENT_SHADER, "./shaders/fragBlock.glsl");

		_program->AddShader(vertShader);
		_program->AddShader(fragShader);
		_program->Link();

		// Initialize time
		_time = 0.0f;

		// Setup light
		_lightPosition = glm::vec4(0, 0, -300, 1);
		_lightDirection = glm::vec4(0, 0, 1, 0);
		glm::mat4 rotMat = glm::rotate(45.0f, glm::vec3(-1.0f, -1.0f, 0));
		_lightPosition = _lightPosition * rotMat;
		_lightDirection = _lightDirection * rotMat;

		Profile::End();
		return true;
	}

	void BlockScene::Shutdown()
	{
		// Cleanup shader program
		delete _program;
		_program = nullptr;


		delete _camera;
		_camera = nullptr;
	}


	void BlockScene::OnGUI(float deltaTime)
	{
		Profile::Begin("SceneGUI");
		
		Profile::End();
	}


	bool BlockScene::Update(float deltaTime)
	{
		Profile::Begin("SceneUpdate");
		_camera->Update(deltaTime);

		Gizmos::clear();
		Gizmos::addTransform(glm::mat4(1));


		glm::vec4 white(1);
		glm::vec4 black(0, 0, 0, 1);

		for (int i = 0; i < 21; i++)
		{
			Gizmos::addLine(glm::vec3(-10 + i, 0, 10),
							glm::vec3(-10 + i, 0, -10),
							i == 10 ? white : black);
			Gizmos::addLine(glm::vec3(10, 0, -10 + i),
							glm::vec3(-10, 0, -10 + i),
							i == 10 ? white : black);
		}

		Gizmos::addSphere(glm::vec3(_lightPosition.x, _lightPosition.y, _lightPosition.z), 5.0f, 10, 10, glm::vec4(1, 1, 0, 1));

		_time += deltaTime;

		Profile::End();
		return true;
	}

	void BlockScene::Render(float deltaTime)
	{
		Profile::Begin("SceneRender");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed

		Gizmos::draw(_camera->GetProjectionView());

		glm::mat4 worldMatrix(1);
		glm::mat4 pvw = _camera->GetProjectionView() * worldMatrix;

		// Set up our shader program
		_program->Use();

		// Material properties
		GL::Uniform::Set("diffuse", 0);
		GL::Uniform::Set("normal", 1);
		GL::Uniform::Set("specular", 2);
		GL::Uniform::Set("specularPower", 64.0f);
		GL::Uniform::Set("fresnelScale", 0.5f);


		// Light properties
		GL::Uniform::Set("light.position", glm::vec3(_lightPosition.x, _lightPosition.y, _lightPosition.z));
		GL::Uniform::Set("light.direction", -glm::vec3(_lightDirection.x, _lightDirection.y, _lightDirection.z)); // Direction TO the light
		GL::Uniform::Set("light.colour", glm::vec3(1, 1, 1));
		GL::Uniform::Set("light.intensity", 1.0f);
		GL::Uniform::Set("ambientLight", glm::vec3(0.25f, 0.25f, 0.25f));
		
		// Misc properties
		GL::Uniform::Set("projectionViewWorldMatrix", pvw);
		GL::Uniform::Set("worldMatrix", worldMatrix);
		GL::Uniform::Set("cameraPosition", _camera->GetTransform()[3]);
		GL::Uniform::Set("time", _time);
		
		_world->Render(deltaTime);
		Profile::End();
	}


	void BlockScene::Activate()
	{

	}

	void BlockScene::Deactivate()
	{

	}

#pragma region BLOCKWORLDSTUFF

	const float blockSize = 1.0f;

	const glm::ivec3 vertNormTable[6] {
		glm::ivec3( 1, 0, 0), // X+
		glm::ivec3(-1, 0, 0), // X-
		glm::ivec3( 0, 1, 0), // Y+
		glm::ivec3( 0,-1, 0), // Y-
		glm::ivec3( 0, 0, 1), // Z+
		glm::ivec3( 0, 0,-1)  // Z-
	};

	const glm::ivec4 vertTangentTable[6] {
		glm::ivec4( 0, 0,-1, 1), // X+
		glm::ivec4( 0, 0, 1, 1), // X-
		glm::ivec4( 0, 0, 1, 1), // Y+
		glm::ivec4( 0, 0, 1, 1), // Y-
		glm::ivec4( 1, 0, 0, 1), // Z+
		glm::ivec4(-1, 0, 0, 1)  // Z-
	};

	const glm::vec3 vertSideTable[6][4] {
		{	// X+
			glm::vec3( blockSize, blockSize, blockSize),
			glm::vec3( blockSize, blockSize,	  0.0f),
			glm::vec3( blockSize,	   0.0f, blockSize),
			glm::vec3( blockSize,	   0.0f,	  0.0f)
		},
		{	// X-
			glm::vec3(		0.0f, blockSize,	  0.0f),
			glm::vec3(		0.0f, blockSize, blockSize),
			glm::vec3(		0.0f,	   0.0f,	  0.0f),
			glm::vec3(		0.0f,	   0.0f, blockSize)
		},			   
		{	// Y+	   
			glm::vec3( blockSize, blockSize,	  0.0f),
			glm::vec3( blockSize, blockSize, blockSize),
			glm::vec3(		0.0f, blockSize,	  0.0f),
			glm::vec3(		0.0f, blockSize, blockSize)
		},			   
		{	// Y-	   
			glm::vec3(		0.0f,	   0.0f,	  0.0f),
			glm::vec3(		0.0f,	   0.0f, blockSize),
			glm::vec3( blockSize,	   0.0f,	  0.0f),
			glm::vec3( blockSize,	   0.0f, blockSize)
		},			   
		{	// Z+	   
			glm::vec3(		0.0f, blockSize, blockSize),
			glm::vec3( blockSize, blockSize, blockSize),
			glm::vec3(		0.0f,	   0.0f, blockSize),
			glm::vec3( blockSize,	   0.0f, blockSize)
		},			   
		{	// Z-	   
			glm::vec3( blockSize, blockSize,	  0.0f),
			glm::vec3(		0.0f, blockSize,	  0.0f),
			glm::vec3( blockSize,	   0.0f,	  0.0f),
			glm::vec3(		0.0f,	   0.0f,	  0.0f)
		},
	};

	void World::GenerateMeshes()
	{
		Profile::Begin("GenerateMeshes");
		for each (auto cmap in _chunkMap)
		{
			for each (auto kvp in cmap.second)
			{
				int xOff = cmap.first * Chunk::CHUNK_X;
				int zOff = kvp.first * Chunk::CHUNK_Z;

				// TODO: Generate the mesh(again, this should go in renderer)
				GenerateChunkMesh(kvp.second, xOff, zOff);
			}
		}
		Profile::End();
	}

	void World::GenerateChunkMesh(Chunk* chunk, int xOff, int zOff)
	{
		Profile::Begin("GenerateChunkMesh");
		// Vertex information
		struct Vertex
		{
			glm::vec4	position;
			glm::vec2	texCoord;
			glm::vec4	normal;
			glm::vec4	tangent;
			float		roughness;
		};

		std::vector<Vertex> vecVertices;
		std::vector<GLuint> vecIndices;
		Profile::Begin("Vertex Part");
		for (int x = 0; x < Chunk::CHUNK_X; x++)
		{
			for (int y = 0; y < Chunk::CHUNK_Y; y++)
			{
				for (int z = 0; z < Chunk::CHUNK_Z; z++)
				{
					int xx = xOff + x;
					int zz = zOff + z;
					unsigned char id = GetBlockAt(xx, y, zz);

					if (id == 0)
						continue; // Skip air

					Profile::Begin("All Sides");
					for (int side = 0; side < 6; side++)
					{
						glm::ivec3 offset = glm::ivec3(xx, y, zz) + vertNormTable[side];

						unsigned char nid = GetBlockAt(offset.x, offset.y, offset.z);

						// HACK: Set GetBlockAt to return -1 if there is no loaded chunk, so we can use this to clip the sides
						if (nid != 0)
							continue; // Skip sides that are not visible

						Profile::Begin("Side");
						// Setup our indices - It is fine to do this before the vertices, since our triangle order doesn't change
						int vertStartIndex = vecVertices.size();
						// Triangle 1
						vecIndices.push_back(vertStartIndex);		vecIndices.push_back(vertStartIndex + 2);	vecIndices.push_back(vertStartIndex + 1);
						// Triangle 2
						vecIndices.push_back(vertStartIndex + 1);	vecIndices.push_back(vertStartIndex + 2);	vecIndices.push_back(vertStartIndex + 3);

						// Setup our vertices
						int texIndex = _atlasDiffuse->GetIndex(BLOCKS[id]->GetTextureBySide(side)); // SPEED
						glm::vec2 minUV = _atlasDiffuse->MinUV(texIndex);
						glm::vec2 maxUV = _atlasDiffuse->MaxUV(texIndex);
						float roughness = ((id == 3 && side == 2) ? 0.35f : 0.8f);

						vecVertices.push_back(Vertex {
							glm::vec4(xx + vertSideTable[side][0].x, y + vertSideTable[side][0].y, zz + vertSideTable[side][0].z, 1),		// Position
							glm::vec2(minUV.x, minUV.y),		// TexCoords
							glm::vec4(vertNormTable[side].x, vertNormTable[side].y, vertNormTable[side].z, 0),		// Normal
							glm::vec4(vertTangentTable[side].x, vertTangentTable[side].y, vertTangentTable[side].z, vertTangentTable[side].w),		// Tangent
							roughness
						});
						vecVertices.push_back(Vertex {
							glm::vec4(xx + vertSideTable[side][1].x, y + vertSideTable[side][1].y, zz + vertSideTable[side][1].z, 1),		// Position
							glm::vec2(maxUV.x, minUV.y),		// TexCoords
							glm::vec4(vertNormTable[side].x, vertNormTable[side].y, vertNormTable[side].z, 0),		// Normal
							glm::vec4(vertTangentTable[side].x, vertTangentTable[side].y, vertTangentTable[side].z, vertTangentTable[side].w),		// Tangent
							roughness
						});
						vecVertices.push_back(Vertex {
							glm::vec4(xx + vertSideTable[side][2].x, y + vertSideTable[side][2].y, zz + vertSideTable[side][2].z, 1),		// Position
							glm::vec2(minUV.x, maxUV.y),		// TexCoords
							glm::vec4(vertNormTable[side].x, vertNormTable[side].y, vertNormTable[side].z, 0),		// Normal
							glm::vec4(vertTangentTable[side].x, vertTangentTable[side].y, vertTangentTable[side].z, vertTangentTable[side].w),		// Tangent
							roughness
						});
						vecVertices.push_back(Vertex {
							glm::vec4(xx + vertSideTable[side][3].x, y + vertSideTable[side][3].y, zz + vertSideTable[side][3].z, 1),		// Position
							glm::vec2(maxUV.x, maxUV.y),		// TexCoords
							glm::vec4(vertNormTable[side].x, vertNormTable[side].y, vertNormTable[side].z, 0),		// Normal
							glm::vec4(vertTangentTable[side].x, vertTangentTable[side].y, vertTangentTable[side].z, vertTangentTable[side].w),		// Tangent
							roughness
						});
						Profile::End();
					}
					Profile::End();
				}
			}
		}
		Profile::End();
		Profile::Begin("GL Part");

		// Generate our vertex buffer and index buffer
		glGenBuffers(1, &chunk->_vbo);
		glGenBuffers(1, &chunk->_ibo);

		// Generate our vertex array
		glGenVertexArrays(1, &chunk->_vao);

		// Bind our vertex array
		glBindVertexArray(chunk->_vao);


		// Tell GL that our _vbo is an array buffer(vertex buffer) and bind it to the vertex array
		glBindBuffer(GL_ARRAY_BUFFER, chunk->_vbo);
		// Tell GL that our _ibo is an element array buffer(index buffer) and bind it to the vertex array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->_ibo);

		// Tell GL how big our selected array buffer(_vbo) is, where it is in memory(vertices) and how to handle it
		glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(Vertex), &vecVertices[0], GL_STATIC_DRAW);

		// Tell GL how big our selected element array buffer(_ibo) is, where it is in memory(indices) and how to handle it
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(GLuint), &vecIndices[0], GL_STATIC_DRAW);

		// Tell GL the structure of our array buffer(_vbo)
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::texCoord)));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::texCoord) + sizeof(Vertex::normal)));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::texCoord) + sizeof(Vertex::normal) + sizeof(Vertex::tangent)));

		// Unbind our vertex array so that no further attempts to change a vertex array wont affect _vao
		glBindVertexArray(0);

		chunk->_indexCount = vecIndices.size();
		Profile::End();
		Profile::End();
	}

	/*
	This is my attempt at an optimized mesh generation
	The main idea is to look at only empty spaces, and generate the mesh for any adjacent solid blocks
	TESTED - This doesn't work, at least in the existing world data, because each chunk is roughly 50-50
			filled blocks vs empty blocks, which means this does just as much work as the other method
			IDEA - Use some form of octtree to split up the chunk, scaning to see if a oct is empty, then
			splitting it up more if its not empty might be faster then the current method
			To get the most out of this method, it might be better to store the world itself as an octree,
			this way it is already calculated down to full/empty leaves
	*/
	void World::GenerateChunkMesh2(Chunk* chunk, int xOff, int zOff)
	{
		Profile::Begin("GenerateChunkMesh");
		// Vertex information
		struct Vertex
		{
			glm::vec4	position;
			glm::vec2	texCoord;
			glm::vec4	normal;
			glm::vec4	tangent;
			float		roughness;
		};

		std::vector<Vertex> vecVertices;
		std::vector<GLuint> vecIndices;
		Profile::Begin("Vertex Part");
		for (int x = 0; x < Chunk::CHUNK_X; x++)
		{
			for (int z = 0; z < Chunk::CHUNK_Z; z++)
			{
				for (int y = 0; y < Chunk::CHUNK_Y; y++)
				{
					int xx = xOff + x;
					int zz = zOff + z;
					unsigned char id = GetBlockAt(xx, y, zz);

					if (id != 0)
						continue; // Skip any opaque blocks

					Profile::Begin("All Sides");
					for (int side = 0; side < 6; side++)
					{
						float dx = xx + vertNormTable[side].x;
						float dy = y + vertNormTable[side].y;
						float dz = zz + vertNormTable[side].z;
						//glm::ivec3 offset = glm::ivec3(xx, y, zz) + vertNormTable[side];
						int modSide = side + (1 - ((side % 2) * 2)); // This calculates the side of the adjacent block

						unsigned char nid = GetBlockAt(dx, dy, dz);

						if (nid == 0)
							continue; // Skip any non-opaque blocks, TODO: Alpha mesh pass

						Profile::Begin("Side");
						// Setup our indices - It is fine to do this before the vertices, since our triangle order doesn't change
						int vertStartIndex = vecVertices.size();
						// Triangle 1
						vecIndices.push_back(vertStartIndex);		vecIndices.push_back(vertStartIndex + 2);	vecIndices.push_back(vertStartIndex + 1);
						// Triangle 2
						vecIndices.push_back(vertStartIndex + 1);	vecIndices.push_back(vertStartIndex + 2);	vecIndices.push_back(vertStartIndex + 3);

						// Setup our vertices
						int texIndex = _atlasDiffuse->GetIndex(BLOCKS[nid]->GetTextureBySide(modSide));
						glm::vec2 minUV = _atlasDiffuse->MinUV(texIndex);
						glm::vec2 maxUV = _atlasDiffuse->MaxUV(texIndex);
						float roughness = ((nid == 3 && modSide == 2) ? 0.35f : 0.8f);

						vecVertices.push_back(Vertex {
							glm::vec4(dx + vertSideTable[modSide][0].x, dy + vertSideTable[modSide][0].y, dz + vertSideTable[modSide][0].z, 1),		// Position
							glm::vec2(minUV.x, minUV.y),		// TexCoords
											  glm::vec4(vertNormTable[modSide].x, vertNormTable[modSide].y, vertNormTable[modSide].z, 0),		// Normal
											  glm::vec4(vertTangentTable[modSide].x, vertTangentTable[modSide].y, vertTangentTable[modSide].z, vertTangentTable[modSide].w),		// Tangent
											  roughness
						});
						vecVertices.push_back(Vertex {
							glm::vec4(dx + vertSideTable[modSide][1].x, dy + vertSideTable[modSide][1].y, dz + vertSideTable[modSide][1].z, 1),		// Position
							glm::vec2(maxUV.x, minUV.y),		// TexCoords
											  glm::vec4(vertNormTable[modSide].x, vertNormTable[modSide].y, vertNormTable[modSide].z, 0),		// Normal
											  glm::vec4(vertTangentTable[modSide].x, vertTangentTable[modSide].y, vertTangentTable[modSide].z, vertTangentTable[modSide].w),		// Tangent
											  roughness
						});
						vecVertices.push_back(Vertex {
							glm::vec4(dx + vertSideTable[modSide][2].x, dy + vertSideTable[modSide][2].y, dz + vertSideTable[modSide][2].z, 1),		// Position
							glm::vec2(minUV.x, maxUV.y),		// TexCoords
											  glm::vec4(vertNormTable[modSide].x, vertNormTable[modSide].y, vertNormTable[modSide].z, 0),		// Normal
											  glm::vec4(vertTangentTable[modSide].x, vertTangentTable[modSide].y, vertTangentTable[modSide].z, vertTangentTable[modSide].w),		// Tangent
											  roughness
						});
						vecVertices.push_back(Vertex {
							glm::vec4(dx + vertSideTable[modSide][3].x, dy + vertSideTable[modSide][3].y, dz + vertSideTable[modSide][3].z, 1),		// Position
							glm::vec2(maxUV.x, maxUV.y),		// TexCoords
											  glm::vec4(vertNormTable[modSide].x, vertNormTable[modSide].y, vertNormTable[modSide].z, 0),		// Normal
											  glm::vec4(vertTangentTable[modSide].x, vertTangentTable[modSide].y, vertTangentTable[modSide].z, vertTangentTable[modSide].w),		// Tangent
											  roughness
						});
						Profile::End();
					}
					Profile::End();
				}
			}
		}
		Profile::End();
		Profile::Begin("GL Part");

		// Generate our vertex buffer and index buffer
		glGenBuffers(1, &chunk->_vbo);
		glGenBuffers(1, &chunk->_ibo);

		// Generate our vertex array
		glGenVertexArrays(1, &chunk->_vao);

		// Bind our vertex array
		glBindVertexArray(chunk->_vao);


		// Tell GL that our _vbo is an array buffer(vertex buffer) and bind it to the vertex array
		glBindBuffer(GL_ARRAY_BUFFER, chunk->_vbo);
		// Tell GL that our _ibo is an element array buffer(index buffer) and bind it to the vertex array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->_ibo);

		// Tell GL how big our selected array buffer(_vbo) is, where it is in memory(vertices) and how to handle it
		glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(Vertex), &vecVertices[0], GL_STATIC_DRAW);

		// Tell GL how big our selected element array buffer(_ibo) is, where it is in memory(indices) and how to handle it
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(GLuint), &vecIndices[0], GL_STATIC_DRAW);

		// Tell GL the structure of our array buffer(_vbo)
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position)));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::texCoord)));
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::texCoord) + sizeof(Vertex::normal)));
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(sizeof(Vertex::position) + sizeof(Vertex::texCoord) + sizeof(Vertex::normal) + sizeof(Vertex::tangent)));

		// Unbind our vertex array so that no further attempts to change a vertex array wont affect _vao
		glBindVertexArray(0);

		chunk->_indexCount = vecIndices.size();
		Profile::End();
		Profile::End();
	}

#pragma endregion
}