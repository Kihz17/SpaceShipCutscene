#include "ScenePanel.h"
#include "Scene.h"

#include "TextureManager.h"

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_stdlib.h"

const char* ScenePanel::textureTypes[] = { "Diffuse", "Heightmap", "Discard", "Alpha"};
const char* ScenePanel::filterTypes[] = { "Linear", "Nearest" };
const char* ScenePanel::wrapTypes[] = { "Clamp", "Repeat" };

ScenePanel::ScenePanel(Scene* scene) :
	scene(scene), 
meshData(nullptr),
boundingBox(nullptr)
, selectedTexture(0),
selectedFilter(0),
selectedWrap(0),
genMipMaps(false)
{
}

ScenePanel::~ScenePanel()
{

}

void ScenePanel::OnUpdate(float deltaTime)
{
	ImGui::Begin("Mesh Editor");

	if (this->meshData)
	{
		std::string file = this->meshData->mesh->GetPath();
		file = file.substr(file.find_last_of('\\') + 1);
		ImGui::Text(file.c_str());

		ImGui::NewLine();
		ImGui::DragFloat3("Parent Position", (float*)&this->meshData->position);
		ImGui::DragFloat3("Parent Orientation", (float*)&this->meshData->orientation, 0.01f);
		ImGui::DragFloat3("Parent Scale", (float*)&this->meshData->scale, 0.01f);

		// Submesh data
		{
			ImGui::NewLine();
			ImGui::Text("===========================================================================");
			std::unordered_map<std::string, Ref<SubmeshData>>::iterator it;
			for (it = this->meshData->submeshData.begin(); it != this->meshData->submeshData.end(); it++)
			{
				Ref<SubmeshData> submeshData = it->second;
				{
					std::stringstream ss;
					ss << "Submesh: " << it->first;
					ImGui::Text(ss.str().c_str());
				}
				ImGui::NewLine();
				{
					std::stringstream ss;
					ss << this->meshData->GetName() << "-" << it->first << " Position";
					ImGui::DragFloat3(ss.str().c_str(), (float*)&submeshData->position, 0.01f);
				}
				{
					std::stringstream ss;
					ss << this->meshData->GetName() << "-" << it->first << " Orientation";
					ImGui::DragFloat3(ss.str().c_str(), (float*)&submeshData->orientation, 0.01f);
				}
				{
					std::stringstream ss;
					ss << this->meshData->GetName() << "-" << it->first << " Scale";
					ImGui::DragFloat3(ss.str().c_str(), (float*)&submeshData->scale, 0.01f);
				}
				{
					std::stringstream ss;
					ss << it->first << " Alpha Transparency";
					ImGui::DragFloat(ss.str().c_str(), (float*)&submeshData->alphaTransparency, 0.01f, 0.0f, 1.0f);
				}

				ImGui::NewLine();
				RenderSubmeshTextureData(submeshData, it->first);
				ImGui::Text("===========================================================================");
			}
		}

		ImGui::NewLine();
		if (ImGui::Button("Add Texture To Entire Mesh"))
		{
			std::stringstream ss;
			ss << SOLUTION_DIR << "Extern\\assets\\textures\\" << this->textureName;
			const char* textureTypeString = textureTypes[this->selectedTexture];
			TextureFilterType filterType = StringToFilterType(filterTypes[this->selectedFilter]);
			TextureWrapType wrapType = StringToWrapType(wrapTypes[this->selectedWrap]);
			if (textureTypeString == "Diffuse")
			{
				Ref<DiffuseTexture> texture = TextureManager::LoadDiffuseTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				this->meshData->AddTexture(texture);
			}
			else if (textureTypeString == "Heightmap")
			{
				Ref<HeightMapTexture> texture = TextureManager::LoadHeightmapTexture(ss.str(), filterType, wrapType, glm::vec3(0.0f, 0.0f, 0.0f), 1000.0f);
				this->meshData->AddTexture(texture);
			}
			else if (textureTypeString == "Discard")
			{
				Ref<DiscardTexture> texture = TextureManager::LoadDiscardTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				this->meshData->AddTexture(texture);
			}
			else if (textureTypeString == "Alpha")
			{
				Ref<AlphaTexture> texture = TextureManager::LoadAlphaTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				this->meshData->AddTexture(texture);
			}
		}

		ImGui::NewLine();
		ImGui::Text("Texture Data:");
		ImGui::InputText("Texture", &this->textureName);
		ImGui::Combo("Texture Type", &this->selectedTexture, textureTypes, IM_ARRAYSIZE(textureTypes));
		ImGui::Combo("Filter Type", &this->selectedFilter, filterTypes, IM_ARRAYSIZE(filterTypes));
		ImGui::Combo("Wrap Type", &this->selectedWrap, wrapTypes, IM_ARRAYSIZE(wrapTypes));
		ImGui::Checkbox("Gen Mip Maps", &this->genMipMaps);
		ImGui::Text("===========================================================================");

		ImGui::NewLine();
		if (ImGui::Button("Duplicate Mesh"))
		{
			this->scene->AddMesh(SceneMeshData::Duplicate(this->meshData));
		}

		ImGui::NewLine();
		ImGui::InputText("Submesh File Name", &this->submeshFile);
		if (ImGui::Button("Add Submesh"))
		{
			std::stringstream ss;
			ss << SOLUTION_DIR << "Extern\\assets\\models\\" << this->submeshFile;
			this->meshData->AddSubmesh(this->scene, ss.str());
		}
		ImGui::Text("===========================================================================");
	}

	if (this->light)
	{
		ImGui::NewLine();
		ImGui::DragFloat4("Light Position", (float*)&this->light->position);

		ImGui::NewLine();
		ImGui::DragFloat4("Diffuse", (float*)&this->light->diffuse, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat4("Specular", (float*)&this->light->specular, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat4("Attenuation", (float*)&this->light->attenuation, 0.001f);

		ImGui::NewLine();
		ImGui::DragFloat4("Direction", (float*)&this->light->direction, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat("InnerAngle", (float*)&this->light->innerAngle, 0.01f);

		ImGui::NewLine();
		ImGui::DragFloat("OuterAngle", (float*)&this->light->outerAngle, 0.01f);

		ImGui::NewLine();
		ImGui::InputText("Light Type", &this->lightType);


		if (ImGui::Button("Change Light Type"))
		{
			if (this->lightType == "DIRECTIONAL")
			{
				this->light->lightType = Light::LightType::DIRECTIONAL;
			}
			else if (this->lightType == "POINT")
			{
				this->light->lightType = Light::LightType::POINT;
			}
			else if (this->lightType == "SPOT")
			{
				this->light->lightType = Light::LightType::SPOT;
			}
		}

		this->light->SendToShader();

		ImGui::NewLine();
		ImGui::Checkbox("State", &this->light->state);
	}

	ImGui::NewLine();
	if (ImGui::Button("Add Light"))
	{
		scene->AddLight(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	ImGui::NewLine();
	if (this->boundingBox)
	{
		ImGui::DragFloat3("Bounding Box Min Point", (float*)&this->boundingBox->GetBoundingBox().min, 0.01f);
		ImGui::DragFloat3("Bounding Box Max Point", (float*)&this->boundingBox->GetBoundingBox().max, 0.01f);
		ImGui::DragFloat3("Bounding Box Position", (float*)&this->boundingBox->GetPosition(), 0.01f);
		ImGui::DragFloat3("Bounding Box Orientation", (float*)&this->boundingBox->GetDirection(), 0.01f);
		ImGui::NewLine();
	}

	ImGui::InputText("Bounding Box Name", &this->boundingBoxName);
	if (ImGui::Button("Add Bounding Box"))
	{
		if (boundingBoxName != "")
		{
			scene->AddBoundingBox(SceneBoundingBox::CreateDefault(this->boundingBoxName, this->scene->camera->position));
		}
	}

	ImGui::NewLine();
	ImGui::InputText("Load Script Name", &this->loadScriptName);
	if (ImGui::Button("Load Script"))
	{
		this->scene->LoadScript(this->loadScriptName);
	}

	{
		ImGui::NewLine();
		std::unordered_map<std::string, InstructionScheduler*>::iterator it = scene->scripts.begin();
		while (it != scene->scripts.end())
		{
			std::stringstream ss;
			ss << "Start Script " << it->first;
			if (ImGui::Button(ss.str().c_str()))
			{
				this->scene->RunScript(it->first);
			}
			it++;
		}	
	}

	ImGui::NewLine();
	ImGui::Text("Load Scene");
	ImGui::InputText("Load Scene Name", &this->sceneLoadName);
	if (ImGui::Button("Load"))
	{
		this->scene->Load(this->sceneLoadName);
	}

	ImGui::Text("");
	ImGui::Text("Save Scene");
	ImGui::InputText("Save Scene Name", &this->sceneName);
	if (ImGui::Button("Save"))
	{
		this->scene->Save(this->sceneName);
	}

	ImGui::End();
}

void ScenePanel::RenderSubmeshTextureData(Ref<SubmeshData> submeshData, const std::string& submeshName)
{
	int removeIndex = -1;
	int textureIndex = 0;
	for (const Ref<SceneTextureData>& textureData : submeshData->textures)
	{
		std::string textureFile = textureData->texture->GetPath();
		textureFile = textureFile.substr(textureFile.find_last_of("\\") + 1);
		{
			std::stringstream ss;
			ss << "Texture: " << textureFile;
			ImGui::Text(ss.str().c_str());
		}
		{
			std::stringstream ss;
			ss << textureFile << " Texture Ratio For " << submeshName;
			ImGui::DragFloat(ss.str().c_str(), &textureData->ratio, 0.01f);
		}
		{
			std::stringstream ss;
			ss << textureFile << " Texture Coord Scale For " << submeshName;
			ImGui::DragFloat(ss.str().c_str(), &textureData->texCoordScale, 0.01f);
		}
		{
			std::stringstream ss;
			ss << "Remove " << textureFile << " From " << submeshName;
			if (ImGui::Button(ss.str().c_str()))
			{
				removeIndex = textureIndex;
			}
		}

		ImGui::NewLine();
		textureIndex++;
	}

	if (removeIndex != -1)
	{
		submeshData->textures.erase(submeshData->textures.begin() + removeIndex);
	}
	{
		std::stringstream ss;
		ss << "Add Texture To " << submeshName;
		if (ImGui::Button(ss.str().c_str()))
		{
			std::stringstream ss;
			ss << SOLUTION_DIR << "Extern\\assets\\textures\\" << this->textureName;
			const char* textureTypeString = textureTypes[this->selectedTexture];
			TextureFilterType filterType = StringToFilterType(filterTypes[this->selectedFilter]);
			TextureWrapType wrapType = StringToWrapType(wrapTypes[this->selectedWrap]);
			if (textureTypeString == "Diffuse")
			{
				Ref<DiffuseTexture> texture = TextureManager::LoadDiffuseTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				submeshData->AddTexture(texture);
			}
			else if (textureTypeString == "Heightmap")
			{
				Ref<HeightMapTexture> texture = TextureManager::LoadHeightmapTexture(ss.str(), filterType, wrapType, glm::vec3(0.0f, 0.0f, 0.0f), 1000.0f);
				submeshData->AddTexture(texture);
			}
			else if (textureTypeString == "Discard")
			{
				Ref<DiscardTexture> texture = TextureManager::LoadDiscardTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				submeshData->AddTexture(texture);
			}
			else if (textureTypeString == "Alpha")
			{
				Ref<AlphaTexture> texture = TextureManager::LoadAlphaTexture(ss.str(), filterType, wrapType, this->genMipMaps);
				submeshData->AddTexture(texture);
			}
		}
	}	
}