#include "Scene.h"
#include "MeshManager.h"
#include "Renderer.h"
#include "YAMLOverloads.h"
#include "ScriptParser.h"

#include <sstream>
#include <fstream>

Scene::Scene(Ref<Shader> shader)
	: shader(shader), 
	debugMode(false), 
	scenePanel(this), 
	currentMeshIndex(0), 
	currentLightIndex(0), 
	currentAABBIndex(0),
	showCurrentEdit(true),
	camera(NULL)
{
	std::stringstream ss;
	ss << SOLUTION_DIR << "Extern\\assets\\models\\ISO_Sphere.ply";
	this->lightMesh = MeshManager::LoadMesh(ss.str());
}

void Scene::Save(const std::string& path)
{
	if (path.empty())
	{
		return;
	}

	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";

	if (this->camera)
	{
		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "Position" << YAML::Value << this->camera->position;
		out << YAML::Key << "Direction" << YAML::Value << this->camera->direction;
		out << YAML::Key << "Yaw" << YAML::Value << this->camera->yaw;
		out << YAML::Key << "Pitch" << YAML::Value << this->camera->pitch;
		out << YAML::Key << "LastMouseX" << YAML::Value << this->camera->lastWindowX;
		out << YAML::Key << "LastMouseY" << YAML::Value << this->camera->lastWindowY;
		out << YAML::EndMap;
	}

	// Save EnvMap
	if (this->envMap)
	{
		out << YAML::Key << "EnvironmentMap" << YAML::Value;
		this->envMap->Save(out);
	}

	// Save lights
	out << YAML::Key << "Lights" << YAML::Value << YAML::BeginSeq;
	for (const Ref<Light>& light : this->lights)
	{
		light->Save(out);
	}
	out << YAML::EndSeq;

	// Save meshes
	out << YAML::Key << "Meshes" << YAML::Value << YAML::BeginSeq;
	std::vector<Ref<SceneMeshData>>::iterator meshIt;
	for (meshIt = this->meshes.begin(); meshIt != this->meshes.end(); meshIt++)
	{
		(*meshIt)->Save(out);
	}
	out << YAML::EndSeq;

	out << YAML::Key << "BoundingBoxes" << YAML::Value << YAML::BeginSeq;
	std::vector<Ref<SceneBoundingBox>>::iterator boundingBoxIt;
	for (boundingBoxIt = this->boundingBoxes.begin(); boundingBoxIt != this->boundingBoxes.end(); boundingBoxIt++)
	{
		(*boundingBoxIt)->Save(out);
	}
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream ofs(path);
	ofs << out.c_str();
}

