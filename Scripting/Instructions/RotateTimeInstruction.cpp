#include "RotateTimeInstruction.h"

RotateTimeInstruction::RotateTimeInstruction(const std::string& handle, const glm::vec3& rotation, float seconds)
	: Instruction(handle), 
	rotation(rotation),
	seconds(seconds), 
	rotationIncrements(0.0f), 
	initializeIncrements(true),
	completed(false),
	xDistance(0), 
	yDistance(0), 
	zDistance(0),
	currentXDistance(0), 
	currentYDistance(0), 
	currentZDistance(0)
{

}

void RotateTimeInstruction::Execute(float deltaTime, Scene* scene)
{
	if (completed)
	{
		return;
	}

	Ref<GameObject> gameObject = GetGameObject(scene);

	glm::vec3& orientation = gameObject->GetDirection();
	if (initializeIncrements)
	{
		this->initializeIncrements = false;

		float xRot = orientation.x + glm::radians(this->rotation.x);
		this->xDistance = xRot - orientation.x;
		this->rotationIncrements.x = xDistance / this->seconds;

		float yRot = orientation.y + glm::radians(this->rotation.y);
		this->yDistance = yRot - orientation.y;
		this->rotationIncrements.y = yDistance / this->seconds;

		float zRot = orientation.z + glm::radians(this->rotation.z);
		this->zDistance = zRot - orientation.z;
		this->rotationIncrements.z = zDistance / this->seconds;
	}

	glm::vec3 rotationIncrement = rotationIncrements * deltaTime;
	orientation += rotationIncrement;
	this->currentXDistance += std::abs(rotationIncrement.x);
	this->currentYDistance += std::abs(rotationIncrement.y);
	this->currentZDistance += std::abs(rotationIncrement.z);

	if (this->currentXDistance >= std::abs(this->xDistance) && this->currentYDistance >= std::abs(this->yDistance) && this->currentZDistance >= std::abs(this->zDistance))
	{
		completed = true;
	}
}

void RotateTimeInstruction::Reset()
{
	this->initializeIncrements = true;
	this->rotationIncrements = glm::vec3(0.0f);
	this->completed = false;
	this->xDistance = 0;
	this->yDistance = 0;
	this->zDistance = 0;
	this->currentXDistance = 0;
	this->currentYDistance = 0;
	this->currentZDistance = 0;
}