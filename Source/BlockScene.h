#pragma once

#include "Scene.h"
#include "Camera.h"
#include "GL_Program.h"
#include "TextureAtlas.h"

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <map>
#include "Profiler.h"

namespace Block
{
	float getHeight(int x, int y, float scale, int octaves, float amplitude, float persistance);

#pragma region BLOCKS
	class Block
	{
	public:
		unsigned char blockID;

		Block(unsigned char id) : blockID(id) {}
		virtual ~Block() {}

		virtual const char* GetTextureBySide(int side) const { return ""; }
		virtual int GetTexIDBySide(int side) const { return 0; }

		virtual bool IsFullBlock() const { return false; }
		virtual bool IsOpaqueBlock() const { return false; }
	};

	class BlockStone : public Block
	{
	public:
		BlockStone(unsigned char id) : Block(id) {}
		virtual ~BlockStone() {}

		virtual const char* GetTextureBySide(int side) const
		{
			return "stone";
		}
		virtual int GetTexIDBySide(int side) const
		{
			return 0;
		}
		virtual bool IsFullBlock() const { return true; }
		virtual bool IsOpaqueBlock() const { return true; }
	};

	class BlockDirt : public Block
	{
	public:
		BlockDirt(unsigned char id) : Block(id) {}
		virtual ~BlockDirt() {}

		virtual const char* GetTextureBySide(int side) const
		{
			return "dirt";
		}
		virtual int GetTexIDBySide(int side) const
		{
			return 1;
		}
		virtual bool IsFullBlock() const { return true; }
		virtual bool IsOpaqueBlock() const { return true; }
	};

	class BlockGrass : public Block
	{
	public:
		BlockGrass(unsigned char id) : Block(id) {}
		virtual ~BlockGrass() {}

		virtual const char* GetTextureBySide(int side) const
		{
			if (side == 2)
				return "grassTop";
			else if (side == 3)
				return "dirt";
			
			return "grassSide";
		}
		virtual int GetTexIDBySide(int side) const
		{
			if (side == 2)
				return 3;
			else if (side == 3)
				return 1;

			return 2;
		}
		virtual bool IsFullBlock() const { return true; }
		virtual bool IsOpaqueBlock() const { return true; }
	};

	static const Block* BLOCKS[] = {
		new Block(0), // Air
		new BlockStone(1),
		new BlockDirt(2),
		new BlockGrass(3)
	};
#pragma endregion

	class Chunk
	{
	public:
		static const int CHUNK_X = 16;
		static const int CHUNK_Y = 256;
		static const int CHUNK_Z = 16;

		unsigned char _blocks[CHUNK_X * CHUNK_Y * CHUNK_Z];

		Chunk()
		{
			// Zero the block data
			for (int i = 0; i < (CHUNK_X * CHUNK_Y * CHUNK_Z); i++)
				_blocks[i] = 0;
		}

		virtual ~Chunk()
		{
			// Cleanup the buffers and array
			glDeleteBuffers(1, &_vbo);
			glDeleteBuffers(1, &_ibo);
			glDeleteVertexArrays(1, &_vao);
		}

		unsigned char GetBlockAt(int x, int y, int z)
		{
			// Bounds checking
			if (x < 0 || x >= CHUNK_X || y < 0 || y >= CHUNK_Y || z < 0 || z >= CHUNK_Z)
				return 0;

			return _blocks[(x * CHUNK_Y * CHUNK_Z) + (z * CHUNK_Y) + y];
		}

		// Gets the highest block index at these local coords
		int GetHeightAt(int x, int z)
		{
			// Bounds checking
			if (x < 0 || x >= CHUNK_X || z < 0 || z >= CHUNK_Z)
				return CHUNK_Y;

			// Scan down from the height of the world
			for (int y = CHUNK_Y - 1; y >= 0; y--)
			{
				if (GetBlockAt(x, y, z) != 0)
					return y;
			}

			return -1; // No blocks in this column, this is bad
		}

		void SetBlockAt(int x, int y, int z, unsigned char id)
		{
			// Bounds checking
			if (x < 0 || x > CHUNK_X || y < 0 || y > CHUNK_Y || z < 0 || z > CHUNK_Z)
				return;

			_blocks[(x * CHUNK_Y * CHUNK_Z) + (z * CHUNK_Y) + y] = id;
		}

