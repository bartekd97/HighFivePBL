#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>

#include "LifeTime.h"
#include "LifeTimeSystem.h"
#include "ECSCore.h"

#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "PrimitiveRenderer.h"
#include "Logger.h"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

ECSCore gECSCore;

int main()
{
	LoggerInitialize();

	gECSCore.Init();
	gECSCore.RegisterComponent<LifeTime>();
	auto lifeTimeSystem = gECSCore.RegisterSystem<LifeTimeSystem>();
	{
		Signature signature;
		signature.set(gECSCore.GetComponentType<LifeTime>());
		gECSCore.SetSystemSignature<LifeTimeSystem>(signature);
	}

	std::vector<GameObject> gameObjects(5);
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i] = gECSCore.CreateGameObject();

		gECSCore.AddComponent<LifeTime>(
			gameObjects[i],
			{ 0.0f, i * 5.0f }
		);
	}

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", nullptr, nullptr);

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}



	//glViewport(0,0, 1280, 720);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	ShaderManager::Initialize();
	TextureManager::Initialize();
	MaterialManager::Initialize();

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

		gECSCore.UpdateSystems(dt);

		glClearColor(0.2f, 0.7f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		hwShader->use();
		PrimitiveRenderer::DrawScreenQuad();

		glfwSwapBuffers(window);
		
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
	}

	glfwTerminate();
	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}