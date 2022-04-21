#pragma once

#include "SceneMeshData.h"
#include "SceneBoundingBox.h"
#include "UUID.h"
#include "EnvironmentMap.h"
#include "Light.h"
#include "Camera.h"
#include "ScenePanel.h"
#include "InstructionScheduler.h"

#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <algorithm>

class Scene
{
public:
	Scene(Ref<Shader> shader);

	void Save(const std::string& path);
	void Load(const std::string& path);

	inline void SetEnvMap(const Ref<EnvironmentMap> envMap) { this->envMap = envMap; }

	void AddLight(const glm::vec3& position);

	void AddMesh(Ref<SceneMeshData> meshData);
	void AddBoundingBox(Ref<SceneBoundingBox> boundingBox);

	void OnUpdate(Ref<Camera> camera, float deltaTime);

	void NextMesh();
	void PreviousMesh();

	void NextLight();
	void PreviousLight();

	void NextAABB();
	void PreviousAABB();

	void AddGameObject(Ref<GameObject> gameObject);
	Ref<GameObject> GetGameObject(const std::string& handle);

	void LoadScript(const std::string& path);
	void RunScript(const std::string& name);

	bool showCurrentEdit;
	bool debugMode;
	Ref<Camera> camera;

private:
	friend class ScenePanel;
	std::unordered_map<std::string, std::pair<Ref<SceneMeshData>, Ref<SubmeshData>>> alphaTransparentSubmeshes;
	std::vector<Ref<SceneMeshData>> meshes;

	std::vector<Ref<SceneBoundingBox>> boundingBoxes;

	int currentMeshIndex;
	int currentLightIndex;
	int currentAABBIndex;

	Ref<EnvironmentMap> envMap;
	std::vector<Ref<Light>> lights;

	Ref<Shader> shader;
	Ref<Mesh> lightMesh;
	static const unsigned int MAX_LIGHTS = 10; // This must match the value in the fragment shader

	ScenePanel scenePanel;

	std::unordered_map<std::string, Ref<GameObject>> gameObjects;
	std::unordered_map<std::string, InstructionScheduler*> scripts;
	std::vector<InstructionScheduler*> activeScripts;
};