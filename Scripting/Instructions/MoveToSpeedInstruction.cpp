#include "MoveToSpeedInstruction.h"

MoveToSpeedInstruction::MoveToSpeedInstruction(const std::string& handle, const glm::vec3& destination, float speed)
	: Instruction(handle),
	destination(destination),
	speed(speed),
	completed(false)
{

}

void MoveToSpeedInstruction::Execute(float deltaTime, Scene* scene)
{
	Ref<GameObject> gameObject = GetGameObject(scene);
	glm::vec3& position = gameObject->GetPosition();

	glm::vec3 velocity = glm::normalize(this->destination - position) * this->speed;
	position += velocity * deltaTime;

	glm::vec3 difference = this->destination - position;
	if (glm::dot(difference, difference) <= 1.0f) // Arrived
	{
		this->completed = true;
	}
}

void MoveToSpeedInstruction::Reset()
{
	this->completed = false;
}