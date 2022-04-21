#include "FollowWaypointsInstruction.h"

FollowWaypointsInstruction::FollowWaypointsInstruction(const std::string& handle, float speed, std::vector<glm::vec3> points)
	: Instruction(handle), speed(speed), points(points)
{

}

void FollowWaypointsInstruction::Execute(float deltaTime, Scene* scene)
{
	Ref<GameObject> gameObject = GetGameObject(scene);
	glm::vec3& position = gameObject->GetPosition();
	glm::vec3& destination = points[currentPoint];
	glm::vec3 difference = position - destination;
	if (glm::dot(difference, difference) <= 4.0f) // We have arrived
	{
		currentPoint++;
		if (currentPoint >= points.size())
		{
			completed = true;
			return;
		}

		destination = points[currentPoint];
		glm::vec3 newDirection = destination - position;
		float angle = std::atan2(-newDirection.x, newDirection.z);
		gameObject->GetDirection().y = angle;
	}

	glm::vec3 velocity = glm::normalize(destination - position) * speed;
	position += velocity * deltaTime;
}

void FollowWaypointsInstruction::Reset()
{
	currentPoint = 0;
	completed = false;
}