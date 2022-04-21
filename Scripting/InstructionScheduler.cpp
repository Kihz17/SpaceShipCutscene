#include "InstructionScheduler.h"
#include "InstructionSet.h"
#include "Scene.h"

InstructionScheduler::InstructionScheduler()
{

}

InstructionScheduler::~InstructionScheduler()
{
	for (InstructionSet* set : this->queue)
	{
		delete set;
	}

	this->queue.clear();

	for (InstructionSet* set : this->parallelExecutes)
	{
		delete set;
	}

	this->parallelExecutes.clear();
}

void InstructionScheduler::Execute(float deltaTime, Scene* scene)
{
	if (!queue.empty())
	{
		InstructionSet* front = this->queue.front();
		if (front->IsParallelExecute()) // We are a parallel exec, this will continue to execute to completion while simultaneously moving on to another instruction set
		{
			this->parallelExecutes.push_back(front);
			this->queue.pop_front();
		}
		else if (front->IsComplete()) // Current set has finished executing, move on
		{
			if (!front->IsRepeat()) // No repeating, safe to remove
			{
				this->queue.pop_front();
				delete front; // No longer using this
			}
			else // Repeating, increment counter
			{
				front->IncrementRepeat();
			}
		}

		if (!this->queue.empty())
		{
			this->queue.front()->Execute(deltaTime, scene);
		}
	}

	// Execute parallels along with others
	std::vector<InstructionSet*>::iterator parallelIt = this->parallelExecutes.begin();
	while (parallelIt != this->parallelExecutes.end())
	{
		if ((*parallelIt)->IsComplete())
		{
			InstructionSet* set = (*parallelIt);
			parallelIt = this->parallelExecutes.erase(parallelIt);
			delete set; // No longer using
		}
		else
		{
			(*parallelIt)->Execute(deltaTime, scene);
			parallelIt++;
		}	
	}
}