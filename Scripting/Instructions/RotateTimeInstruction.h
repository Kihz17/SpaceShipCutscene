#pragma once

#include "Instruction.h"

class RotateTimeInstruction : public Instruction
{
public:
	RotateTimeInstruction(const std::string& handle, const glm::vec3& rotation, float seconds);

	virtual void Execute(float deltaTime, Scene* scene) override;
	inline bool IsComplete() const { return completed; }

	virtual void Reset() override;

private:
	glm::vec3 rotation;
	float seconds;

	float xDistance, yDistance, zDistance;
	float currentXDistance, currentYDistance, currentZDistance;
	glm::vec3 rotationIncrements;
	bool initializeIncrements;

	bool completed;
};