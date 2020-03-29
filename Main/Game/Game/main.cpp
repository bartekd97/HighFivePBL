#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>

#include "LifeTime.h"

#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "PrimitiveRenderer.h"
#include "Logger.h"

#include "HFEngine.h"
#include "WindowManager.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int main()
{
	if (!HFEngine::Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "HFEngine test"))
	{
		std::cout << "Failed to initialize engine" << std::endl;
		return -1;
	}

	std::vector<GameObject> gameObjects(5);
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i] = HFEngine::ECS.CreateGameObject();

		HFEngine::ECS.AddComponent<LifeTime>(
			gameObjects[i],
			{ 0.0f, i * 5.0f }
		);
	}

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

		glClearColor(0.2f, 0.7f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		hwShader->use();
		PrimitiveRenderer::DrawScreenQuad();

		glfwSwapBuffers(window);
		
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
	}

	HFEngine::Terminate();

	return 0;
}
