#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>
#include <math.h>

#include "Components.h"

#include "Texture.h"
#include "Material.h"
#include "Model.h"
#include "Shader.h"
#include "PrimitiveRenderer.h"
#include "Logger.h"

#include "HFEngine.h"
#include "WindowManager.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void ReportGameObjects(float dt);

int main()
{
	if (!HFEngine::Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "HFEngine test"))
	{
		std::cout << "Failed to initialize engine" << std::endl;
		return -1;
	}

	GameObject cubeSpawner = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.AddComponent<CubeSpawner>(
		cubeSpawner,
		{ 0.05f, 0.0f }
	);

	GLFWwindow* window = WindowManager::GetWindow();

	/*
	auto ssShader = ShaderManager::GetShader("SpaceshipShader");
	ssShader->use();
	ssShader->setInt("albedoMap", MaterialBindingPoint::ALBEDO_MAP);
	ssShader->setInt("normalMap", MaterialBindingPoint::NORMAL_MAP);
	ssShader->setInt("metalnessMap", MaterialBindingPoint::METALNESS_MAP);
	ssShader->setInt("roughnessMap", MaterialBindingPoint::ROUGHNESS_MAP);
	ssShader->setInt("emissiveMap", MaterialBindingPoint::EMISSIVE_MAP);
	*/

	//auto sampleTex = TextureManager::GetTexture("Sample", "albedo");
	//sampleTex->bind(1);

	//auto sampleMat = MaterialManager::GetMaterial("Sample", "mat");

	auto spaceship = ModelManager::GetModel("Sample", "spaceship");
	MeshRenderer spaceshipRenderer = { spaceship->mesh, spaceship->material, true };
	/*
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT), 0.1f, 100.0f);
	*/
	//ssShader->setMat4("gModel", model);
	//ssShader->setMat4("gView", view);
	//ssShader->setMat4("gProjection", projection);

	GameObject ss1 = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.AddComponent<Transform>(
		ss1,
		{ { 0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, { 1.0f, 1.0f, 1.0f } }
	);
	HFEngine::ECS.AddComponent<MeshRenderer>(ss1,spaceshipRenderer);

	GameObject ss2 = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.AddComponent<Transform>(
		ss2,
		{ { 4.0f, 2.75f, 0.0f}, {0.0f, 0.0f, 0.0f}, { 0.5f, 0.5f, 0.5f } }
	);
	HFEngine::ECS.AddComponent<MeshRenderer>(ss2, spaceshipRenderer);

	GameObject ss3 = HFEngine::ECS.CreateGameObject();
	HFEngine::ECS.AddComponent<Transform>(
		ss3,
		{ { -4.0f, 2.75f, 0.0f}, {0.0f, 0.0f, 0.0f}, { 0.5f, 0.5f, 0.5f } }
	);
	HFEngine::ECS.AddComponent<MeshRenderer>(ss3, spaceshipRenderer);

	float dt = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		glfwPollEvents();

		HFEngine::ECS.UpdateSystems(dt);
		ReportGameObjects(dt);

		HFEngine::ECS.GetComponent<Transform>(ss1).rotation = { 0.0f, (float)glfwGetTime(), 0.0f };
		HFEngine::ECS.GetComponent<Transform>(ss2).rotation = { 0.0f, (float)glfwGetTime(), 0.0f };
		HFEngine::ECS.GetComponent<Transform>(ss3).rotation = { 0.0f, (float)glfwGetTime(), 0.0f };

		/*
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		HFEngine::ECS.RenderSystems();

		//hwShader->use();
		//PrimitiveRenderer::DrawScreenQuad();

		ssShader->use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
		ssShader->setMat4("gModel", model);

		spaceship->material->apply();
		spaceship->mesh->bind();
		spaceship->mesh->draw();
		*/

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