		void Render(float deltaTime)
		{
			// Re-bind our vertex array object that we set up previously
			glBindVertexArray(_vao);

			// Tell it what to draw from our bound buffers
			glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, 0);

			// Unbind out vertex array object so that other code doesn't accidently change our _vao
			// TODO: We may not need this?
			glBindVertexArray(0);
		}


		// Stuff for GL mesh
		// TODO: Should probably store the vertices and indices, and map them to the block
		// so that when a block is added/removed it can just add/remove the specific vertices/indicies
		GLuint _vao, _vbo, _ibo;

		GLuint _indexCount;
	};


	class World
	{
		std::map<int, std::map<int, Chunk*>> _chunkMap;

	public:
		// TODO: Remove these and make the world infinite
		static const int WORLD_X = 16;
		static const int WORLD_Z = 16;
		int worldSeed = 20;

		World()
		{
			Profile::Begin("Generate Texture Atlases");
			// TODO: Look into hardware texture arrays
			// Setup texture atlas
			_atlasDiffuse = new TextureAtlas(2, 512, glm::ivec4(0x66, 0x66, 0x66, 0xFF));
			_atlasDiffuse->AddTex("stone", "textures/blocks/stone.png", 0);
			_atlasDiffuse->AddTex("dirt", "textures/blocks/dirt.png", 1);
			_atlasDiffuse->AddTex("grassSide", "textures/blocks/grass_side.png", 2);
			_atlasDiffuse->AddTex("grassTop", "textures/blocks/grass_top.png", 3);
			_atlasDiffuse->Upload();

			_atlasNormal = new TextureAtlas(2, 512, glm::ivec4(0x00, 0x00, 0x00, 0x00));
			_atlasNormal->AddTex("stone", "textures/blocks/stone_n.png", 0);
			_atlasNormal->AddTex("dirt", "textures/blocks/dirt_n.png", 1);
			_atlasNormal->AddTex("grassSide", "textures/blocks/grass_side_n.png", 2);
			_atlasNormal->AddTex("grassTop", "textures/blocks/grass_top_n.png", 3);
			_atlasNormal->Upload();

			_atlasSpecular = new TextureAtlas(2, 512, glm::ivec4(0x00, 0x00, 0x00, 0xFF));
			_atlasSpecular->AddTex("stone", "textures/blocks/stone_s.png", 0);
			_atlasSpecular->AddTex("dirt", "textures/blocks/dirt_s.png", 1);
			_atlasSpecular->AddTex("grassSide", "textures/blocks/grass_side_s.png", 2);
			_atlasSpecular->AddTex("grassTop", "textures/blocks/grass_top_s.png", 3);
			_atlasSpecular->Upload();
			Profile::End();
		}

		virtual ~World()
		{
			// Cleanup texture atlas
			delete _atlasDiffuse;
			_atlasDiffuse = nullptr;
			delete _atlasNormal;
			_atlasNormal = nullptr;
			delete _atlasSpecular;
			_atlasSpecular = nullptr;
		}

		// Loads a chunk from disk, if chunk doesn't exist on disk, generate a new chunk, and save it
		void LoadChunk(int x, int z)
		{
			// For now, we just generate the chunk if it isn't in the map already
			// TODO: Chunk saving/loading
			// TODO: Probably want a separate world builder class for generation
			if (_chunkMap.count(x) != 0 && _chunkMap[x].count(z) != 0)
				return; // Don't generate a new chunk if we already have one

			Profile::Begin("Generate Chunk");
			// Step #1 - Generate Terrain Height
			int xOff = x * Chunk::CHUNK_X;
			int zOff = z * Chunk::CHUNK_Z;
			Chunk* newChunk = new Chunk();
			_chunkMap[x][z] = newChunk; // TODO: Can we mutex _chunkMap[x][z] to prevent other code from reading/writing this chunk while it is being generated?

			for (int x = 0; x < Chunk::CHUNK_X; x++)
			{
				for (int z = 0; z < Chunk::CHUNK_Z; z++)
				{
					int height = (int)(getHeight(worldSeed + xOff + x, worldSeed + zOff + z, 1.0f / (Chunk::CHUNK_X * Chunk::CHUNK_Z), 4, 20, 0.5f) + (Chunk::CHUNK_Y / 2));

					// Fill to height with stone
					for (int y = 0; y < height; y++)
					{
						newChunk->SetBlockAt(x, y, z, 1);
					}

					// Dirt height
					int dirtHeight = (int)(getHeight(worldSeed + xOff + x + height, worldSeed + zOff + z + height, 0.01f, 4, 4, 0.5f)) + 2;
					// Fill to height + dirtHeight with dirt
					for (int y = height; y <= height + dirtHeight; y++)
					{
						newChunk->SetBlockAt(x, y, z, 2);
					}

					// Fill top layer with grass
					int topBlock = GetHeightAt(xOff + x, zOff + z);
					unsigned char topID = GetBlockAt(xOff + x, topBlock, zOff + z);

					// Only set it if it is dirt
					if (topID == 2) // TODO: Have a better method for checking block types
						newChunk->SetBlockAt(x, topBlock, z, 3); // Convert the top block from dirt to grass
				}
			}
			Profile::End();
		}

		// Unloads the chunk from memory
		void UnloadChunk(int x, int z)
		{
			Chunk* chunk = _chunkMap[x][z];

			if (chunk == nullptr)
				return; // Nothing to unload

			_chunkMap[x][z] = nullptr;
			delete chunk;
		}

		unsigned char GetBlockAt(int x, int y, int z)
		{
			int chunkX = (int)floorf(x / (float)Chunk::CHUNK_X);
			int chunkZ = (int)floorf(z / (float)Chunk::CHUNK_Z);

			if (_chunkMap.count(chunkX) == 0 || _chunkMap[chunkX].count(chunkZ) == 0)
				return -1; // TODO: Generate/Load chunk if it isn't in the map, for now we just return air
						  // (NOTE: Also need to make sure we don't try to generate a chunk we are currently generating)
							// HACK: Changed this to return -1, this will clip the sides where no chunks are loaded

			Chunk* currentChunk = _chunkMap[chunkX][chunkZ];

			return currentChunk->GetBlockAt((x % Chunk::CHUNK_X), y, (z % Chunk::CHUNK_Z));
		}

		// Returns the highest block offset at these world coords
		int GetHeightAt(int x, int z)
		{
			int chunkX = (int)floorf(x / (float)Chunk::CHUNK_X);
			int chunkZ = (int)floorf(z / (float)Chunk::CHUNK_Z);

			if (_chunkMap.count(chunkX) == 0 || _chunkMap[chunkX].count(chunkZ) == 0)
				return -1; // TODO: Generate/Load chunk if it isn't in the map, for now we just return -1, this is bad

			return _chunkMap[chunkX][chunkZ]->GetHeightAt(x % Chunk::CHUNK_X, z % Chunk::CHUNK_Z); // TODO: Negative coords
		}

		// TODO: Rendering stuff - Move this elsewhere
		void Render(float deltaTime)
		{
			// Use our texture atlas
			_atlasDiffuse->Bind(0);
			_atlasNormal->Bind(1);
			_atlasSpecular->Bind(2);

			for each (auto cmap in _chunkMap)
			{
				for each (auto kvp in cmap.second)
				{
					kvp.second->Render(deltaTime);
				}
			}
		}

		TextureAtlas* _atlasDiffuse;
		TextureAtlas* _atlasNormal;
		TextureAtlas* _atlasSpecular;

		// TODO: This should go somewhere else, only within the renderer(client)
		void GenerateMeshes();
		void GenerateChunkMesh(Chunk* chunk, int xOff, int zOff);
		void GenerateChunkMesh2(Chunk* chunk, int xOff, int zOff);
	};


	class BlockScene : public Scene
	{
	public:
		BlockScene();
		virtual ~BlockScene();

		virtual bool Init();
		virtual void Shutdown();

		virtual void OnGUI(float deltaTime);
		virtual bool Update(float deltaTime);
		virtual void Render(float deltaTime);

		virtual void Activate();
		virtual void Deactivate();

	protected:
		Camera* _camera;

		Chunk* _chunk;
		World* _world;

		GL::Program* _program;
		float _time;

		// TODO: proper light class
		glm::vec4 _lightPosition;
		glm::vec4 _lightDirection;
	};
}