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
#include "Utility/TextureTools.h"

#include "HFEngine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "Event/EventManager.h"
#include "Event/Events.h"

#include "MapGenerator/MapGenerator.h"

#include "Resourcing/MeshFileLoader.h"

#include "GUI/Button.h"

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

	GLFWwindow* window = WindowManager::GetWindow();

	MapGenerator generator;
	generator.Generate();

	auto prefab = PrefabManager::GetPrefab("Player");
	auto movableTestObject = prefab->Instantiate({ 100.0f, 0.0f, 100.0f });
	HFEngine::ECS.SetNameGameObject(movableTestObject, "Player");

	auto prefabCircle = PrefabManager::GetPrefab("TestCircle");
	auto testCircleObject = prefabCircle->Instantiate({ 90.0f, 0.0f, 90.0f });

	// particle test
	//auto particles = HFEngine::ECS.CreateGameObject(movableTestObject);
	//HFEngine::ECS.GetComponent<Transform>(particles).SetPosition({ 5.0f, 0.1f, 0.0f });
	/*
	ParticleContainer container;
	container.SetMaxParticles(256);
	ParticleEmitter emitter;
	emitter.shape = ParticleEmitter::EmitterShape::CIRCLE;
	emitter.sourcShapeeSize = { 0.5f, 0.5f };
	emitter.targetShapeSize = { 0.5f, 0.5f };
	emitter.size = { 1.9f, 1.9f };
	emitter.lifetime = { 2.25f, 3.0f };
	emitter.velocity = { 0.5f, 0.75f };
	emitter.rate = 16.0f;
	emitter.emitting = true;
	ParticleRenderer renderer;
	renderer.colorOverTime = TextureTools::GenerateGradientTexture(
		{
			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f },
			{ 1.0f, 0.0f, 0.0f }
		}
	);
	renderer.opacityOverTime = TextureTools::GenerateGradientTexture({1.0f, 1.0f, 0.0f});
	renderer.spriteSheet = TextureManager::GetTexture("Particles", "Flames");
	renderer.spriteSheetCount = 4;
	HFEngine::ECS.AddComponent<ParticleContainer>(movableTestObject, container);
	HFEngine::ECS.AddComponent<ParticleEmitter>(movableTestObject, emitter);
	HFEngine::ECS.AddComponent<ParticleRenderer>(movableTestObject, renderer);
	*/

	auto testGuiObject = HFEngine::ECS.CreateGameObject("TestGUI");
	HFEngine::ECS.AddComponent<ScriptContainer>(testGuiObject, {});
	auto& tgScriptContainer = HFEngine::ECS.GetComponent<ScriptContainer>(testGuiObject);
	tgScriptContainer.AddScript(testGuiObject, "GUIStatistics");

	float dt = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		InputManager::PollEvents();
		//if (InputManager::GetKeyDown(GLFW_KEY_O)) HFEngine::ECS.DestroyGameObject(testCircleObject);//HFEngine::ECS.SetEnabledGameObject(testCircleObject, false);
		//doCameraMovement(cameraObject, dt);

		HFEngine::ProcessGameFrame(dt);

		ReportGameObjects(dt);

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
