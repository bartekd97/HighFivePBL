#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>
#include <math.h>

#include "LifeTime.h"

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

	auto ssShader = ShaderManager::GetShader("SpaceshipShader");
	ssShader->use();
	ssShader->setInt("albedoMap", MaterialBindingPoint::ALBEDO_MAP);
	ssShader->setInt("normalMap", MaterialBindingPoint::NORMAL_MAP);
	ssShader->setInt("metalnessMap", MaterialBindingPoint::METALNESS_MAP);
	ssShader->setInt("roughnessMap", MaterialBindingPoint::ROUGHNESS_MAP);
	ssShader->setInt("emissiveMap", MaterialBindingPoint::EMISSIVE_MAP);

	//auto sampleTex = TextureManager::GetTexture("Sample", "albedo");
	//sampleTex->bind(1);

	//auto sampleMat = MaterialManager::GetMaterial("Sample", "mat");

	auto spaceship = ModelManager::GetModel("Sample", "spaceship");
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 5.0f, 10.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)(SCREEN_WIDTH) / (float)(SCREEN_HEIGHT), 0.1f, 100.0f);

	//ssShader->setMat4("gModel", model);
	ssShader->setMat4("gView", view);
	ssShader->setMat4("gProjection", projection);

	float dt = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		glfwPollEvents();

		HFEngine::ECS.UpdateSystems(dt);

		glClearColor(0.2f, 0.7f, 0.5f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//hwShader->use();
		//PrimitiveRenderer::DrawScreenQuad();

		ssShader->use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
		ssShader->setMat4("gModel", model);

		spaceship->material->apply();
		spaceship->mesh->bind();
		spaceship->mesh->draw();

		glfwSwapBuffers(window);
		
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
	}

	HFEngine::Terminate();

	return 0;
}
