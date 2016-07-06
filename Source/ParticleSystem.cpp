#include "ParticleSystem.h"
#include "gl_core_4_4.h"

#include <glm/ext.hpp>


ParticleEmitter::ParticleEmitter()
	: _transform(1),
	_particles(nullptr),
	_aliveParticles(0),
	_maxParticles(0),
	_vertexData(nullptr),
	_vao(0), _vbo(0), _ibo(0),
	_emitTimer(0), _emitRate(0)
{
}


ParticleEmitter::~ParticleEmitter()
{
	delete[] _particles;
	delete[] _vertexData;
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ibo);
	glDeleteVertexArrays(1, &_vao);
}


void ParticleEmitter::Init(unsigned int maxParticles, unsigned int emitRate,
						   float lifespanMin, float lifespanMax,
						   float velocityMin, float velocityMax,
						   float sizeStart, float sizeEnd,
						   const glm::vec4 & colourStart, const glm::vec4 & colourEnd)
{
	// Timers
	_emitTimer = 0;
	_emitRate = 1.0f / emitRate;

	// Store data
	_maxParticles = maxParticles;
	_lifespanMin = lifespanMin;
	_lifespanMax = lifespanMax;
	_velocityMin = velocityMin;
	_velocityMax = velocityMax;
	_sizeStart = sizeStart;
	_sizeEnd = sizeEnd;
	_colourStart = colourStart;
	_colourEnd = colourEnd;

	// Create particles
	_particles = new Particle[_maxParticles];
	_aliveParticles = 0;

	// Create vertex data, 4 vertices per particle
	_vertexData = new ParticleVertex[4 * _maxParticles];

	// create the index buffeer data for the particles
	// 6 indices per quad of 2 triangles
	// fill it now as it never changes
	unsigned int* indexData = new unsigned int[_maxParticles * 6];
	for (unsigned int i = 0; i < _maxParticles; ++i)
	{
		indexData[i * 6 + 0] = i * 4 + 0;
		indexData[i * 6 + 1] = i * 4 + 1;
		indexData[i * 6 + 2] = i * 4 + 2;

		indexData[i * 6 + 3] = i * 4 + 0;
		indexData[i * 6 + 4] = i * 4 + 2;
		indexData[i * 6 + 5] = i * 4 + 3;
	}

	// create opengl buffers
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ibo);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _maxParticles * 4 *
				 sizeof(ParticleVertex), _vertexData,
				 GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _maxParticles * 6 *
				 sizeof(unsigned int), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // colour
	glEnableVertexAttribArray(2); // texcoord
	glEnableVertexAttribArray(3); // texIndex
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
						  sizeof(ParticleVertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
						  sizeof(ParticleVertex), ((char*)0) + 16);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
						  sizeof(ParticleVertex), ((char*)0) + 32);
	glVertexAttribPointer(3, 1, GL_INT, GL_FALSE,
						  sizeof(ParticleVertex), ((char*)0) + 40);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] indexData;
}

void ParticleEmitter::Update(float deltaTime, const glm::mat4& cameraTransform)
{
	// Do no processing if there are no particles
	if (_maxParticles == 0)
		return;

	// Emit new particles
	_emitTimer += deltaTime;
	while (_emitTimer > _emitRate)
	{
		Emit();
		_emitTimer -= _emitRate;
	}

	// Update all alive particles
	for (unsigned int i = 0; i < _aliveParticles;)
	{
		Particle& particle = _particles[i];

		// Update life
		particle.lifetime += deltaTime;

		// If dead, swap it
		if (particle.lifetime >= particle.lifespan)
		{
			particle = _particles[--_aliveParticles];
		}
		else
		{
			// Update particle properties
			particle.position += particle.velocity * deltaTime;
			particle.size = glm::mix(_sizeStart, _sizeEnd, particle.lifetime / particle.lifespan);
			particle.colour = glm::mix(_colourStart, _colourEnd, particle.lifetime / particle.lifespan);

			// billboard / update vertex data
			glm::vec3 zAxis = glm::normalize(glm::vec3(cameraTransform[3]) - particle.position);
			glm::vec3 xAxis = glm::cross(glm::vec3(cameraTransform[1]), zAxis);
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);

			glm::mat4 billboard(glm::vec4(xAxis, 0),
								glm::vec4(yAxis, 0),
								glm::vec4(zAxis, 0),
								glm::vec4(0, 0, 0, 1));

			float hSize = particle.size * 0.5f;

			_vertexData[i * 4 + 0].position = billboard * glm::vec4( hSize, hSize, 0, 1) + glm::vec4(particle.position, 0);
			_vertexData[i * 4 + 1].position = billboard * glm::vec4(-hSize, hSize, 0, 1) + glm::vec4(particle.position, 0);
			_vertexData[i * 4 + 2].position = billboard * glm::vec4(-hSize,-hSize, 0, 1) + glm::vec4(particle.position, 0);
			_vertexData[i * 4 + 3].position = billboard * glm::vec4( hSize,-hSize, 0, 1) + glm::vec4(particle.position, 0);

			_vertexData[i * 4 + 0].colour = particle.colour;
			_vertexData[i * 4 + 1].colour = particle.colour;
			_vertexData[i * 4 + 2].colour = particle.colour;
			_vertexData[i * 4 + 3].colour = particle.colour;

			_vertexData[i * 4 + 0].texcoord = glm::vec2(1, 0);
			_vertexData[i * 4 + 1].texcoord = glm::vec2(0, 0);
			_vertexData[i * 4 + 2].texcoord = glm::vec2(0, 1);
			_vertexData[i * 4 + 3].texcoord = glm::vec2(1, 1);

			_vertexData[i * 4 + 0].texIndex = particle.texIndex;
			_vertexData[i * 4 + 1].texIndex = particle.texIndex;
			_vertexData[i * 4 + 2].texIndex = particle.texIndex;
			_vertexData[i * 4 + 3].texIndex = particle.texIndex;

			// jump to next particle
			++i;
		}
	}
}

void ParticleEmitter::Render()
{
	// Safety, only draw if there are particles to draw
	if (_aliveParticles == 0)
		return;

	// Update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, _aliveParticles * 4 * sizeof(ParticleVertex), _vertexData);

	// Draw particles
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, _aliveParticles * 6, GL_UNSIGNED_INT, 0);
}

void ParticleEmitter::Emit()
{
	// Return is no particles are avaliable
	if (_aliveParticles >= _maxParticles)
		return;

	// Get the first dead particle
	Particle& particle = _particles[_aliveParticles++];

	// Change the initial position of particles here, for different types of emitters
	glm::vec2 v = glm::diskRand(2.0f);
	particle.position = glm::vec3(_transform[3]) + glm::vec3(v.x, 0, v.y);

	// Generate random velocity
	float f = glm::linearRand(_velocityMin, _velocityMax);
	particle.velocity = glm::vec3(_transform[1]) * f;//glm::sphericalRand(f);

	// Set up lifetime
	particle.lifetime = 0;
	particle.lifespan = glm::linearRand(_lifespanMin, _lifespanMax);

	// Set up texture index
	particle.texIndex = glm::linearRand(0, 2);
}
