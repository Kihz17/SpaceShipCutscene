#include "MoveToTimeInstruction.h"

#include <iostream>

MoveToTimeInstruction::MoveToTimeInstruction(const std::string& handle, const glm::vec3& destination, float seconds)
	: Instruction(handle), destination(destination), seconds(seconds), completed(false), calculateEase(true), startVelocityMult(1.0f)
{
}

MoveToTimeInstruction::MoveToTimeInstruction(const std::string& handle, const glm::vec3& destination, float seconds, float accelDesired, float accelTime, float decelDesired, float decelTime, float startVelocity)
	: Instruction(handle), 
	destination(destination), 
	seconds(seconds), 
	completed(false), 
	calculateEase(true),
	accelDesired(accelDesired), 
	accelTime(accelTime), 
	decelDesired(decelDesired), 
	decelTime(decelTime), 
	startVelocityMult(startVelocity),
	accelerating(true),
	decelerating(false),
	travelVelocity(0.0f)
{

}

void MoveToTimeInstruction::Execute(float deltaTime, Scene* scene)
{
	if (this->completed)
	{
		return;
	}

	Ref<GameObject> gameObject = GetGameObject(scene);
	glm::vec3& position = gameObject->GetPosition();

	if (calculateEase)
	{
		calculateEase = false;

		//// Calcualte acceleration
		glm::vec3 direction = glm::normalize(this->destination - position);
		//this->velocity = direction * this->startVelocityMult;
		//this->accelRate = ((direction * this->accelDesired) - velocity) / this->accelTime;

		//// Calculate velocity in between acceleration and deceleration end points
		//this->positionAfterAccel= position + accelRate * this->accelTime;
		//float travelTime = this->seconds - this->accelTime - this->decelTime;
		//float accelPointToEndDistance = glm::distance(this->destination, positionAfterAccel);
		//float inBetweenSpeed = accelPointToEndDistance / travelTime;
		//this->travelVelocity = direction * inBetweenSpeed;

		//// Calculate decleration
		//this->decelRate = -(direction * inBetweenSpeed) / this->decelTime;
		//this->decelStartPos = this->destination + decelRate * this->decelTime;

		this->velocity = direction * (glm::distance(position, this->destination) / this->seconds);
	}

	//if (accelerating)
	//{
	//	this->velocity += this->accelRate * deltaTime;
	//	float length = glm::length(this->velocity);
	//	std::cout << "Length: " << length << std::endl;
	//	std::cout << "Desired: " << accelDesired << std::endl;
	//	if (length >= this->accelDesired) // Finished accelerating
	//	{
	//		accelerating = false; 
	//	}

	//	position += this->velocity;

	//	/*glm::vec3 accelDifference = position - this->positionAfterAccel;
	//	if (glm::dot(accelDifference, accelDifference) <= 2.0f)
	//	{
	//		accelerating = false;
	//	}*/
	//}
	//else if (decelerating)
	//{
	//	this->velocity += this->decelRate * deltaTime;
	//}
	//else
	//{
	//	this->velocity = this->travelVelocity;
	//}

	position += this->velocity * deltaTime;

	//glm::vec3 decelDifference = position - this->decelStartPos;
	//if (!decelerating && glm::dot(decelDifference, decelDifference) <= 1.0f) // We are close to our decel being point, start decelerating on our next frame
	//{
	//	decelerating = true;
	//}

	glm::vec3 destinationDifference = position - this->destination;
	if (glm::dot(destinationDifference, destinationDifference) <= 1.0f) // We are close to our end point, complete
	{
		this->completed = true;
	}
}

void MoveToTimeInstruction::Reset()
{
	this->calculateEase = true;
	this->completed = false;
	this->accelerating = true;
	this->decelerating = false;
}