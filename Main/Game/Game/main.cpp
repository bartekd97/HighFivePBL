#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>

#include "LifeTime.h"
#include "Transform.h"
#include "CubeRenderer.h"
#include "CubeSpawner.h"

#include "Texture.h"
#include "Material.h"
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

	auto hwShader = ShaderManager::GetShader("HelloWorldShader");
	hwShader->use();
	hwShader->setInt("albedoMap", MaterialBindingPoint::ALBEDO_MAP);
	hwShader->setInt("normalMap", MaterialBindingPoint::NORMAL_MAP);
	hwShader->setInt("metalnessMap", MaterialBindingPoint::METALNESS_MAP);
	hwShader->setInt("roughnessMap", MaterialBindingPoint::ROUGHNESS_MAP);
	hwShader->setInt("emissiveMap", MaterialBindingPoint::EMISSIVE_MAP);

	//auto sampleTex = TextureManager::GetTexture("Sample", "albedo");
	//sampleTex->bind(1);

	auto sampleMat = MaterialManager::GetMaterial("Sample", "mat");
	sampleMat->apply();

	float dt = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		glfwPollEvents();

		HFEngine::ECS.UpdateSystems(dt);
		ReportGameObjects(dt);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//hwShader->use();
		//PrimitiveRenderer::DrawScreenQuad();
		HFEngine::ECS.RenderSystems();

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
