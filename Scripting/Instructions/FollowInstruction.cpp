#include "FollowInstruction.h"

FollowInstruction::FollowInstruction(const std::string& handle, const std::string& followHandle, float followDistance, float followSpeed, glm::vec3 followOffset, float followDurationSeconds)
	: Instruction(handle),
	followHandle(followHandle),
	followDistanceSqr(followDistance * followDistance), // convert to squared
	offset(followOffset),
	speed(followSpeed),
	followDuration(followDurationSeconds * 1000.0f), // convert to millis
	currentFollowTime(0.0f)
{

}

void FollowInstruction::Execute(float deltaTime, Scene* scene)
{
	Ref<GameObject> thisObject = GetGameObject(scene);
	Ref<GameObject> followGameObject = scene->GetGameObject(this->followHandle);

	glm::vec3& position = thisObject->GetPosition();
	glm::vec3 difference = (followGameObject->GetPosition() + this->offset) - position;
	float distSquared = glm::dot(difference, difference);
	if (distSquared <= this->followDistanceSqr) // We are close enough, no need to move
	{
		return;
	}

	glm::vec3 velocity = glm::normalize(difference) * this->speed;
	position += velocity * deltaTime;
	this->currentFollowTime += deltaTime;
}

void FollowInstruction::Reset()
{
	this->currentFollowTime = 0.0f;
}