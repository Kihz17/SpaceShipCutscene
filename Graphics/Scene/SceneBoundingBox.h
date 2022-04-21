#pragma once

#include "pch.h"
#include "GameObject.h"

#include <yaml-cpp/yaml.h>

#include <string>

class SceneBoundingBox : public GameObject
{
public:
	SceneBoundingBox(const std::string& name, glm::vec3 position, glm::vec3 min, const glm::vec3 max);

	void Draw(Ref<Shader> shader);

	virtual void Save(YAML::Emitter& emitter) const;
	static Ref<SceneBoundingBox> StaticLoad(const YAML::Node& node);

	inline virtual glm::vec3 GetPositionSafe() const override { return this->position; };
	inline virtual glm::vec3& GetPosition() override { return this->position; };
	inline virtual glm::vec3 GetDirectionSafe() const override { return this->orientation; }
	inline virtual glm::vec3& GetDirection() override { return this->orientation; }
	inline virtual AABB& GetBoundingBox() override { return this->boundingBox; };
	inline virtual AABB GetBoundingBoxSafe() const override { return this->boundingBox; };

	static Ref<SceneBoundingBox> CreateDefault(const std::string& name, glm::vec3 position);

private:
	AABB boundingBox;
	glm::vec3 position;
	glm::vec3 orientation;
};