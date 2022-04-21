#include "SceneBoundingBox.h"
#include "YAMLOverloads.h"

#include <glm/gtc/matrix_transform.hpp> 


SceneBoundingBox::SceneBoundingBox(const std::string& name, glm::vec3 position, glm::vec3 min, glm::vec3 max)
	: GameObject(name), 
	position(position), 
	boundingBox(AABB(min, max)), 
	orientation(0.0f)
{

}

void SceneBoundingBox::Draw(Ref<Shader> shader)
{
	glm::mat4 transform(1.0f);
	transform *= glm::translate(glm::mat4(1.0f), this->position);
	transform *= glm::rotate(glm::mat4(1.0f), this->orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f), this->orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f), this->orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	this->boundingBox.Draw(shader, transform);
}

Ref<SceneBoundingBox> SceneBoundingBox::CreateDefault(const std::string& name, glm::vec3 position)
{
	return CreateRef<SceneBoundingBox>(name, position, glm::vec3(-5.0f, -5.0f, -5.0f), glm::vec3(5.0f, 5.0f, 5.0f));
}

void SceneBoundingBox::Save(YAML::Emitter& emitter) const
{
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "Name" << YAML::Value << this->friendlyName;
	emitter << YAML::Key << "Min" << YAML::Value << this->boundingBox.min;
	emitter << YAML::Key << "Max" << YAML::Value << this->boundingBox.max;
	emitter << YAML::Key << "Position" << YAML::Value << this->position;
	emitter << YAML::Key << "Orientation" << YAML::Value << this->orientation;
	emitter << YAML::EndMap;
}

Ref<SceneBoundingBox> SceneBoundingBox::StaticLoad(const YAML::Node& node)
{
	std::string name = node["Name"].as<std::string>();
	glm::vec3 min = node["Min"].as<glm::vec3>();
	glm::vec3 max = node["Max"].as<glm::vec3>();
	glm::vec3 position = node["Position"].as<glm::vec3>();
	glm::vec3 orientation = node["Orientation"].as<glm::vec3>();
	Ref<SceneBoundingBox> aabb = CreateRef<SceneBoundingBox>(name, position, min, max);
	aabb->orientation = orientation;
	return aabb;
}