void Scene::Load(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.good())
	{
		std::cout << "Scene does not exist!" << std::endl;
		return;
	}

	this->meshes.clear();
	this->alphaTransparentSubmeshes.clear();
	this->boundingBoxes.clear();
	this->lights.clear();
	this->currentMeshIndex = 0;
	this->scenePanel.SetMeshData(NULL);
	this->activeScripts.clear();
	for (const std::pair<std::string, InstructionScheduler*>& entry : this->scripts)
	{
		delete entry.second;
	}
	this->scripts.clear();

	std::stringstream ss;
	ss << ifs.rdbuf();

	shader->Bind();
	
	YAML::Node root = YAML::Load(ss.str());
	if (!root["Scene"])
	{
		std::cout << "Error loading scene file " << path << std::endl;
		return;
	}

	std::string sceneName = root["Scene"].as<std::string>();

	const YAML::Node& cameraNode = root["Camera"];
	if (cameraNode && this->camera)
	{
		this->camera->position = cameraNode["Position"].as<glm::vec3>();
		this->camera->direction = cameraNode["Direction"].as<glm::vec3>();
		this->camera->yaw = cameraNode["Yaw"].as<float>();
		this->camera->pitch = cameraNode["Pitch"].as<float>();
		if (cameraNode["LastMouseX"])
		{
			this->camera->lastWindowX = cameraNode["LastMouseX"].as<int>();
		}
		if (cameraNode["LastMouseY"])
		{
			this->camera->lastWindowY = cameraNode["LastMouseY"].as<int>();
		}
	}

	// Load Env map
	const YAML::Node& envMapNode = root["EnvironmentMap"];
	if (envMapNode)
	{		
		this->envMap = EnvironmentMap::StaticLoad(envMapNode);
	}

	// Load Lights
	const YAML::Node& lightNode = root["Lights"];
	if (lightNode)
	{
		YAML::const_iterator it;
		for (it = lightNode.begin(); it != lightNode.end(); it++)
		{
			YAML::Node childNode = (*it);
			Ref<Light> light = Light::StaticLoad(childNode);

			light->SendToShader();
			this->lights.push_back(light);
		}
	}

	const YAML::Node& meshNode = root["Meshes"];
	if (meshNode)
	{
		YAML::const_iterator it;
		for (it = meshNode.begin(); it != meshNode.end(); it++)
		{
			const YAML::Node& node = (*it);
			Ref<SceneMeshData> meshData = SceneMeshData::StaticLoad(this, node);
			AddMesh(meshData);
		}
	}

	const YAML::Node& boundingBoxNode = root["BoundingBoxes"];
	if (boundingBoxNode)
	{
		YAML::const_iterator it;
		for (it = boundingBoxNode.begin(); it != boundingBoxNode.end(); it++)
		{
			const YAML::Node& node = (*it);
			Ref<SceneBoundingBox> aabbData = SceneBoundingBox::StaticLoad(node);
			AddBoundingBox(aabbData);
		}
	}
}

void Scene::AddLight(const glm::vec3& position)
{
	int lightIndex = this->lights.size();
	if (lightIndex >= MAX_LIGHTS)
	{
		std::cout << "The maximum number of lights has been reached. You must increase the MAX_LIGHTS value in LightManager.h AND the fragment shader to inccrease the number of lights." << std::endl;
		return;
	}

	Ref<Light> light = CreateRef<Light>(lightIndex);
	light->position = glm::vec4(position, 1.0f);

	shader->Bind();
	light->SendToShader();
	this->lights.push_back(light);
}

void Scene::AddMesh(Ref<SceneMeshData> meshData)
{
	this->gameObjects.insert(std::make_pair(meshData->GetName(), std::static_pointer_cast<GameObject>(meshData)));
	this->meshes.push_back(meshData);

	// Try to find alpha transparent submeshes
	std::unordered_map<std::string, Ref<SubmeshData>>::iterator it;
	for (it = meshData->submeshData.begin(); it != meshData->submeshData.end(); it++)
	{
		this->gameObjects.insert(std::make_pair(it->second->GetName(), std::static_pointer_cast<GameObject>(it->second)));

		if (it->second->alphaTransparency < 1.0f || it->second->hasAlphaTransparentTexture)
		{
			std::pair<Ref<SceneMeshData>, Ref<SubmeshData>> alphaTransparentPair = std::make_pair(meshData, it->second);
			this->alphaTransparentSubmeshes.insert(std::make_pair(it->first, alphaTransparentPair));
		}
	}
}

void Scene::AddBoundingBox(Ref<SceneBoundingBox> boundingBox)
{
	std::unordered_map<std::string, Ref<GameObject>>::iterator it = this->gameObjects.find(boundingBox->GetName());
	if (it != this->gameObjects.end())
	{
		std::cout << "Bounding box with name '" << boundingBox->GetName() << "' already exists." << std::endl;
		return;
	}

	this->gameObjects.insert(std::make_pair(boundingBox->GetName(), std::static_pointer_cast<GameObject>(boundingBox)));
	this->boundingBoxes.push_back(boundingBox);
}

