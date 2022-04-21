#pragma once

#include "Instruction.h"

class FollowInstruction : public Instruction
{
public:
	FollowInstruction(const std::string& handle, const std::string& followHandle, float followDistance, float followSpeed, glm::vec3 followOffset = glm::vec3(0.0f, 0.0f, 0.0f), float followDurationSeconds = -1.0);
	virtual ~FollowInstruction() = default;

	virtual void Execute(float deltaTime, Scene* scene) override;

	virtual void Reset() override;

	inline bool IsComplete() const { return followDuration > 0.0f && currentFollowTime >= followDuration; } // We will follow for a certain amount of time (unless we are -1, in which case we follow forever)
private:
	std::string followHandle;
	float speed;
	glm::vec3 offset;
	float followDistanceSqr;

	float followDuration;
	float currentFollowTime;
};