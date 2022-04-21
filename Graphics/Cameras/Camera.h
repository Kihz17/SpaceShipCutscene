#pragma once

#include "GameObject.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <algorithm>  

class Camera : public GameObject
{
public:
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 up; // An vector that represents the up direction

	Camera(float windowWidth, float windowHeight);
	~Camera();

	float yaw; // Represents our horizontal camera rotation (Turning head)
	float pitch; // Represents our vertical camera rotation (Looking up and down)

	float lastWindowX; // Keeps track of our last x pos in the GLFW window so we can calculate the offset since the last frame
	float lastWindowY; // Keeps track of our last y pos in the GLFW window so we can calculate the offset since the last frame

	float sensitivity;

	// Move the camera around according to the mouse position in the window
	void MoveCamera(float mouseX, float mouseY);

	// Gets the Camera's view matrix according to yaw/pitch using glm::lookAt
	glm::mat4 GetViewMatrix();

	inline virtual glm::vec3 GetPositionSafe() const override { return this->position; };
	inline virtual glm::vec3& GetPosition() override { return this->position; };
	inline virtual glm::vec3 GetDirectionSafe() const override { return this->direction; }
	inline virtual glm::vec3& GetDirection() override { return this->direction; }
	inline virtual AABB& GetBoundingBox() override { return placeHolderAABB; };
	inline virtual AABB GetBoundingBoxSafe() const override { return placeHolderAABB; };

private:
	// Update's the camera's direction vector according to the yaw and pitch
	void UpdateDirection();
	AABB placeHolderAABB;
};