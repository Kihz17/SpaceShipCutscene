#include "SerialInstructionSet.h"

#include "InstructionScheduler.h"

SerialInstructionSet::SerialInstructionSet(int numberOfRepeats, std::vector<Instruction*> instructions)
	: 
	instructions(instructions),
	currentExecutionIndex(0),
	repeatCount(0),
	numberOfRepeats(numberOfRepeats)
{

}

SerialInstructionSet::~SerialInstructionSet()
{
	for (Instruction* instruction : this->instructions)
	{
		delete instruction;
	}

	this->instructions.clear();
}

void SerialInstructionSet::Execute(float deltaTime, Scene* scene)
{
	if (this->instructions[currentExecutionIndex]->IsComplete())
	{
		this->currentExecutionIndex++;
	}

	if (this->currentExecutionIndex < this->instructions.size()) // Set has finished executing
	{
		this->instructions[currentExecutionIndex]->Execute(deltaTime, scene);
	}
}

void SerialInstructionSet::IncrementRepeat()
{ 
	this->currentExecutionIndex = 0;
	this->repeatCount++; 
	for (Instruction* instruction : this->instructions)
	{
		instruction->Reset();
	}
}