#pragma once


class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual bool Init() = 0;
	virtual void Shutdown() = 0;

	virtual bool Update(float deltaTime) { return true; }
	virtual void Render(float deltaTime) {}

	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
};