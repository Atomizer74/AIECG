#pragma once

#include <glm/glm.hpp>



struct Particle
{
	glm::vec3	position;
	glm::vec3	velocity;

	glm::vec4	colour;
	float		size;

	float		lifetime;
	float		lifespan;

	int			texIndex;
};


struct ParticleVertex
{
	glm::vec4	position;
	glm::vec4	colour;
	glm::vec2	texcoord;
	int			texIndex;
};


class ParticleEmitter
{
public:
	ParticleEmitter();
	virtual ~ParticleEmitter();

	void Init(unsigned int maxParticles, unsigned int emitRate,
			  float lifespanMin, float lifespanMax,
			  float velocityMin, float velocityMax,
			  float sizeStart, float sizeEnd,
			  const glm::vec4& colourStart, const glm::vec4& colourEnd);


	void	Update(float deltaTime, const glm::mat4& cameraTransform);
	void	Render();


	const glm::mat4&	GetTransform() const { return _transform; }
	void				SetTransform(const glm::mat4& transform) { _transform = transform; }

protected:
	// Emits the particles
	virtual void	Emit();

	glm::mat4		_transform;

	// The particles
	Particle*		_particles;
	unsigned int	_aliveParticles;
	unsigned int	_maxParticles;

	// Render data
	unsigned int	_vao, _vbo, _ibo;
	ParticleVertex*	_vertexData;

	// Emitter properties
	float			_emitTimer;
	float			_emitRate;

	float			_lifespanMin, _lifespanMax;
	float			_velocityMin, _velocityMax;
	float			_sizeStart, _sizeEnd;
	glm::vec4		_colourStart, _colourEnd;
};