void Scene::OnUpdate(Ref<Camera> camera, float deltaTime)
{
	InstructionScheduler* scriptToRemove = nullptr;
	this->activeScripts.begin();
	for (InstructionScheduler* scheduler : this->activeScripts)
	{
		if (scheduler->IsComplete()) scriptToRemove = scheduler;
		scheduler->Execute(deltaTime, this);
	}

	if (scriptToRemove)
	{
		std::vector<InstructionScheduler*>::iterator it = activeScripts.begin();
		while (it != activeScripts.end())
		{
			if ((*it) == scriptToRemove) break;
			it++;
		}
		this->activeScripts.erase(it);
	}

	glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	// Draw environment map
	if (envMap)
	{
		this->envMap->Draw(this->shader, camera->position, glm::vec3(10.0f, 10.0f, 10.0f));
	}

	// Draw meshes
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);

	// Render opaque meshes
	for (const Ref<SceneMeshData>& meshData : this->meshes)
	{
		glm::mat4 parentTransform(1.0f);
		parentTransform *= glm::translate(glm::mat4(1.0f), meshData->position);
		parentTransform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		parentTransform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		parentTransform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		parentTransform *= glm::scale(glm::mat4(1.0f), meshData->scale);

		bool renderColorOverride = meshData == this->scenePanel.GetEditMesh() && this->showCurrentEdit;

		std::unordered_map<std::string, Ref<SubmeshData>>::iterator it;
		for (it = meshData->submeshData.begin(); it != meshData->submeshData.end(); it++)
		{
			std::unordered_map<std::string, Submesh>& submeshes = meshData->mesh->GetSubmeshes();
			if (submeshes.find(it->first) == submeshes.end())
			{
				std::cout << "WARNING: Submesh '" << it->first << "' was not found in mesh '" << meshData->GetName() << "'!";
				continue;
			}

			if (this->alphaTransparentSubmeshes.find(it->first) != this->alphaTransparentSubmeshes.end()) // This submesh is transparent, render it later
			{
				continue;
			}

			Ref<SubmeshData> submeshData = it->second;

			if (submeshData->alphaTransparency < 1.0f || submeshData->hasAlphaTransparentTexture) // Alpha transparency is now below 1, we need to flag this object to be sorted
			{
				this->alphaTransparentSubmeshes.insert(std::make_pair(it->first, std::make_pair(meshData, submeshData)));
			}

			glm::mat4 submeshTransform = parentTransform * submeshData->GetTransform();

			if (renderColorOverride)
			{
				Renderer::RenderSubmeshWithColorOverride(shader, submeshes[it->first], submeshTransform, glm::vec3(0.0f, 1.0f, 0.0f), debugMode, true);
			}
			else
			{
				Renderer::RenderSubmesh(shader, submeshes[it->first], submeshTransform, submeshData, debugMode);
			}
		}
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Sort the transparent meshes (use insertion sort here since they are sorted the majority of the time)
	std::vector<std::string> sortedAlphaTransparentSubmeshes(this->alphaTransparentSubmeshes.size());
	{
		int index = -1;
		std::unordered_map<std::string, std::pair<Ref<SceneMeshData>, Ref<SubmeshData>>>::iterator it;
		for (it = this->alphaTransparentSubmeshes.begin(); it != this->alphaTransparentSubmeshes.end(); it++)
		{
			if (index == -1) // First insert
			{
				index++;
				sortedAlphaTransparentSubmeshes[index] = it->first;
				continue;
			}

			int currentCompareIndex = index;
			while (currentCompareIndex >= 0)
			{
				const std::string& lastKey = sortedAlphaTransparentSubmeshes[currentCompareIndex];
				std::pair<Ref<SceneMeshData>, Ref<SubmeshData>>& entry = this->alphaTransparentSubmeshes[lastKey];
				glm::vec3 lastPosition = entry.first->position + glm::vec3(entry.second->position);
				glm::vec3 position = it->second.first->position + glm::vec3(it->second.second->position);

				glm::vec3 lastDifference = this->camera->position - lastPosition;
				glm::vec3 thisDifference = this->camera->position - position;
				if (glm::dot(thisDifference, thisDifference) < glm::dot(lastDifference, lastDifference))
				{
					sortedAlphaTransparentSubmeshes[currentCompareIndex + 1] = sortedAlphaTransparentSubmeshes[currentCompareIndex];
					currentCompareIndex--;
				}
				else // We found our spot
				{
					sortedAlphaTransparentSubmeshes[currentCompareIndex + 1] = it->first;
					index++;
					break;
				}
			}
		}
	}

	std::vector<std::string> noLongerTrasparent;
	for (const std::string& key : sortedAlphaTransparentSubmeshes)
	{
		std::unordered_map<std::string, std::pair<Ref<SceneMeshData>, Ref<SubmeshData>>>::iterator it = this->alphaTransparentSubmeshes.find(key);
		std::pair<Ref<SceneMeshData>, Ref<SubmeshData>>& data = it->second;
		Ref<SceneMeshData> meshData = data.first;

		std::unordered_map<std::string, Submesh>& submeshes = meshData->mesh->GetSubmeshes();
		if (submeshes.find(it->first) == submeshes.end())
		{
			std::cout << "WARNING: Submesh '" << it->first << "' was not found in mesh '" << meshData->GetName() << "'!";
			continue;
		}

		Ref<SubmeshData> submeshData = data.second;

		bool transparent = submeshData->alphaTransparency < 1.0f || submeshData->hasAlphaTransparentTexture;
		if (!transparent)
		{
			noLongerTrasparent.push_back(key);
		}

		glm::mat4 parentTransform(1.0f);
		parentTransform *= glm::translate(glm::mat4(1.0f), meshData->position);
		parentTransform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		parentTransform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		parentTransform *= glm::rotate(glm::mat4(1.0f), meshData->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		parentTransform *= glm::scale(glm::mat4(1.0f), meshData->scale);

		glm::mat4 transform = parentTransform * submeshData->GetTransform();
		if (meshData == this->scenePanel.GetEditMesh() && this->showCurrentEdit)
		{
			Renderer::RenderSubmeshWithColorOverride(shader, submeshes[it->first], transform, glm::vec3(0.0f, 1.0f, 0.0f), debugMode, true);
		}
		else
		{
			Renderer::RenderSubmesh(shader, submeshes[it->first], transform, submeshData, debugMode);
		}
	}

	for (const std::string& key : noLongerTrasparent)
	{
		this->alphaTransparentSubmeshes.erase(key);
	}

	glDisable(GL_BLEND);

	// Draw lights & bounding boxes
	if (debugMode)
	{
		for (const Ref<Light>& light : this->lights)
		{
			glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(light->position));
			{
				float distTo95Percent = Light::CalcApproxDistFromAtten(0.95f, 0.01, 10000.0f, light->attenuation.x, light->attenuation.y, light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo95Percent, distTo95Percent, distTo95Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(1.0f, 0.0f, 0.0f), this->debugMode, true);
			}
			{
				float distTo50Percent = Light::CalcApproxDistFromAtten(0.5f, 0.01, 10000.0f, light->attenuation.x, light->attenuation.y, light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo50Percent, distTo50Percent, distTo50Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(1.0f, 1.0f, 0.0f), this->debugMode, true);
			}
			{
				float distTo25Percent = Light::CalcApproxDistFromAtten(0.25f, 0.01, 10000.0f, light->attenuation.x, light->attenuation.y, light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo25Percent, distTo25Percent, distTo25Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(0.0f, 1.0f, 0.0f), this->debugMode, true);
			}
			{
				float distTo5Percent = Light::CalcApproxDistFromAtten(0.25f, 0.01, 10000.0f, light->attenuation.x, light->attenuation.y, light->attenuation.z, 50);
				glm::mat4 transform(1.0f);
				transform *= translate;
				transform *= glm::scale(glm::mat4(1.0f), glm::vec3(distTo5Percent, distTo5Percent, distTo5Percent));
				Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(0.0f, 0.0f, 1.0f), this->debugMode, true);
			}

			glm::mat4 transform(1.0f);
			transform *= translate;
			Renderer::RenderMeshWithColorOverride(this->shader, this->lightMesh, transform, glm::vec3(1.0f, 1.0f, 1.0f), this->debugMode, true);
		}

		for (const Ref<SceneBoundingBox>& bb : this->boundingBoxes)
		{
			bb->Draw(this->shader);
		}
	}

	scenePanel.OnUpdate(deltaTime);
}

