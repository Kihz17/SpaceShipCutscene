#pragma once

#include "InstructionSet.h"
#include "Instruction.h"

class SerialInstructionSet : public InstructionSet
{
public:
	SerialInstructionSet(int numberOfRepeats, std::vector<Instruction*> instructions);
	virtual ~SerialInstructionSet();

	virtual void Execute(float deltaTime, Scene* scene) override;
	inline virtual bool IsComplete() const override { return this->currentExecutionIndex >= this->instructions.size(); }

	inline virtual void IncrementRepeat() override;
	inline virtual bool IsRepeat() const override { return repeatCount < numberOfRepeats || numberOfRepeats == -1; }

private:
	std::vector<Instruction*> instructions;

	int currentExecutionIndex;

	int repeatCount;
	int numberOfRepeats; // -1 for infinite, 0 for none
};