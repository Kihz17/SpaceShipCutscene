#pragma once

#include "pch.h"
#include "UUID.h"
#include "Mesh.h"
#include "Serializable.h"
#include "SceneTextureData.h"
#include "GameObject.h"

#include <glm/glm.hpp>

#include <unordered_map>

class Scene;
struct SubmeshData : GameObject
{
	SubmeshData(const std::string& name, AABB aabb) : GameObject(name), boundingBox(aabb) {}

	glm::vec3 position{ 0.0f, 0.0f, 0.0f };
	glm::vec3 orientation{ 1.0f, 1.0f, 1.0f };
	glm::vec3 scale{1.0f, 1.0f, 1.0f};
	std::vector<Ref<SceneTextureData>> textures;
	float alphaTransparency = 1.0f;
	bool hasAlphaTransparentTexture = false;
	AABB boundingBox;

	void AddTexture(Ref<Texture> texture, float ratio = 1.0f, float scale = 1.0f);
	void AddTexture(Ref<SceneTextureData> textureData);

	glm::mat4 GetTransform() const;

	inline virtual glm::vec3 GetPositionSafe() const override { return this->position; };
	inline virtual glm::vec3& GetPosition() override { return this->position; };
	inline virtual glm::vec3 GetDirectionSafe() const override { return this->orientation; }
	inline virtual glm::vec3& GetDirection() override { return this->orientation; }
	inline virtual AABB& GetBoundingBox() override { return this->boundingBox; };
	inline virtual AABB GetBoundingBoxSafe() const override { return this->boundingBox; };
};

class SceneMeshData : public GameObject
{
public:
	SceneMeshData(const std::string& name, Ref<Mesh> mesh);
	virtual ~SceneMeshData();

	virtual void Save(YAML::Emitter& emitter) const;

	virtual void AddTexture(Ref<Texture> texture, float ratio = 1.0f, float scale = 1.0f);
	virtual void AddTexture(Ref<SceneTextureData> textureData);

	virtual void AddSubmesh(Scene* scene, const std::string& filePath, std::string name = GameObject::GenerateFriendlyName());

	static Ref<SceneMeshData> StaticLoad(Scene* scene, const YAML::Node& node);

	inline virtual glm::vec3 GetPositionSafe() const override { return this->position; };
	inline virtual glm::vec3& GetPosition() override { return this->position; };
	inline virtual glm::vec3 GetDirectionSafe() const override { return this->orientation; }
	inline virtual glm::vec3& GetDirection() override { return this->orientation; }
	inline virtual AABB& GetBoundingBox() override { return this->mesh->GetBoundingBox(); };
	inline virtual AABB GetBoundingBoxSafe() const override { return this->mesh->GetBoundingBoxSafe(); };

	static Ref<SceneMeshData> Duplicate(Ref<SceneMeshData> meshData);

	UUID uuid;
	Ref<Mesh> mesh;

	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 scale;

	std::unordered_map<std::string, Ref<SubmeshData>> submeshData;

private:
	std::unordered_map<std::string, std::string> externalSubmeshes;
};