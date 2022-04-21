#pragma once

#include "Instruction.h"

class RotateSpeedInstruction : public Instruction
{
public:
	RotateSpeedInstruction(const std::string& handle, const glm::vec3& rotation, const glm::vec3& rotatioIncrements);

	virtual void Execute(float deltaTime, Scene* scene) override;
	inline bool IsComplete() const { return completed; }

	virtual void Reset() override;

private:
	glm::vec3 rotation;
	float speeds;

	float xDistance, yDistance, zDistance;
	float currentXDistance, currentYDistance, currentZDistance;
	glm::vec3 rotationIncrements;
	bool initializeIncrements;

	bool completed;
};