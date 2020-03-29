#include <iostream>

#include "Logger.h"
#include "LifeTimeSystem.h"
#include "LifeTime.h"
#include "HFEngine.h"

void LifeTimeSystem::Update(float dt)
{
	for (auto const& gameObject : gameObjects)
	{
		auto& lifeTime = HFEngine::ECS.GetComponent<LifeTime>(gameObject);
		lifeTime.lifeTime += dt;
		if (isgreater(lifeTime.threshold, 0.0f))
		{
			if (isgreaterequal(lifeTime.lifeTime, lifeTime.threshold))
			{
				lifeTime.lifeTime = 0.0f;
				//std::cout << "[LifeTime] GameObject " << gameObject << " reached lifetime threshold (" << lifeTime.threshold << ")" << std::endl;
				LogInfo("[LifeTime] GameObject {} reached lifetime threshold: {}", gameObject, lifeTime.threshold);
			}
		}
	}
}
