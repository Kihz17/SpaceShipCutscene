#pragma once

#include "Scene.h"

#include <vector>

enum class InstructionSetType
{
	None = 0,
	Serial,
	Parallel,
	Async
};

class InstructionScheduler;
class InstructionSet
{
public:
	virtual ~InstructionSet() = default;

	virtual void Execute(float deltaTime, Scene* scene) = 0;
	virtual bool IsComplete() const = 0;

	virtual void IncrementRepeat() = 0;
	virtual bool IsRepeat() const = 0;

	virtual bool IsParallelExecute() const { return false; }
};