void Scene::NextMesh()
{
	if (this->meshes.empty())
	{
		this->scenePanel.SetMeshData(NULL);
		return;
	}

	this->currentMeshIndex = std::min(this->currentMeshIndex + 1, (int)(this->meshes.size() - 1));
	this->scenePanel.SetMeshData(meshes[currentMeshIndex]);
}

void Scene::PreviousMesh()
{
	if (this->meshes.empty())
	{
		this->scenePanel.SetMeshData(NULL);
		return;
	}

	this->currentMeshIndex = std::max(this->currentMeshIndex - 1, 0);
	this->scenePanel.SetMeshData(meshes[currentMeshIndex]);
}

void Scene::NextLight()
{
	if (this->lights.empty())
	{
		this->scenePanel.SetLight(nullptr);
		return;
	}

	this->currentLightIndex = std::min(this->currentLightIndex + 1, (int)(this->lights.size() - 1));
	this->scenePanel.SetLight(lights[currentLightIndex]);
}

void Scene::PreviousLight()
{
	if (this->lights.empty())
	{
		this->scenePanel.SetLight(nullptr);
		return;
	}

	this->currentLightIndex = std::max(this->currentLightIndex - 1, 0);
	this->scenePanel.SetLight(lights[currentLightIndex]);
}

void Scene::NextAABB()
{
	if(this->boundingBoxes.empty())
	{
		this->scenePanel.SetBoundingBox(nullptr);
		return;
	}

	this->currentAABBIndex = std::min(this->currentAABBIndex + 1, (int)this->boundingBoxes.size() - 1);
	this->scenePanel.SetBoundingBox(this->boundingBoxes[this->currentAABBIndex]);
}

