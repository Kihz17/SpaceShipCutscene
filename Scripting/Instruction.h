#pragma once

#include "Scene.h"

#include <string>

class Instruction
{
public:
	Instruction(const std::string& handle) : handle(handle) {}

	virtual void Execute(float deltaTime, Scene* scene) = 0;
	virtual bool IsComplete() const = 0;

	virtual void Reset() = 0;

	inline Ref<GameObject> GetGameObject(Scene* scene) const 
	{
		if (this->handle == "Camera" || this->handle == "camera")
		{
			return scene->camera;
		}

		return scene->GetGameObject(this->handle); 
	}

protected:
	std::string handle;
};