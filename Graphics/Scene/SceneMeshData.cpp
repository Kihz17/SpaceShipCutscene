#include "SceneMeshData.h"
#include "MeshManager.h"
#include "YAMLOverloads.h"
#include "Scene.h"

#include <glm/gtx/matrix_decompose.hpp>

SceneMeshData::SceneMeshData(const std::string& name, Ref<Mesh> mesh)
	: GameObject(name), mesh(mesh), 
	position(0.0f), 
	orientation(0.0f, 0.0f, 0.0f), 
	scale(1.0f, 1.0f, 1.0f) 
{
	std::unordered_map<std::string, Submesh> ::iterator it;
	for (it = mesh->GetSubmeshes().begin(); it != mesh->GetSubmeshes().end(); it++)
	{
		Ref<SubmeshData> data = CreateRef<SubmeshData>(GameObject::GenerateFriendlyName(), it->second.boundingBox);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(it->second.transform, scale, rotation, translation, skew, perspective);

		data->position = position;
		data->orientation = glm::eulerAngles(rotation);
		data->scale = scale;
		this->submeshData.insert(std::make_pair(it->first, data));
	}
}

SceneMeshData::~SceneMeshData()
{
	std::cout << "Destroying mesh data";
}

void SceneMeshData::Save(YAML::Emitter& emitter) const
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "UUID" << YAML::Value << this->uuid;
	emitter << YAML::Key << "Name" << YAML::Value << this->friendlyName;
	emitter << YAML::Key << "Path" << YAML::Value << SerializeUtils::SavePath(this->mesh->GetPath());
	emitter << YAML::Key << "ParentPosition" << YAML::Value << this->position;
	emitter << YAML::Key << "ParentOrientation" << YAML::Value << this->orientation;
	emitter << YAML::Key << "ParentScale" << YAML::Value << this->scale;

	emitter << YAML::Key << "Submeshes" << YAML::Value << YAML::BeginSeq;
	for (const std::pair<std::string, Ref<SubmeshData>>& entry : this->submeshData)
	{
		const Ref<SubmeshData> subMeshData = entry.second;
		emitter << YAML::BeginMap;

		if (this->externalSubmeshes.find(entry.first) != this->externalSubmeshes.end()) // This submesh was externally added, save that
		{
			emitter << YAML::Key << "SubmeshPath" << YAML::Value << SerializeUtils::SavePath(this->externalSubmeshes.at(entry.first));
		}
		emitter << YAML::Key << "SubmeshName" << YAML::Value << entry.first;
		emitter << YAML::Key << "GameObjectName" << YAML::Value << entry.second->GetName();
		emitter << YAML::Key << "Position" << YAML::Value << subMeshData->position;
		emitter << YAML::Key << "Orientation" << YAML::Value << subMeshData->orientation;
		emitter << YAML::Key << "Scale" << YAML::Value << subMeshData->scale;
		emitter << YAML::Key << "AlphaTransparency" << YAML::Value << subMeshData->alphaTransparency;

		emitter << YAML::Key << "Textures" << YAML::Value << YAML::BeginSeq;
		for (const Ref<SceneTextureData>& textureData : subMeshData->textures)
		{
			textureData->Save(emitter);
		}
		emitter << YAML::EndSeq;

		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;
	emitter << YAML::EndMap;
}

Ref<SceneMeshData> SceneMeshData::StaticLoad(Scene* scene, const YAML::Node& node)
{
	std::string path = SerializeUtils::LoadPath(node["Path"].as<std::string>());
	std::string name = node["Name"].as<std::string>();
	Ref<Mesh> mesh = MeshManager::LoadMesh(path);

	Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(name, mesh);
	meshData->uuid = node["UUID"].as<uint64_t>();
	meshData->position = node["ParentPosition"].as<glm::vec3>();
	meshData->orientation = node["ParentOrientation"].as<glm::vec3>();
	meshData->scale = node["ParentScale"].as<glm::vec3>();

	const YAML::Node& submeshNode = node["Submeshes"];
	if (submeshNode)
	{
		YAML::const_iterator submeshIt;
		for (submeshIt = submeshNode.begin(); submeshIt != submeshNode.end(); submeshIt++)
		{
			YAML::Node childNode = (*submeshIt);

			std::string submeshKey = childNode["SubmeshName"].as<std::string>();
			std::unordered_map<std::string, Ref<SubmeshData>>::iterator foundSubIt = meshData->submeshData.find(submeshKey);
			Ref<SubmeshData> subMeshData = nullptr;
			if (foundSubIt != meshData->submeshData.end()) // Submesh data was part of the model file
			{
				subMeshData = foundSubIt->second;
			}
			else // Externally added submesh
			{
				std::string submeshPath = SerializeUtils::LoadPath(childNode["SubmeshPath"].as<std::string>());
				std::string submeshGameObjectName = childNode["GameObjectName"].as<std::string>();
				meshData->AddSubmesh(scene, submeshPath, submeshGameObjectName);
				subMeshData = meshData->submeshData[submeshKey];
			}

			subMeshData->position = childNode["Position"].as<glm::vec3>();
			subMeshData->orientation = childNode["Orientation"].as<glm::vec3>();
			subMeshData->scale = childNode["Scale"].as<glm::vec3>();
			subMeshData->alphaTransparency = childNode["AlphaTransparency"].as<float>();

			const YAML::Node& textureNode = childNode["Textures"];
			if (textureNode)
			{
				YAML::const_iterator it;
				for (it = textureNode.begin(); it != textureNode.end(); it++)
				{
					YAML::Node textureChildNode = (*it);
					Ref<SceneTextureData> textureData = SceneTextureData::StaticLoad(textureChildNode);
					subMeshData->AddTexture(textureData);
				}
			}
		}
	}

	return meshData;
}

