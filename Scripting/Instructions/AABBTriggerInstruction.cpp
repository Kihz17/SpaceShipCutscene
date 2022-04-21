#include "AABBTriggerInstruction.h"

AABBTriggerInstruction::AABBTriggerInstruction(const std::string& handle, const std::string& aabbHandle, Instruction* instruction)
	: Instruction(handle),
	aabbHandle(aabbHandle),
	triggerInstruction(instruction),
	executeTriggerInstruction(false)
{

}

AABBTriggerInstruction::~AABBTriggerInstruction()
{
	delete this->triggerInstruction;
}

void AABBTriggerInstruction::Execute(float deltaTime, Scene* scene)
{
	if (this->executeTriggerInstruction)
	{
		this->triggerInstruction->Execute(deltaTime, scene);
		return;
	}

	Ref<GameObject> gameObject = GetGameObject(scene);
	Ref<GameObject> aabb = scene->GetGameObject(this->aabbHandle);

	glm::mat4 transform(1.0f);
	glm::vec3& orientation = aabb->GetDirection();
	transform *= glm::translate(glm::mat4(1.0f), aabb->GetPosition());
	transform *= glm::rotate(glm::mat4(1.0f), orientation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f), orientation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transform *= glm::rotate(glm::mat4(1.0f), orientation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	if (aabb->GetBoundingBox().IsInside(transform, gameObject->GetPosition()))
	{
		this->executeTriggerInstruction = true;
	}
}

void AABBTriggerInstruction::Reset()
{
	this->executeTriggerInstruction = false;
	this->triggerInstruction->Reset();
}