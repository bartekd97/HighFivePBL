#include <iostream>
#include <random>

#include "../../Utility/Logger.h"
#include "CubeSpawnerSystem.h"
#include "HFEngine.h"

#include "../Components.h"

void CubeSpawnerSystem::Update(float dt)
{
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_real<float> randPosition(-8.0f, 8.0f);
	std::uniform_real<float> randColor(0.0f, 1.0f);

	for (auto const& gameObject : gameObjects)
	{
		auto& cubeSpawner = HFEngine::ECS.GetComponent<CubeSpawner>(gameObject);
		cubeSpawner.accumulator += dt;
		if (isgreater(cubeSpawner.accumulator, cubeSpawner.interval))
		{

			GameObject cube = HFEngine::ECS.CreateGameObject();
			HFEngine::ECS.AddComponent<Transform>(
				cube,
				{ { randPosition(generator), 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, { 1.0f, 1.0f, 1.0f } }
			);
			HFEngine::ECS.AddComponent<CubeRenderer>(
				cube,
				{ 0.1f, {randColor(generator), randColor(generator), randColor(generator)} }
			);
			HFEngine::ECS.AddComponent<RigidBody>(
				cube,
				{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } }
			);
			HFEngine::ECS.AddComponent<Gravity>(
				cube,
				{ { 0.0f, -3.0f, 0.0f } }
			);
			HFEngine::ECS.AddComponent<LifeTime>(
				cube,
				{ 5.0f, 0.0f }
			);
			cubeSpawner.accumulator = 0.0f;
		}
	}
}
