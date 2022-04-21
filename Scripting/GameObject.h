#pragma once

#include "AABB.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>

class GameObject
{
public:
	GameObject(const std::string& name) : friendlyName(name) {}
	virtual ~GameObject() = default;

	virtual glm::vec3 GetPositionSafe() const = 0;
	virtual glm::vec3& GetPosition() = 0;
	virtual glm::vec3 GetDirectionSafe() const = 0;
	virtual glm::vec3& GetDirection() = 0;
	virtual AABB& GetBoundingBox() = 0;
	virtual AABB GetBoundingBoxSafe() const = 0;

	inline virtual std::string& GetName() { return this->friendlyName; }

	static std::string GenerateFriendlyName();

protected:
	std::string friendlyName;
};