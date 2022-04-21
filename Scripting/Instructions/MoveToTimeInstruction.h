#pragma once

#include "Instruction.h"

class MoveToTimeInstruction : public Instruction
{
public:
	MoveToTimeInstruction(const std::string& handle, const glm::vec3& destination, float seconds);
	MoveToTimeInstruction(const std::string& handle, const glm::vec3& destination, float seconds, float accelDesired, float accelTime, float decelDesired, float decelTime, float startVelocity);
	virtual ~MoveToTimeInstruction() = default;

	virtual void Execute(float deltaTime, Scene* scene) override;

	virtual void Reset() override;

	inline bool IsComplete() const { return this->completed; }
private:
	glm::vec3 destination;
	float seconds;
	float speed;
	glm::vec3 velocity;

	float accelDesired;
	float accelTime;
	glm::vec3 accelRate;
	bool accelerating;
	glm::vec3 positionAfterAccel;

	float decelDesired;
	float decelTime;
	glm::vec3 decelRate;
	glm::vec3 decelStartPos;
	bool decelerating;

	glm::vec3 travelVelocity;

	float startVelocityMult;
	bool calculateEase;

	bool completed;
};