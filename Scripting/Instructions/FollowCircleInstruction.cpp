#include "FollowCircleInstruction.h"

FollowCircleInstruction::FollowCircleInstruction(const std::string& handle, const glm::vec3& center, float speed, float radius, int startAngle, int endAngle, bool vertical)
	: Instruction(handle),
	center(center),
	speed(speed),
	radius(radius),
	radiusSquared(radius * radius),
	startAngle(startAngle),
	endAngle(endAngle),
	currentAngle(startAngle),
	vertical(vertical),
	completed(false)
{
	NextPoint(0);
}

void FollowCircleInstruction::Execute(float deltaTime, Scene* scene)
{
	Ref<GameObject> gameObject = GetGameObject(scene);
	glm::vec3& position = gameObject->GetPosition();

	glm::vec3 difference = this->desiredPosition - position;
	if (glm::dot(difference, difference) <= 4.0f) // We have arrived at our desired point, pick a new spot
	{
		glm::vec3 oldDirection = desiredPosition - position;
		NextPoint(startAngle > endAngle ? -1 : 1); // Just move up in 1 degree increments
		glm::vec3 newDirection = desiredPosition - position;
		float angle = std::atan2(-newDirection.x, newDirection.z);
		if (startAngle > endAngle) angle *= -1.0;
		gameObject->GetDirection().y = angle;
	}

	if (this->currentAngle == this->endAngle) // We are nearing the end
	{
		difference = this->desiredPosition - position;
		if (glm::dot(difference, difference) <= 4.0f) // We have arrived at our end point
		{
			this->completed = true;
			return;
		}
	}

	glm::vec3 velocity = glm::normalize(desiredPosition - position) * this->speed;
	position += velocity * deltaTime;
}

void FollowCircleInstruction::Reset()
{
	this->completed = false;
	this->currentAngle = this->startAngle;
	NextPoint(0);
}

void FollowCircleInstruction::NextPoint(int angleIncrement)
{
	this->currentAngle += angleIncrement;

	float radians = glm::radians((float) currentAngle);
	if (vertical)
	{
		this->desiredPosition = glm::vec3(center.x + cos(radians) * radius, center.y + sin(radians) * radius, center.z);
	}
	else
	{
		this->desiredPosition = glm::vec3(center.x + cos(radians) * radius, center.y, center.z + sin(radians) * radius);
	}
}