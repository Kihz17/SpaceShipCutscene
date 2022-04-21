#pragma once 

#include "Instruction.h"

class MoveToSpeedInstruction : public Instruction
{
public:
	MoveToSpeedInstruction(const std::string& handle, const glm::vec3& destination, float speed);
	virtual ~MoveToSpeedInstruction() = default;

	virtual void Execute(float deltaTime, Scene* scene) override;

	virtual void Reset() override;

	inline bool IsComplete() const { return this->completed; }
private:
	glm::vec3 destination;
	float speed;

	bool completed;
};