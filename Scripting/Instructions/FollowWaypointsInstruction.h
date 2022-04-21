#pragma once 

#include "Instruction.h"

#include <vector>

class FollowWaypointsInstruction : public Instruction
{
public:
	FollowWaypointsInstruction(const std::string& handle, float speed, std::vector<glm::vec3> points);
	virtual ~FollowWaypointsInstruction() = default;

	virtual void Execute(float deltaTime, Scene* scene) override;

	virtual void Reset() override;

	inline bool IsComplete() const { return this->completed; }
private:
	int currentPoint;
	std::vector<glm::vec3> points;
	float speed;

	bool completed;
};