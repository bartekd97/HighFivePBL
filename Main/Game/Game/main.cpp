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

#include "Resourcing/MeshFileLoader.h"

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

	/*GameObject ss1 = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.GetComponent<Transform>(ss1).SetPosition({ 100.0f, 2.0f, 100.0f });
	HFEngine::ECS.AddComponent<MeshRenderer>(ss1, spaceshipRenderer);*/

	//HFEngine::ECS.GetComponent<Transform>(ss1).SetScale({ 0.4f, 0.4f, 0.4f });

	float dt = 0.0f;

	//auto prefab = PrefabManager::GetPrefab("Sample");
	//prefab->Instantiate({100,10,100});

	auto prefab = PrefabManager::GetPrefab("CircleTest");
	auto movableTestObject = prefab->Instantiate({ 100.0f, 1.0f, 100.0f });
	HFEngine::ECS.SetNameGameObject(movableTestObject, "Player");
	prefab->Instantiate({ 80.0f, 1.0f, 100.0f });

	auto demon = ModelManager::GetModel("Characters/Player", "Demon");
	SkinnedMeshRenderer demonRenderer = { demon->mesh, demon->material, demon->skinningData };
	HFEngine::ECS.RemoveComponent<MeshRenderer>(movableTestObject);
	HFEngine::ECS.AddComponent<SkinnedMeshRenderer>(movableTestObject, demonRenderer);
	HFEngine::ECS.AddComponent<SkinAnimator>(movableTestObject, SkinAnimator());
	HFEngine::ECS.GetComponent<SkinAnimator>(movableTestObject).clips = demon->animations;
	HFEngine::ECS.GetComponent<SkinAnimator>(movableTestObject).SetAnimation("running");
	HFEngine::ECS.GetComponent<Transform>(movableTestObject).SetScale(glm::vec3(3.0f));

	auto bigPrefab = PrefabManager::GetPrefab("BigCircleTest");
	bigPrefab->Instantiate({ 90.0f, 1.0f, 120.0f });

	auto triggerColliderTest = PrefabManager::GetPrefab("TriggerTest");
	auto triggerBox = triggerColliderTest->Instantiate({ 105.0f, 0.0f, 90.0f });
	HFEngine::ECS.GetComponent<Transform>(triggerBox).RotateSelf(45, glm::vec3(0.0f, 1.0f, 0.0f));

	HFEngine::ECS.AddComponent<ScriptContainer>(movableTestObject, {});
	auto& scriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(movableTestObject);
	scriptContainer.AddScript(movableTestObject, "CharControllerTest");
	scriptContainer.AddScript(movableTestObject, "MapCellOptimizer");

	GameObject cameraObject = HFEngine::ECS.CreateGameObject("CameraObject");
	HFEngine::ECS.GetComponent<Transform>(cameraObject).SetPosition({ 100.0f, 30.0f, 120.0f });
	HFEngine::ECS.GetComponent<Transform>(cameraObject).SetRotation({ -50.0f, 0.0f, 0.0f });
	//HFEngine::ECS.GetComponent<Transform>(cameraObject).SetPosition({ 100.0f, 200.0f, 100.0f });
	//HFEngine::ECS.GetComponent<Transform>(cameraObject).SetRotation({ -90.0f, 0.0f, 0.0f });

	while (!glfwWindowShouldClose(window))
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		InputManager::PollEvents();

		//doCameraMovement(cameraObject, dt);

		HFEngine::ECS.UpdateSystems(dt);
		ReportGameObjects(dt);

		HFEngine::MainCamera.SetView(HFEngine::ECS.GetComponent<Transform>(cameraObject));

		HFEngine::Renderer.Render();

		glfwSwapBuffers(window);

		ModelManager::UnloadUnused();
		
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
