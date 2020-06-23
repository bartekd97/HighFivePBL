#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include <chrono>
#include <math.h>
#include <cxxopts.hpp>

#include "ECS/Components.h"
#include "Utility/Logger.h"

#include "HFEngine.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "Event/EventManager.h"
#include "Event/Events.h"

#include "GUI/Button.h"
#include "Audio/AudioManager.h"

#include "Scene/SceneManager.h"
#include "Scene/Scenes/Game.h"
#include "Scene/Scenes/GameLite.h"
#include "Scene/Scenes/MainMenu.h"

//const int SCREEN_WIDTH = 1280;
//const int SCREEN_HEIGHT = 720;

void ReportGameObjects(float dt);

int main(int argc, char** argv)
{
	cxxopts::Options options("HFEngine", "HFEngine configuration");

	options.add_options()
		("width", "Screen width", cxxopts::value<int>()->default_value("1280"))
		("height", "Screen height", cxxopts::value<int>()->default_value("720"))
		("fullscreen", "Enable fullscreen mode", cxxopts::value<bool>()->default_value("false"))
		;

	auto result = options.parse(argc, argv);

	HFEngine::HFEngineConfigStruct config;
	config.WindowTitle = "Unholy Survivor: Reckoning";
	config.ScreenWidth = result["width"].as<int>();
	config.ScreenHeight = result["height"].as<int>();
	config.FullscreenMode = result["fullscreen"].as<bool>();


	//if (!HFEngine::Initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Unholy Survivor: Reckoning"))
	if (!HFEngine::Initialize(config))
	{
		std::cout << "Failed to initialize engine" << std::endl;
		return -1;
	}

	GLFWwindow* window = WindowManager::GetWindow();

	// register scenes
	SceneManager::RegisterScene("Game", std::make_shared<GameScene>());
	SceneManager::RegisterScene("GameLite", std::make_shared<GameLiteScene>());
	SceneManager::RegisterScene("MainMenu", std::make_shared<MainMenuScene>());

	//AudioManager ac;
	//ac.Init_al();

	
	//ALuint source2;
	//ac.CreateDefaultSourceAndPlay(source2, "glass_ping", true, 0.01f);

	// request initial scene
	AudioManager::PlayBackground("menuKorpecki", 1.0f);
	SceneManager::RequestLoadScene("MainMenu");

	float dt = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		InputManager::PollEvents();

		HFEngine::ProcessGameFrame(dt);

		ReportGameObjects(dt);

		glfwSwapBuffers(window);

		ModelManager::UnloadUnused();
		
		auto stopTime = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float, std::chrono::seconds::period>(stopTime - startTime).count();
	}

	//ac.Exit_al();

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