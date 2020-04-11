#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>
#include <math.h>

#include "ECS/Components.h"

#include "Resourcing/Texture.h"
#include "Resourcing/Material.h"
#include "Resourcing/Model.h"
#include "Resourcing/Shader.h"
#include "Resourcing/Prefab.h"
#include "Rendering/PrimitiveRenderer.h"
#include "Utility/Logger.h"

#include "HFEngine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "Event/EventManager.h"
#include "Event/Events.h"

#include "MapGenerator/MapGenerator.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void ReportGameObjects(float dt);
void doCameraMovement(GameObject cameraObject, float dt);

int main()
{
	if (!HFEngine::Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "HFEngine test"))
	{
		std::cout << "Failed to initialize engine" << std::endl;
		return -1;
	}

	/*EventManager::AddListener(Events::Test::TICK, [](Event& event) {
		LogInfo("Tick event fired!");
	});*/

	/*GameObject cubeSpawner = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.AddComponent<CubeSpawner>(
		cubeSpawner,
		{ 0.05f, 0.0f }
	);*/

	GLFWwindow* window = WindowManager::GetWindow();

	MapGenerator generator;
	generator.Generate();


	auto spaceship = ModelManager::GetModel("Sample", "spaceship");
	MeshRenderer spaceshipRenderer = { spaceship->mesh, spaceship->material, true };


	GameObject ss1 = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.GetComponent<Transform>(ss1).SetPosition({ 0.0f, -1.0f, 0.0f });
	HFEngine::ECS.AddComponent<MeshRenderer>(ss1,spaceshipRenderer);

	GameObject ss2 = HFEngine::ECS.CreateGameObject(ss1);
	HFEngine::ECS.GetComponent<Transform>(ss2).SetPosition({ -6.0f, 2.0f, 0.0f });
	HFEngine::ECS.AddComponent<MeshRenderer>(ss2, spaceshipRenderer);

	GameObject ss3 = HFEngine::ECS.CreateGameObject(ss2, "Esese trzy");
	HFEngine::ECS.GetComponent<Transform>(ss3).SetPosition({ -6.0f, 2.0f, 0.0f });
	HFEngine::ECS.AddComponent<MeshRenderer>(ss3, spaceshipRenderer);

	GameObject ss4 = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.GetComponent<Transform>(ss4).SetPosition({ 4.0f, 0.0f, -8.0f });
	HFEngine::ECS.AddComponent<MeshRenderer>(ss4, spaceshipRenderer);

	HFEngine::ECS.GetComponent<Transform>(ss1).SetScale({ 0.4f, 0.4f, 0.4f });

	float enableTestInterval = 5.0f, accum = 0.0f;

	float dt = 0.0f;

	auto prefab = PrefabManager::GetPrefab("Sample");
	prefab->Instantiate({100,10,100});

	GameObject cameraObject = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.GetComponent<Transform>(cameraObject).SetPosition({ 100.0f, 25.0f, 100.0f });
	HFEngine::ECS.GetComponent<Transform>(cameraObject).SetRotation({ -45.0f, 0.0f, 0.0f });

	while (!glfwWindowShouldClose(window))
	{
		accum += dt;
		if (accum >= enableTestInterval)
		{
			accum = 0.0f;
			HFEngine::ECS.SetEnabledGameObject(ss2, !HFEngine::ECS.IsEnabledGameObject(ss2));
			EventManager::FireEvent(Events::Test::TICK);
		}

		auto startTime = std::chrono::high_resolution_clock::now();

		InputManager::PollEvents();

		doCameraMovement(cameraObject, dt);

		HFEngine::ECS.UpdateSystems(dt);
		ReportGameObjects(dt);

		HFEngine::ECS.GetComponent<Transform>(ss1).SetRotation({ 0.0f, (float)glfwGetTime() * 5.0f, 0.0f });
		HFEngine::ECS.GetComponent<Transform>(ss3).SetRotation({ 0.0f, (float)glfwGetTime() * 15.0f, 0.0f });
		HFEngine::ECS.GetComponent<Transform>(ss4).SetRotation({ 0.0f, (float)glfwGetTime() * -3.0f, 0.0f });

		HFEngine::MainCamera.SetView(HFEngine::ECS.GetComponent<Transform>(cameraObject));

		HFEngine::Renderer.Render();

		glfwSwapBuffers(window);
		
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
	}

	HFEngine::Terminate();

	return 0;
}

void ReportGameObjects(float dt)
{
	const float interval = 5.0f;
	static int frames = 0;
	static float accumulator = 0.0f;
	accumulator += dt;
	frames += 1;
	if (isgreaterequal(accumulator, interval))
	{
		LogInfo("FPS: {}; GameObjects count: {}", frames / interval, HFEngine::ECS.GetLivingGameObjectsCount());
		accumulator = 0.0f;
		frames = 0;
	}
}

void doCameraMovement(GameObject cameraObject, float dt)
{
	const float moveSpeed = 25.0f;
	const float rotateSpeed = 90.0f;
	Transform& trans = HFEngine::ECS.GetComponent<Transform>(cameraObject);

	if (InputManager::GetKeyStatus(GLFW_KEY_W))
		trans.TranslateSelf(moveSpeed * dt, trans.GetFront());
	if (InputManager::GetKeyStatus(GLFW_KEY_S))
		trans.TranslateSelf(moveSpeed * dt, -trans.GetFront());
	if (InputManager::GetKeyStatus(GLFW_KEY_A))
		trans.TranslateSelf(moveSpeed * dt, -trans.GetRight());
	if (InputManager::GetKeyStatus(GLFW_KEY_D))
		trans.TranslateSelf(moveSpeed * dt, trans.GetRight());
	if (InputManager::GetKeyStatus(GLFW_KEY_SPACE))
		trans.TranslateSelf(moveSpeed * dt, glm::vec3(0, 1, 0));
	if (InputManager::GetKeyStatus(GLFW_KEY_LEFT_SHIFT))
		trans.TranslateSelf(moveSpeed * dt, glm::vec3(0, -1, 0));

	if (InputManager::GetKeyStatus(GLFW_KEY_UP))
		trans.RotateSelf(rotateSpeed * dt, trans.GetRight());
	if (InputManager::GetKeyStatus(GLFW_KEY_DOWN))
		trans.RotateSelf(rotateSpeed * dt, -trans.GetRight());
	if (InputManager::GetKeyStatus(GLFW_KEY_LEFT))
		trans.RotateSelf(rotateSpeed * dt, glm::vec3(0, 1, 0));
	if (InputManager::GetKeyStatus(GLFW_KEY_RIGHT))
		trans.RotateSelf(rotateSpeed * dt, glm::vec3(0, -1, 0));
	/*
	if (input.getKeyStatus(GLFW_KEY_PAGE_UP))
		trans.rotateSelf(rotateSpeed * dt, glm::vec3(0, 0, 1));
	if (input.getKeyStatus(GLFW_KEY_PAGE_DOWN))
		trans.rotateSelf(rotateSpeed * dt, glm::vec3(0, 0, -1));
		*/
}