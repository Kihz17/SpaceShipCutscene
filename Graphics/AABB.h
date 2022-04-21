#pragma once

#include "pch.h"
#include "Shader.h"

#include <glm/glm.hpp>

class AABB 
{
public:
	AABB(const glm::vec3& min, const glm::vec3 max);
	virtual ~AABB();

	void Draw(Ref<Shader> shader, const glm::mat4& transform) const;

	bool IsInside(const glm::mat4& transform, const glm::vec3& position);

	glm::vec3 min;
	glm::vec3 max;

private:
	float* GetVertices() const;
};