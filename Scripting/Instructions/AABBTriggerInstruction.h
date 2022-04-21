#pragma once 

#include "Instruction.h"

class AABBTriggerInstruction : public Instruction
{
public:
	AABBTriggerInstruction(const std::string& handle, const std::string& aabbHandle, Instruction* instruction);
	virtual ~AABBTriggerInstruction();

	virtual void Execute(float deltaTime, Scene* scene) override;

	virtual void Reset() override;

	inline bool IsComplete() const { return this->triggerInstruction->IsComplete(); }
private:
	Instruction* triggerInstruction;
	std::string aabbHandle;
	bool executeTriggerInstruction;
};