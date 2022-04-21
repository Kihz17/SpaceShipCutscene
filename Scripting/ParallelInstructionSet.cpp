#include "ParallelInstructionSet.h"

ParallelInstructionSet::ParallelInstructionSet(std::vector<Instruction*> instructions, bool parallelExecute)
	: instructions(instructions), parallelExecute(parallelExecute)
{

}

ParallelInstructionSet::~ParallelInstructionSet()
{
	for (Instruction* instruction : this->instructions)
	{
		delete instruction;
	}

	this->instructions.clear();
}

void ParallelInstructionSet::Execute(float deltaTime, Scene* scene)
{
	int completeCount = 0;
	for (Instruction* instruction : this->instructions)
	{
		if (instruction->IsComplete())
		{
			completeCount++;
			continue;
		}

		instruction->Execute(deltaTime, scene);
	}

	if (completeCount >= this->instructions.size())
	{
		this->allCompleted = true;
	}
}