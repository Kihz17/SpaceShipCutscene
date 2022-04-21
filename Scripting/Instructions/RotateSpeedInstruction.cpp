#include "RotateSpeedInstruction.h"

RotateSpeedInstruction::RotateSpeedInstruction(const std::string& handle, const glm::vec3& rotation, const glm::vec3& rotationIncrements)
	: Instruction(handle),
	rotation(rotation),
	rotationIncrements(rotationIncrements),
	initializeIncrements(true),
	completed(false),
	xDistance(0),
	yDistance(0),
	zDistance(0),
	currentXDistance(0),
	currentYDistance(0),
	currentZDistance(0)
{
	this->rotationIncrements.z = glm::radians(rotationIncrements.x);
	this->rotationIncrements.y = glm::radians(rotationIncrements.y);
	this->rotationIncrements.z = glm::radians(rotationIncrements.z);
}

void RotateSpeedInstruction::Execute(float deltaTime, Scene* scene)
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

		float yRot = orientation.y + glm::radians(this->rotation.y);
		this->yDistance = yRot - orientation.y;

		float zRot = orientation.z + glm::radians(this->rotation.z);
		this->zDistance = zRot - orientation.z;
	}

	glm::vec3 rotationIncrement = rotationIncrements * deltaTime;

	bool finishedX = this->currentXDistance >= std::abs(this->xDistance);
	if (!finishedX)
	{
		orientation.x += rotationIncrement.x;
		this->currentXDistance += std::abs(rotationIncrement.x);
	}

	bool finishedY = this->currentYDistance >= std::abs(this->yDistance);
	if (!finishedY)
	{
		this->currentYDistance += std::abs(rotationIncrement.y);
		orientation.y += rotationIncrement.y;
	}

	bool finishedZ = this->currentZDistance >= std::abs(this->zDistance);
	if (!finishedZ)
	{
		orientation.z += rotationIncrement.z;
		this->currentZDistance += std::abs(rotationIncrement.z);
	}

	if (finishedX && finishedY && finishedZ)
	{
		completed = true;
	}
}

void RotateSpeedInstruction::Reset()
{
	this->initializeIncrements = true;
	this->completed = false;
	this->xDistance = 0;
	this->yDistance = 0;
	this->zDistance = 0;
	this->currentXDistance = 0;
	this->currentYDistance = 0;
	this->currentZDistance = 0;
}