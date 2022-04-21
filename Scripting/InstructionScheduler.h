#pragma once

#include <list>
#include <vector>

class InstructionSet;
class Scene;
class InstructionScheduler
{
public:
	InstructionScheduler();
	virtual ~InstructionScheduler();

	void Execute(float deltaTime, Scene* scene);

	inline void PushInstructionSet(InstructionSet* set) { this->queue.push_back(set); }

	inline bool IsComplete() const { return queue.empty() && parallelExecutes.empty(); }

private:
	std::list<InstructionSet*> queue;
	std::vector<InstructionSet*> parallelExecutes;
};
