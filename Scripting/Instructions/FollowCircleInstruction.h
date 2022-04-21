#pragma once 

#include "Instruction.h"

class FollowCircleInstruction : public Instruction
{
public:
	FollowCircleInstruction(const std::string& handle, const glm::vec3& center, float speed, float radius, int startAngle, int endAngle, bool vertical = false);
	virtual ~FollowCircleInstruction() = default;

	virtual void Execute(float deltaTime, Scene* scene) override;

	virtual void Reset() override;

	inline bool IsComplete() const { return this->completed; }
private:
	void NextPoint(int angleIncrement);

	glm::vec3 center;
	float radius;
	float radiusSquared;
	float speed;
	int startAngle;
	int endAngle;
	int currentAngle;
	bool vertical;

	glm::vec3 desiredPosition;

	bool completed;
};