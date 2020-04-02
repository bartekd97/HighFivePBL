#include <iostream>

#include "Logger.h"
#include "LifeTimeSystem.h"
#include "HFEngine.h"

#include "../Components.h"

void LifeTimeSystem::Update(float dt)
{
	auto it = gameObjects.begin();
	while (it != gameObjects.end())
	{
		auto gameObject = *(it++);
		auto& lifeTime = HFEngine::ECS.GetComponent<LifeTime>(gameObject);
		lifeTime.accumulator += dt;
		if (isgreaterequal(lifeTime.accumulator, lifeTime.lifeTime))
		{
			HFEngine::ECS.DestroyGameObject(gameObject);
		}
	}
}