void SceneMeshData::AddTexture(Ref<Texture> texture, float ratio, float scale)
{
	Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(texture, ratio, scale);
	std::unordered_map<std::string, Ref<SubmeshData>>::iterator it;
	for (it = this->submeshData.begin(); it != this->submeshData.begin(); it++)
	{
		it->second->AddTexture(textureData);
	}
}

void SceneMeshData::AddTexture(Ref<SceneTextureData> textureData)
{
	std::unordered_map<std::string, Ref<SubmeshData>>::iterator it;
	for (it = this->submeshData.begin(); it != this->submeshData.begin(); it++)
	{
		it->second->AddTexture(textureData);
	}
}

void SceneMeshData::AddSubmesh(Scene* scene, const std::string& filePath, std::string name)
{
	this->mesh->AddSubmesh(filePath);
	std::unordered_map<std::string, Submesh> ::iterator it;
	for (it = mesh->GetSubmeshes().begin(); it != mesh->GetSubmeshes().end(); it++)
	{
		if (this->submeshData.find(it->first) != this->submeshData.end()) // Not new submesh
		{
			continue;
		}

		this->externalSubmeshes.insert(std::make_pair(it->first, filePath));
		Ref<SubmeshData> data = CreateRef<SubmeshData>(name, it->second.boundingBox);

		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 translation;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(it->second.transform, scale, rotation, translation, skew, perspective);

		data->position = position;
		data->orientation = glm::eulerAngles(rotation);
		data->scale = scale;

		std::stringstream ss;
		ss << this->GetName() << "-" << it->first;
		this->submeshData.insert(std::make_pair(it->first, data));
		scene->AddGameObject(std::static_pointer_cast<GameObject>(data));
	}
}

Ref<SceneMeshData> SceneMeshData::Duplicate(Ref<SceneMeshData> meshData)
{
	Ref<SceneMeshData> newMeshData = CreateRef<SceneMeshData>(GameObject::GenerateFriendlyName(), meshData->mesh);
	newMeshData->position = meshData->position;
	newMeshData->orientation = meshData->orientation;
	newMeshData->scale = meshData->scale;
	newMeshData->submeshData.clear();

	std::unordered_map<std::string, Ref<SubmeshData>>::iterator it;
	for (it = meshData->submeshData.begin(); it != meshData->submeshData.end(); it++)
	{
		Ref<SubmeshData> copyFrom = it->second;

		Ref<SubmeshData> submeshData = CreateRef<SubmeshData>(copyFrom->GetName(), copyFrom->GetBoundingBoxSafe());
		submeshData->alphaTransparency = copyFrom->alphaTransparency;
		submeshData->hasAlphaTransparentTexture = copyFrom->hasAlphaTransparentTexture;
		submeshData->orientation = copyFrom->orientation;
		submeshData->position = copyFrom->position;
		submeshData->scale = copyFrom->scale;

		for (const Ref<SceneTextureData>& textureData : copyFrom->textures)
		{
			Ref<SceneTextureData> duplicatedData = CreateRef<SceneTextureData>(textureData->texture, textureData->ratio, textureData->texCoordScale);
			submeshData->textures.push_back(duplicatedData);
		}

		newMeshData->submeshData.insert(std::make_pair(it->first, submeshData));
	}

	return newMeshData;
}

void SubmeshData::AddTexture(Ref<Texture> texture, float ratio, float scale)
{
	Ref<SceneTextureData> textureData = CreateRef<SceneTextureData>(texture, ratio, scale);

	if (texture->GetType() == TextureType::Heightmap)
	{
		if (!textures.empty() && textures[0]->texture->GetType() == TextureType::Heightmap)
		{
			std::cout << "WARNING: Tried to add multiple heightmap textures to one mesh!" << std::endl;
			return;
		}

		textures.insert(textures.begin(), textureData); // Make sure heightmap is at the front
		return;
	}
	else if (texture->GetType() == TextureType::Alpha)
	{
		this->hasAlphaTransparentTexture = true;
	}

	textures.push_back(textureData);
}

void SubmeshData::AddTexture(Ref<SceneTextureData> textureData)
{
	if (textureData->texture->GetType() == TextureType::Heightmap)
	{
		if (!textures.empty() && textures[0]->texture->GetType() == TextureType::Heightmap)
		{
			std::cout << "WARNING: Tried to add multiple heightmap textures to one mesh!" << std::endl;
			return;
		}

		textures.insert(textures.begin(), textureData); // Make sure heightmap is at the front
		return;
	}
	else if (textureData->texture->GetType() == TextureType::Alpha)
	{
		this->hasAlphaTransparentTexture = true;
	}

	textures.push_back(textureData);
}

glm::mat4 SubmeshData::GetTransform() const
{
	glm::mat4 transform(1.0f);
	transform *= glm::translate(glm::mat4(1.0f), this->position);
	transform *= glm::rotate(glm::mat4(1.0f), this->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f), this->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f), this->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	transform *= glm::scale(glm::mat4(1.0f), this->scale);
	return transform;
}