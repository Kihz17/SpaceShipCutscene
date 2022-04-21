#pragma once

#include "SceneMeshData.h"
#include "SceneBoundingBox.h"
#include "Light.h"

class Scene;
class ScenePanel
{
public:
	ScenePanel(Scene* scene);
	virtual ~ScenePanel();

	inline void SetMeshData(Ref<SceneMeshData> meshData) { this->meshData = meshData; }
	inline void SetBoundingBox(Ref<SceneBoundingBox> boundingbox) { this->boundingBox = boundingbox; }
	inline void SetLight(Ref<Light> light) { this->light = light; }
	void OnUpdate(float deltaTime);

	inline Ref<SceneMeshData> GetEditMesh() const { return this->meshData; }

private:

	void RenderSubmeshTextureData(Ref<SubmeshData> submeshData, const std::string& submeshName);

	Scene* scene;
	Ref<SceneMeshData> meshData;
	Ref<Light> light;
	Ref<SceneBoundingBox> boundingBox;
	std::string sceneName;
	std::string sceneLoadName;
	std::string textureName;
	std::string lightType;
	std::string loadScriptName;
	std::string boundingBoxName;
	std::string submeshFile;

	int selectedTexture;
	int selectedFilter;
	int selectedWrap;
	bool genMipMaps;

	static const char* textureTypes[];
	static const char* filterTypes[];
	static const char* wrapTypes[];
};