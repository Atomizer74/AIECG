#include "ParticleScene.h"

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>

#include "Gizmos.h"
#include "GL_Uniform.h"
#include "Image.h"


ParticleScene::ParticleScene() : _camera(nullptr) {}
ParticleScene::~ParticleScene() {}

bool ParticleScene::Init()
{
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//_camera = new Camera(glm::pi<float>() * 0.25f, 1280.0f / 720.0f, 0.1f, 1000.0f);
	_camera = new FlyCamera();
	_camera->LookAt(glm::vec3(10.0f), glm::vec3(0.0f));

	// Setup our particle emitter
	_emitter = new ParticleEmitter();
	_emitter->Init(1000, 500,
				   0.5f, 2.0f,
				   1.0f, 5.0f,
				   1.0f, 0.1f,
				   glm::vec4(1, 0, 0, 1), glm::vec4(1, 1, 0, 0.1f));

	// Set up our shaders
	_program = new GL::Program();

	GL::Shader vertShader(GL::ShaderType::VERTEX_SHADER, "./shaders/vertParticle.glsl");
	GL::Shader fragShader(GL::ShaderType::FRAGMENT_SHADER, "./shaders/fragParticle.glsl");

	_program->AddShader(vertShader);
	_program->AddShader(fragShader);
	_program->Link();

	// Setup our particle textures
	Image fire1("textures/fire1.png");
	Image fire2("textures/fire2.png");
	Image fire3("textures/fire3.png");

	// Upload the textures
	_texFire1 = fire1.Upload();
	_texFire2 = fire2.Upload();
	_texFire3 = fire3.Upload();

	return true;
}

void ParticleScene::Shutdown()
{
	// Cleanup the camera
	delete _camera;
	_camera = nullptr;

	// Cleanup textures
	glDeleteTextures(1, &_texFire1);
	glDeleteTextures(1, &_texFire2);
	glDeleteTextures(1, &_texFire3);

	// Cleanup particle emitter
	delete _emitter;

	// Cleanup shader
	delete _program;
}


bool ParticleScene::Update(float deltaTime)
{
	_camera->Update(deltaTime);

	// Update our particle emitter
	_emitter->Update(deltaTime, _camera->GetTransform());


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
	

	return true;
}

void ParticleScene::Render(float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // This can be removed for speed if needed

	Gizmos::draw(_camera->GetProjectionView());

	// Select our shader
	_program->Use();
	GL::Uniform::Set("projectionViewMatrix", _camera->GetProjectionView());

	// Tell the shader about the textures
	GL::Uniform::Set("tex1", 0);
	GL::Uniform::Set("tex2", 1);
	GL::Uniform::Set("tex3", 2);

	// Bind the particle textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texFire1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _texFire2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _texFire3);

	// Render the particles
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	_emitter->Render();
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Unbind the particle textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void ParticleScene::Activate()
{

}

void ParticleScene::Deactivate()
{

}
