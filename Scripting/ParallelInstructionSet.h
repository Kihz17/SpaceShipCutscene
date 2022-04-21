#pragma once

#include "InstructionSet.h"
#include "Instruction.h"

class ParallelInstructionSet : public InstructionSet
{
public:
	ParallelInstructionSet(std::vector<Instruction*> instructions, bool parallelExecute = false);
	virtual ~ParallelInstructionSet();

	virtual void Execute(float deltaTime, Scene* scene) override;
	inline virtual bool IsComplete() const override { return this->allCompleted; }

	virtual bool IsParallelExecute() const override { return this->parallelExecute; }

	// Parallel sets can't repeat
	inline virtual void IncrementRepeat() override { }
	inline virtual bool IsRepeat() const override { return false; }

private:
	std::vector<Instruction*> instructions;

	bool parallelExecute;

	bool allCompleted;
};