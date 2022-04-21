#pragma once

#include "GLCommon.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp> 

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector> 
#include <fstream>
#include <sstream>

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_opengl3.h"
#include "vendor/imgui/imgui_impl_glfw.h"

#include "Camera.h"
#include "Light.h"
#include "EnvironmentMap.h"
#include "AABB.h"
#include "Raycast.h"
#include "Scene.h"
#include "Renderer.h"
#include "MeshManager.h"
#include "TextureManager.h"

const float windowWidth = 1700;
const float windowHeight = 800;
bool editMode = true;

Ref<Camera> camera = CreateRef<Camera>(windowHeight, windowWidth);
float moveSpeed = 10.1f;

Ref<Scene> scene = NULL;

static float getRandom(float low, float high)
{
	return low + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (high - low));
}

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Movement controls
	if (!editMode)
	{
		if (key == GLFW_KEY_W)
		{
			camera->position += camera->direction * moveSpeed;
		}
		if (key == GLFW_KEY_A)
		{
			// Rotate our camera's direction 90 degrees to the left
			glm::vec3 left;
			constexpr float theta = glm::radians(90.0f);
			left.x = camera->direction.x * cos(theta) + camera->direction.z * sin(theta);
			left.y = 0.0f;
			left.z = -camera->direction.x * sin(theta) + camera->direction.z * cos(theta);
			camera->position += left * moveSpeed;
		}
		if (key == GLFW_KEY_S)
		{
			camera->position -= camera->direction * moveSpeed;
		}
		if (key == GLFW_KEY_D)
		{
			// Rotate our camera's direction 90 degrees to the right
			glm::vec3 right;
			constexpr float theta = glm::radians(-90.0f);
			right.x = camera->direction.x * cos(theta) + camera->direction.z * sin(theta);
			right.y = 0.0f;
			right.z = -camera->direction.x * sin(theta) + camera->direction.z * cos(theta);
			camera->position += right * moveSpeed;
		}
		if (key == GLFW_KEY_SPACE)
		{
			camera->position.y += moveSpeed;
		}

	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		scene->debugMode = !scene->debugMode;
	}

	if (key == GLFW_KEY_PAGE_UP && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		scene->NextMesh();
	}
	else if (key == GLFW_KEY_PAGE_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		scene->PreviousMesh();
	}

	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		scene->NextLight();
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		scene->PreviousLight();
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		scene->NextAABB();
	}
	else if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		scene->PreviousAABB();
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		scene->showCurrentEdit = !scene->showCurrentEdit;
	}

	// Toggle cursor view
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		editMode = !editMode;
		int cursorOption = editMode ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, cursorOption);
		if (!editMode) // We switched to edit mode, set our last cursor pos
		{
			glfwSetCursorPos(glfwGetCurrentContext(), camera->lastWindowX, camera->lastWindowY);
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (!editMode)
	{
		camera->MoveCamera(xpos, ypos);
	}
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
	std::stringstream ss;
	ss << SOLUTION_DIR << "Extern\\assets\\models\\";
	std::string path(paths[0]);
	if (path.find(ss.str()) == std::string::npos)
	{
		std::cout << "Model import must be in the models directory!" << std::endl;
		return;
	}

	Ref<Mesh> mesh = MeshManager::LoadMesh(path);
	Ref<SceneMeshData> meshData = CreateRef<SceneMeshData>(GameObject::GenerateFriendlyName(), mesh);
	meshData->position = camera->position + (camera->direction * 10.0f);
	scene->AddMesh(meshData);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Initialize our window
	window = glfwCreateWindow(windowWidth, windowHeight, "Midterm", NULL, NULL);

	// Make sure the window initialized properly
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback); // Tell GLFW where our key callbacks are
	glfwSetCursorPosCallback(window, mouse_callback); // Tell GLFW where our mouse callback is
	glfwSetDropCallback(window, drop_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // Give glad this process ID
	glfwSwapInterval(1);

	// Initialize ImGui
	ImGui::CreateContext(); 
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 420");
	ImGui::StyleColorsDark();

	// Load shader
	std::stringstream ss;
	ss << SOLUTION_DIR << "Extern\\assets\\shaders\\vertexShader.glsl";
	std::string vertexPath = ss.str();
	ss.str("");

	ss << SOLUTION_DIR << "Extern\\assets\\shaders\\fragmentShader.glsl";
	std::string fragmentPath = ss.str();
	ss.str("");

	Ref<Shader> shader = CreateRef<Shader>("Shader#1", vertexPath, fragmentPath);
	scene = CreateRef<Scene>(shader);
	scene->camera = camera;

	float fpsFrameCount = 0.f;
	float fpsTimeElapsed = 0.f;
	float previousTime = static_cast<float>(glfwGetTime());

	Renderer::Initialize(shader);
	Light::InitializeUniforms(shader);
	DiffuseTexture::InitializeUniforms(shader);
	HeightMapTexture::InitializeUniforms(shader);
	DiscardTexture::InitializeUniforms(shader);
	AlphaTexture::InitializeUniforms(shader);

	// Our actual render loop
	while (!glfwWindowShouldClose(window))
	{
		float currentTime = static_cast<float>(glfwGetTime());
		float deltaTime = std::min(currentTime - previousTime, 1.0f);
		previousTime = currentTime;

		// FPS TITLE
		{
			fpsTimeElapsed += deltaTime;
			fpsFrameCount += 1.0f;
			if (fpsTimeElapsed >= 0.03f)
			{
				std::string fps = std::to_string(fpsFrameCount / fpsTimeElapsed);
				std::string ms = std::to_string(1000.f * fpsTimeElapsed / fpsFrameCount);
				std::string newTitle = "FPS: " + fps + "   MS: " + ms;
				glfwSetWindowTitle(window, newTitle.c_str());

	
				fpsTimeElapsed = 0.f;
				fpsFrameCount = 0.f;
			}
		}

		Renderer::BeginFrame(shader, camera);

		// Start imGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Safety, mostly for first frame
		if (deltaTime == 0.0f)
		{
			deltaTime = 0.03f;
		}

		scene->OnUpdate(camera, deltaTime);

		// Render imGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Renderer::EndFrame();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window); // Clean up the window

	glfwTerminate(); 
	exit(EXIT_SUCCESS);
}