void Scene::PreviousAABB()
{
	if (this->boundingBoxes.empty())
	{
		this->scenePanel.SetBoundingBox(nullptr);
		return;
	}

	this->currentAABBIndex = std::max(this->currentAABBIndex - 1, 0);
	this->scenePanel.SetBoundingBox(this->boundingBoxes[this->currentAABBIndex]);
}

Ref<GameObject> Scene::GetGameObject(const std::string& handle)
{
	std::unordered_map<std::string, Ref<GameObject>>::iterator it = this->gameObjects.find(handle);
	if (it == this->gameObjects.end())
	{
		return nullptr;
	}

	return it->second;
}

void Scene::AddGameObject(Ref<GameObject> gameObject)
{
	this->gameObjects.insert(std::make_pair(gameObject->GetName(), gameObject));
}

void Scene::LoadScript(const std::string& path)
{
	InstructionScheduler* script = ScriptParser::ParseScript(path);
	if (script)
	{
		size_t lastSlash = path.find_last_of('\\');
		std::string fileName = lastSlash == std::string::npos ? path : path.substr(lastSlash + 1);

		size_t dotIndex = fileName.find_first_of('.');
		fileName = dotIndex == std::string::npos ? fileName : fileName.substr(0, dotIndex);
		scripts.insert({fileName, script});
	}
}

void Scene::RunScript(const std::string& name)
{
	std::unordered_map<std::string, InstructionScheduler*>::iterator it = this->scripts.find(name);
	if (it == this->scripts.end())
	{
		return;
	}

	InstructionScheduler* scheduler = it->second;
	if (std::count(this->activeScripts.begin(), this->activeScripts.end(), scheduler)) // Script is already active
	{
		return;
	}

	this->activeScripts.push_back(scheduler);
}